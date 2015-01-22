#ifndef XDATHDF_H
#define XDATHDF_H

#include <hdf5.h>
#include <QString>

#include "atomicsystem.h"
#include "speciesinfo.h"

class XDAT2HDF
{
public:
    XDAT2HDF();

    /* create a hdf5 file and store the file identifier in _fileId */
    int createH5File(const QString &_name);

    int createRootGroup(const QString &_name);

    /* close the hdf5 file with identifier _fileId */
    int closeH5File();

    int closeRootGroup();

    hid_t getRootGrpId();

    void writeRootGrpAttributes(const AtomicSystem &_atomicSys, const hid_t &_groupId);

    void writeDatasetAttributes(const QList<SpeciesInfo*> &_speciesInfo, const hid_t &_dSetId);

    /* write positions array into the hdf5 file *
     * use hyperslab/chunking techniques? */
    void writeTimeSteps(const int &_locId, const QString &_name, const unsigned int &_rank,
                       const hsize_t *dims, const QList<SpeciesInfo*> &_speciesInfo,
                       const std::vector<std::vector<float> > &_data, const int &_dataType);

private:
    hid_t _fileId;
    hid_t _rootGrpId;
    hid_t _dSetId;

};

#endif // XDATHDF_H
