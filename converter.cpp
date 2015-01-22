#include <QDebug>
#include <QUrl>
#include <QFileInfo>
#include <qmath.h>
#include <qglobal.h>

#include "converter.h"
#include "xdathdf.h"

#define MAX_INTVAL 2147483647

void myRunFunction(std::vector<std::vector<float> > *const retVec, int nAtoms)
{
    for(int k = 0; k< 24000 ; k++)
    {
        std::vector<float> tmp;
        tmp.clear();
        for(int i = 0; i <= 100; ++i)
        {
            //qDebug() << nAtoms << " " << i << "from" << QThread::currentThread();
            tmp.push_back(i*nAtoms);
        }


        retVec->push_back(tmp);
    }
    qDebug() << "Vec Size" << retVec->size();
}

Converter::Converter(QObject *parent) :
    QObject(parent)
{
    _posFilename.clear();
    _specFilename.clear();
    _xdatFileText.clear();
    _statusText.clear();
    _abortExport = false;

    _atomicSystem = new AtomicSystem();

    QObject::connect(this, SIGNAL(specFileChanged()), this, SLOT(loadSpeciesFile()));
    QObject::connect(this, SIGNAL(posFileChanged()), this, SLOT(loadPositionsFile()));
    QObject::connect(this, SIGNAL(exportCompleted()), this, SLOT(postExportCalls()));
}

QString Converter::posFile() const
{
    return _posFilename;
}

void Converter::setPosFile(const QString &_filename)
{
    if(_posFilename.compare(_filename) != 0)
    {
        _posFilename = _filename;
        emit posFileChanged();
    }
}

QString Converter::specFile() const
{
    return _specFilename;
}

void Converter::setSpecFile(const QString &_filename)
{
    if(_specFilename.compare(_filename) != 0)
    {
        _specFilename = _filename;

        //qDebug() << _filename;
        //fflush(stdout);

        emit specFileChanged();
    }
}

QString Converter::positionsText() const
{
    return _xdatFileText;
}

QString Converter::statusText() const
{
    return _statusText;
}

void Converter::setStatusText(const QString &_text)
{
    if(_statusText.compare(_text) != 0)
    {
        _statusText = _text;

        emit statusTextChanged();
    }
}

bool Converter::abortExport() const
{
    return _abortExport;
}

void Converter::setAbortExport(const bool &_value)
{
    if(_abortExport != _value)
    {
        _abortExport = _value;
        emit abortExportChanged();
    }
}

AtomicSystem* Converter::atomicSystem() const
{
    return _atomicSystem;
}

QQmlListProperty<SpeciesInfo> Converter::speciesList()
{
    return QQmlListProperty<SpeciesInfo>(this, 0, &Converter::appendFunc,
                                         &Converter::countFunc, &Converter::atFunc, 0);
}

QList<SpeciesInfo*> Converter::speciesQList()
{
    return SpeciesList;
}

void Converter::export2Hdf()
{
    //exportPositionsData();
    //return;

    // process file to read and export each time frame
    //processPositionsPerTimeFrame(readIn, _atomicSystem->numOfAtoms(), int(MAX_INTVAL));
    setStatusText("");

    QFuture<void> t1 = QtConcurrent::run(this, &Converter::exportPositionsData);
    //t1.waitForFinished();
}

