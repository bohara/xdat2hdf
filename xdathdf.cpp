#include <QDebug>

#include "xdathdf.h"

XDAT2HDF::XDAT2HDF()
{
    _fileId = -1;
    _rootGrpId = -1;
    _dSetId = -1;

    // Initialize HDF5 library
    herr_t errStatus = H5open();
    if(errStatus < 0)
        qDebug() << "Unable to initialize HDF5 library! Abort converter process!!";
}

int XDAT2HDF::createH5File(const QString &_name)
{
    // Create H5 file
    _fileId = H5Fcreate(_name.toStdString().c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    return (_fileId < 0) ? _fileId  : 0;
}

int XDAT2HDF::createRootGroup(const QString &_name)
{
    try{
        if(H5Lexists(_fileId, _name.toStdString().c_str(), H5P_DEFAULT) == 1)
            _rootGrpId = H5Gopen(_fileId, _name.toStdString().c_str(), H5P_DEFAULT);
        else
            _rootGrpId = H5Gcreate(_fileId, _name.toStdString().c_str(), H5P_DEFAULT,
                                   H5P_DEFAULT, H5P_DEFAULT);
    }
    catch(std::exception &ex)
    {
        qDebug() << "\tException : @" << __FILE__ << __LINE__ << __FUNCTION__
                 << ex.what();
    }
    return (_rootGrpId < 0) ? _rootGrpId : 0;
}

int XDAT2HDF::closeH5File()
{
    return H5Fclose(_fileId);
}

int XDAT2HDF::closeRootGroup()
{
    return H5Gclose(_rootGrpId);
}

hid_t XDAT2HDF::getRootGrpId()
{
    return (_rootGrpId > 0) ? _rootGrpId : 0;
}

void XDAT2HDF::writeRootGrpAttributes(const AtomicSystem &_atomicSys, const hid_t &_groupId)
{
    hid_t attribId, attribSpace;
    hid_t errStatus;

    attribSpace = H5Screate(H5S_SCALAR);

    double dAttrib = _atomicSys.deltaTime();
    attribId = H5Acreate(_groupId, "Delta Time (femtosecond)", H5T_IEEE_F64LE, attribSpace, H5P_DEFAULT, H5P_DEFAULT);
    errStatus = H5Awrite(attribId, H5T_NATIVE_DOUBLE, &dAttrib);
    errStatus = H5Aclose(attribId);

    int iAttrib = _atomicSys.kelvin();
    attribId = H5Acreate(_groupId, "Temperature (Kelvin)", H5T_STD_I32LE, attribSpace, H5P_DEFAULT, H5P_DEFAULT);
    errStatus = H5Awrite(attribId, H5T_NATIVE_INT32, &iAttrib);
    errStatus = H5Aclose(attribId);

    dAttrib = _atomicSys.volPerAtom();
    attribId = H5Acreate(_groupId, "Volume per atom", H5T_IEEE_F64LE, attribSpace, H5P_DEFAULT, H5P_DEFAULT);
    errStatus = H5Awrite(attribId, H5T_NATIVE_DOUBLE, &dAttrib);
    errStatus = H5Aclose(attribId);

    iAttrib = _atomicSys.numOfAtoms();
    attribId = H5Acreate(_groupId, "Number of Atoms", H5T_STD_I32LE, attribSpace, H5P_DEFAULT, H5P_DEFAULT);
    errStatus = H5Awrite(attribId, H5T_NATIVE_INT32, &iAttrib);
    errStatus = H5Aclose(attribId);

    iAttrib = _atomicSys.numTimeFrames();
    attribId = H5Acreate(_groupId, "Number of Frames", H5T_STD_I32LE, attribSpace, H5P_DEFAULT, H5P_DEFAULT);
    errStatus = H5Awrite(attribId, H5T_NATIVE_INT32, &iAttrib);
    errStatus = H5Aclose(attribId);

    dAttrib = _atomicSys.latticeDim().toList().at(0).toDouble();
    attribId = H5Acreate(_groupId, "Lattice X (\u212b)", H5T_IEEE_F64LE, attribSpace, H5P_DEFAULT, H5P_DEFAULT);
    errStatus = H5Awrite(attribId, H5T_NATIVE_DOUBLE, &dAttrib);
    errStatus = H5Aclose(attribId);

    dAttrib = _atomicSys.latticeDim().toList().at(1).toDouble();
    attribId = H5Acreate(_groupId, "Lattice Y (\u212b)", H5T_IEEE_F64LE, attribSpace, H5P_DEFAULT, H5P_DEFAULT);
    errStatus = H5Awrite(attribId, H5T_NATIVE_DOUBLE, &dAttrib);
    errStatus = H5Aclose(attribId);

    dAttrib = _atomicSys.latticeDim().toList().at(2).toDouble();
    attribId = H5Acreate(_groupId, "Lattice Z (\u212b)", H5T_IEEE_F64LE, attribSpace, H5P_DEFAULT, H5P_DEFAULT);
    errStatus = H5Awrite(attribId, H5T_NATIVE_DOUBLE, &dAttrib);
    errStatus = H5Aclose(attribId);

    errStatus = H5Sclose(attribSpace);
}

void XDAT2HDF::writeDatasetAttributes(const QList<SpeciesInfo *> &_speciesInfo, const hid_t &_dSetId)
{
    hid_t attribId, attribSpace;
    hid_t errStatus;

    int rank = 2;
    hsize_t dims[2] = {1, 3};

    attribSpace = H5Screate_simple(rank, dims, NULL);

    foreach (SpeciesInfo* si, _speciesInfo) {
        unsigned int speciesIndices[3] = {si->atomId(), si->beginIndex(), si->endIndex()};
        attribId = H5Acreate(_dSetId, si->symbol().toStdString().c_str(), H5T_STD_U32LE,
                             attribSpace, H5P_DEFAULT, H5P_DEFAULT);
        errStatus = H5Awrite(attribId, H5T_NATIVE_UINT, speciesIndices);
        errStatus = H5Aclose(attribId);
    }
    errStatus = H5Sclose(attribSpace);
}

void XDAT2HDF::writeDisplacementData(const int &_locId, const QString &_name,
                                     const unsigned int &_rank, const hsize_t *dims,
                                     const QList<SpeciesInfo *> &_speciesInfo,
                                     const std::vector<float> &_data, const int &_dataType)
{
    std::string dSetName = _name.toStdString();
    herr_t errStatus;
    hid_t dataSpace;
    hid_t memSpace;
    hid_t typeId;

    // Open a dataset if one already exists at given location
    // Else create a new one with given parameters
    //if(H5Lexists(_locId, dSetName.c_str(), H5P_DEFAULT) == 1)
    //    _dSetId = H5Dopen(_locId, dSetName.c_str(), H5P_DEFAULT);
    //else
    //{
    //}

    hsize_t *maxDims = new hsize_t[_rank];
    hsize_t *cDims = new hsize_t[_rank];
    hid_t dcplId = H5Pcreate(H5P_DATASET_CREATE);
    hid_t daplId = H5Pcreate(H5P_DATASET_ACCESS);

    for(unsigned int i = 0 ; i < _rank; ++i)
        maxDims[i] = H5S_UNLIMITED ;

    /// Create dataspace for dataset
    dataSpace = H5Screate_simple(_rank, dims, NULL);
    typeId = H5Tcopy(_dataType);

    /// Define chunk dimensions
    cDims[0] = dims[0];
    cDims[1] = dims[1];

    /// Set fill value of dataset to 0
    int fillvalue = 0;
    errStatus = H5Pset_fill_value(dcplId, typeId, &fillvalue);

    /// Set chunk cache and dimensions
    errStatus = H5Pset_chunk_cache(daplId, 12421, cDims[0] * cDims[1] * 2 *100,
                                   H5D_CHUNK_CACHE_W0_DEFAULT);
    errStatus = H5Pset_chunk(dcplId, _rank, cDims);

    /// Create dataset given the parameter of datasets and dataspaces
    _dSetId = H5Dcreate(_locId, dSetName.c_str(), typeId, dataSpace, H5P_DEFAULT,
                        dcplId, daplId);

    try
    {
        hid_t nativeType = H5Tget_native_type(typeId, H5T_DIR_ASCEND);

        /// Assuming the dataset is three dimensional, write data of 3rd dimension
        /// in layers using hyperslab of 1st and 2nd dimensions
        hsize_t memOffset[2];
        hsize_t memCount[2];
        hsize_t dsetOffset[3];
        hsize_t dsetCount[3];

        //std::vector<float> *dataArray = (std::vector<float>) (*_data);

        //for(size_t frame = 0; frame < nframes; ++frame)
        //{
        memOffset[0] = 0;
        memOffset[1] = 0;
        memCount[0] = dims[0];
        memCount[1] = dims[1];
        // memCount is used as dataspace dimension
        memSpace = H5Screate_simple(2, memCount, NULL);
        //errStatus = H5Sselect_hyperslab(memSpace, H5S_SELECT_SET, memOffset, NULL,
        //                                memCount, NULL);

        dsetOffset[0] = 0;
        dsetOffset[1] = 0;
        //dsetOffset[2] = frame;
        dsetCount[0] = dims[0];
        dsetCount[1] = dims[1];
        dsetCount[2] = 1;
        //errStatus = H5Sselect_hyperslab(dataSpace, H5S_SELECT_SET, dsetOffset, NULL,
        //                                dsetCount, NULL);

        errStatus = H5Dwrite(_dSetId, nativeType, memSpace, dataSpace, H5P_DEFAULT,
                             (const void*)&_data[0]);

        //}

        //      errStatus = H5Dwrite(_dSetId, nativeType, H5S_ALL, H5S_ALL, H5P_DEFAULT,
        //                           (const void*)_data);

        errStatus =  H5Tclose(nativeType);
    }
    catch(std::exception &ex)
    {
        qDebug() << "\tException : @" << __FILE__ << __LINE__ << __FUNCTION__
                 << ex.what();
    }

    writeDatasetAttributes(_speciesInfo, _dSetId);

    // close functions
    errStatus = H5Tclose(typeId);
    errStatus = H5Sclose(dataSpace);
    errStatus = H5Sclose(memSpace);
    errStatus = H5Pclose(dcplId);
    errStatus = H5Pclose(daplId);
    errStatus = H5Dclose(_dSetId);
}

void XDAT2HDF::writeTimeSteps(const int &_locId, const QString &_name,
                              const unsigned int &_rank, const hsize_t *dims,
                              const QList<SpeciesInfo *> &_speciesInfo,
                              const std::vector<std::vector<float> > &_data, const int &_dataType)
{
    std::string dSetName = _name.toStdString();
    herr_t errStatus;
    hid_t dataSpace;
    hid_t memSpace;
    hid_t typeId;

    // Open a dataset if one already exists at given location
    // Else create a new one with given parameters
    //if(H5Lexists(_locId, dSetName.c_str(), H5P_DEFAULT) == 1)
    //    _dSetId = H5Dopen(_locId, dSetName.c_str(), H5P_DEFAULT);
    //else
    //{
    //}

    hsize_t *maxDims = new hsize_t[_rank];
    hsize_t *cDims = new hsize_t[_rank];
    hid_t dcplId = H5Pcreate(H5P_DATASET_CREATE);
    hid_t daplId = H5Pcreate(H5P_DATASET_ACCESS);

    for(unsigned int i = 0 ; i < _rank; ++i)
        maxDims[i] = H5S_UNLIMITED ;

    /// Create dataspace for dataset
    dataSpace = H5Screate_simple(_rank, dims, NULL);
    typeId = H5Tcopy(_dataType);

    /// Define chunk dimensions
    cDims[0] = dims[0];
    cDims[1] = dims[1];
    cDims[2] = 1;

    /// Set fill value of dataset to 0
    int fillvalue = 0;
    errStatus = H5Pset_fill_value(dcplId, typeId, &fillvalue);

    /// Set chunk cache and dimensions
    errStatus = H5Pset_chunk_cache(daplId, 12421, cDims[0] * cDims[1] * 2 *100,
                                   H5D_CHUNK_CACHE_W0_DEFAULT);
    errStatus = H5Pset_chunk(dcplId, 3, cDims);

    /// Create dataset given the parameter of datasets and dataspaces
    _dSetId = H5Dcreate(_locId, dSetName.c_str(), typeId, dataSpace, H5P_DEFAULT,
                        dcplId, daplId);

    try
    {
        hid_t nativeType = H5Tget_native_type(typeId, H5T_DIR_ASCEND);

        /// Assuming the dataset is three dimensional, write data of 3rd dimension
        /// in layers using hyperslab of 1st and 2nd dimensions
        size_t nframes = dims[_rank-1];
        hsize_t memOffset[2];
        hsize_t memCount[2];
        hsize_t dsetOffset[3];
        hsize_t dsetCount[3];

        //std::vector<float> *dataArray = (std::vector<float>) (*_data);

        for(size_t frame = 0; frame < nframes; ++frame)
        {
            memOffset[0] = 0;
            memOffset[1] = 0;
            memCount[0] = dims[0];
            memCount[1] = dims[1];
            // memCount is used as dataspace dimension
            memSpace = H5Screate_simple(2, memCount, NULL);
            errStatus = H5Sselect_hyperslab(memSpace, H5S_SELECT_SET, memOffset, NULL,
                                            memCount, NULL);

            dsetOffset[0] = 0;
            dsetOffset[1] = 0;
            dsetOffset[2] = frame;
            dsetCount[0] = dims[0];
            dsetCount[1] = dims[1];
            dsetCount[2] = 1;
            errStatus = H5Sselect_hyperslab(dataSpace, H5S_SELECT_SET, dsetOffset, NULL,
                                            dsetCount, NULL);

            errStatus = H5Dwrite(_dSetId, nativeType, memSpace, dataSpace, H5P_DEFAULT,
                                 (const void*)&_data[frame][0]);

        }

        //      errStatus = H5Dwrite(_dSetId, nativeType, H5S_ALL, H5S_ALL, H5P_DEFAULT,
        //                           (const void*)_data);

        errStatus =  H5Tclose(nativeType);
    }
    catch(std::exception &ex)
    {
        qDebug() << "\tException : @" << __FILE__ << __LINE__ << __FUNCTION__
                 << ex.what();
    }

    writeDatasetAttributes(_speciesInfo, _dSetId);

    // close functions
    errStatus = H5Tclose(typeId);
    errStatus = H5Sclose(dataSpace);
    errStatus = H5Sclose(memSpace);
    errStatus = H5Pclose(dcplId);
    errStatus = H5Pclose(daplId);
    errStatus = H5Dclose(_dSetId);
}