size_t Converter::exportPositionsData()
{
    QUrl posfileUrl(_posFilename);

    QFile file(posfileUrl.toLocalFile());
    if(!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
#ifdef QT_DEBUG
        qDebug() << "Could not open/read positions file!";
#endif
        file.close();
        setStatusText(" Error: unable to open file " + posfileUrl.fileName());
        emit exportCompleted();
        return 0;
    }

    // Check if species file is also loaded
    if(SpeciesList.empty() || atomicSystem() == NULL)
    {
        setStatusText(" Atomic species information is missing!");
        emit exportCompleted();
        return 0;
    }

    // Check if the number of atoms in species file matches with the positions file
    if(atomicSystem()->numOfAtoms() != checkSpeciesHygiene())
    {
        setStatusText(" Number of atoms mismatch between Species\n and XDAT files");
        emit exportCompleted();
        return 0;
    }


    try
    {
        /// Read the positions file to get System Info.
        QTextStream readIn(&file);

        /// Define output file path and name
        QString outFilename;
        QFileInfo fi(posfileUrl.toLocalFile());
        outFilename = fi.path() + "/" + fi.fileName().section(".", 0, 0) + ".h5";

        /// process file to read and export each time frame
        size_t nSteps = processPositionsPerTimeFrame(readIn, _atomicSystem->numOfAtoms(), int(MAX_INTVAL));
        atomicSystem()->setNumTimeFrames((int)nSteps);

        /// Close file pointer
        file.close();

        if(_positions.empty() || nSteps == 0)
        {
            // Print Error message saying 0 time steps converted
            setStatusText(tr(" Could not read/process any time frame."));
            return 0;
        }

        /**************************************************************************/
        // Write positions arrays into HDF5 dataset
        XDAT2HDF *xhdfWriter = new XDAT2HDF();
        herr_t errStatus;

        hsize_t *dims = new hsize_t[3];
        dims[0] = _atomicSystem->numOfAtoms();
        dims[1] = 3;
        dims[2] = _positions.size();

        errStatus = xhdfWriter->createH5File(outFilename);
        errStatus = xhdfWriter->createRootGroup(atomicSystem()->systemName());

        /// write system information as root groups attributes
        xhdfWriter->writeRootGrpAttributes(*_atomicSystem, xhdfWriter->getRootGrpId());
        xhdfWriter->writeTimeSteps(xhdfWriter->getRootGrpId(), "POSITIONS", 3,
                                   (const hsize_t*)dims, speciesQList(), _positions, H5T_IEEE_F32BE);

        /// write species information as POSITIONS dataset attributes
        errStatus = xhdfWriter->closeRootGroup();
        errStatus = xhdfWriter->closeH5File();

        if(!errStatus)
            setStatusText(QString(" Export to HDF5 format complete.\n %1 time frame(s) exported ").arg(_positions.size()));

        // Deallocate memory and pointers
        for(size_t i = 0; i < _positions.size(); ++i)
            std::vector<float>(_positions[i]).swap(_positions[i]);
        std::vector<std::vector<float> >(_positions).swap(_positions);
        //_positions.shrink_to_fit();
        delete xhdfWriter;
    }
    catch(std::exception &ex)
    {
        if(file.isOpen())
            file.close();

        qDebug() << "Exception thrown @" << __FILE__ << __LINE__
                 << "function" << __FUNCTION__
                 << ":" << ex.what();
    }

    if(file.isOpen())
        file.close();

    emit exportCompleted();
    return _positions.size();
}

void Converter::loadPositionsFile()
{
    resetQmlUI();

    // Load valid positions file into array of species
    // First convert file dialog returned url to the QUrl and then get the
    // local file path for QFile. This process will remove html protocal "file://"
    // from specified file path.
    QUrl posfileUrl(_posFilename);

    QFile file(posfileUrl.toLocalFile());
    if(!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
#ifdef QT_DEBUG
        qDebug() << "Could not open/read positions file!";
#endif
        setStatusText(" Error: unable to open file " + posfileUrl.fileName());
        file.close();
        return;
    }

    // Peek 1MB into positions file to display in TextArea
    QByteArray ba = file.peek(1024 * 32);
    QString strTextArea(ba);

    // Handles either case of XDAT files with Konfig or Config
    int idx_K = strTextArea.lastIndexOf("Konfig");
    int idx_C = strTextArea.lastIndexOf("Config");
    strTextArea.truncate(idx_C > idx_K ? idx_C : idx_K);
    if(idx_C == -1 && idx_K == -1)
    {
        setStatusText(" Error: invalid XDAT positions file!");
        file.close();
        return;
    }
    _xdatFileText = strTextArea;
    emit positionsTextChanged();

    ////////////////////////////////////////////////
    try
    {
        // Read the positions file to get System Info.
        QTextStream readIn(&file);
        QString line = readIn.readLine();
        QStringList strList = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // If first line stores the number of time steps in the system
        if(strList.size() <= 1)
        {
            _atomicSystem->setNumTimeFrames(strList[0].toInt());

            line = readIn.readLine();
            strList = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

            if(strList[0] == strList[1])
            {
                _atomicSystem->setNumOfAtoms(strList[0].toInt());
            }
        }
        else if(strList.size() > 1 && strList[0] == strList[1]) // Else the first line stores number of atoms
        {
            _atomicSystem->setNumTimeFrames(0);
            _atomicSystem->setNumOfAtoms(strList[0].toInt());
        }

        // Read volume per atoms; lattice dimension; delta time
        line = readIn.readLine();
        strList = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        _atomicSystem->setVolPerAtom(strList[0].toDouble());
        _atomicSystem->setDeltaTime(strList[4].toDouble() * qPow(10.0, 15.0));

        QList<QVariant> dimList;
        dimList.push_back(strList[1].toDouble() * qPow(10.0, 10.0));
        dimList.push_back(strList[2].toDouble() * qPow(10.0, 10.0));
        dimList.push_back(strList[3].toDouble() * qPow(10.0, 10.0));
        _atomicSystem->setLatticeDim(QVariant::fromValue(dimList));

        // Read temperature
        line = readIn.readLine();
        strList = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        _atomicSystem->setKelvin(strList[0].toDouble());

        // Read simulated system name
        line = readIn.readLine();
        line = readIn.readLine();
        _atomicSystem->setSystemName(line.trimmed());

    }
    catch(std::exception &ex)
    {
        file.close();
        qDebug() << "Exception thrown @" << __FILE__ << __LINE__
                 << "function" << __FUNCTION__
                 << ":" << ex.what();
    }

    // Close file handler
    if(file.isOpen())
        file.close();

    emit atomicSystemChanged();
}

void Converter::loadSpeciesFile()
{
    setStatusText("");
    // Clear species list container!
    SpeciesList.clear();

    // Load valid species file into array of species
    // First convert file dialog returned url to the QUrl and then get the
    // local file path for QFile. This process will remove html protocal "file://"
    // from specified file path.
    QUrl speciesfileUrl(_specFilename);

    QFile file(speciesfileUrl.toLocalFile());
    if(!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        setStatusText(" Error: unable to open file " + speciesfileUrl.fileName());
        file.close();
        return;
    }

    // Open text stream to read lines from the file
    QTextStream readIn(&file);

    // Read first line of the file
    QString line = readIn.readLine();
    if(!line.startsWith("#Species"))
    {
#ifdef QT_DEBUG
        qDebug() << "Error: invalid species file!";
#endif
        setStatusText(" Error: invalid species file!");
        file.close();
        return;
    }

    while(!readIn.atEnd())
    {
        line = readIn.readLine();

        if(line.startsWith("#"))
            continue;

        QStringList strList = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        //SpeciesInfo specInfo;
        SpeciesInfo *specInfo = new SpeciesInfo();

        specInfo->setSymbol(strList[0]);
        specInfo->setAtomId(strList[1].toInt());
        specInfo->setBeginIndex(strList[2].toInt());
        specInfo->setEndIndex(strList[3].toInt());

        SpeciesList.push_back(specInfo);
    }

#ifdef QT_DEBUG
    qDebug() << "Species file information: ";
    foreach (SpeciesInfo *sp, SpeciesList)
    {
        qDebug() << sp->symbol() << sp->atomId() << sp->beginIndex() << sp->endIndex();
    }
#endif
    // Close file handler
    file.close();

    // If Species list has changed!!
    emit speciesListChanged();
}

void Converter::postExportCalls()
{
    setAbortExport(true);
}

/* Callback functions */
void Converter::appendFunc(QQmlListProperty<SpeciesInfo> *list, SpeciesInfo *item)
{
    Converter *convObj = qobject_cast<Converter *>(list->object);
    if(convObj)
    {
#ifdef QT_DEBUG
        qDebug() << "Converter : I exists! from AppendFunc";
#endif
        item->setParent(convObj);
        convObj->SpeciesList.append(item);
    }
}

SpeciesInfo* Converter::atFunc(QQmlListProperty<SpeciesInfo> *list, int index)
{
    Converter *convObj = qobject_cast<Converter *>(list->object);
    if(convObj)
    {
#ifdef QT_DEBUG
        qDebug() << "Converter : I exists! from atFunc" ;
#endif
        return convObj->SpeciesList.at(index);
    }

    return 0;
}

int Converter::countFunc(QQmlListProperty<SpeciesInfo> *list)
{
    Converter *convObj = qobject_cast<Converter *>(list->object);
    int count = 0;

    if(convObj)
    {
        count = convObj->SpeciesList.count();
#ifdef QT_DEBUG
        qDebug() << "Converter : I exists! from CountFunc" << count;
#endif
    }

    return count;
}

int Converter::checkSpeciesHygiene()
{
    if(SpeciesList.empty())
        return -1;

    int count = 0;
    foreach (SpeciesInfo* sinfo, SpeciesList) {
        if(sinfo->endIndex() >= sinfo->beginIndex())
            count += sinfo->endIndex() - sinfo->beginIndex() +1;
        else
            return -1;
    }

    return count;
}

size_t Converter::processPositionsPerTimeFrame(QTextStream &_reader, const int &nAtoms, const int &nSteps)
{
    if(_reader.status() != QTextStream::Ok)
    {
        qDebug() << "Error: reading input file via text stream. Text Stream not OK!";
        return 0;
    }
    else if(nAtoms < 1 || nSteps < 1)
    {
        setStatusText(" Unable to process input file...Invalid parameters! ");
#ifdef QT_DEBUG
        qDebug() << "Unable to process input file. Invalid parameters!!";
#endif
        return 0;
    }

    QString line;
    size_t countFrame = 0;

    ///std::vector<std::vector<float> > positions;
    _positions.clear();

    // Read till the end of file/textstream
    while(!_reader.atEnd() && countFrame < (size_t)nSteps && !abortExport())
    {
         line = _reader.readLine().trimmed();

         // Beginning of time frame
         if(line.startsWith("Konfig") || line.startsWith("Config"))
         {
             //float *posArray = new float[nAtoms * 3];
             std::vector<float> posArray;
             posArray.resize(nAtoms * 3);

             if(!(countFrame%1000))
                 setStatusText(" Exporting time frames " + QString::number(countFrame));

             for(size_t i = 0; i < (size_t)nAtoms; ++i)
             {
                 line = _reader.readLine().trimmed();

                 //if(line.startsWith("Konfig") || line.startsWith("Config"))
                 //    break;

                 QStringList strList = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                 if(strList.size() < 3)
                     continue;

                 posArray[i * 3 + 0] = strList[0].toFloat();
                 posArray[i * 3 + 1] = strList[1].toFloat();
                 posArray[i * 3 + 2] = strList[2].toFloat();
             }

             _positions.push_back(posArray);

             countFrame++;
             //delete [] posArray;
         }
         else
             continue;
    }
    Q_ASSERT(countFrame == _positions.size());

    return countFrame;
}

//size_t Converter::processPositionsPerTimeFrame(const QFile &_filePtr , const int &nAtoms, const int &nSteps)
//{
//    return 0;
//}

void Converter::resetQmlUI()
{
    setStatusText("");

    _xdatFileText = "";
    emit positionsTextChanged();
}
