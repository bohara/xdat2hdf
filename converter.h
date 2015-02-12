#ifndef CONVERTER_H
#define CONVERTER_H

#include <QObject>
#include <QFile>
#include <QList>
#include <QTextStream>
#include <QQmlListProperty>
#include <QtConcurrent>

#include "speciesinfo.h"
#include "atomicsystem.h"

/***
typedef struct
{
    unsigned int _elemId;
    unsigned int _elemBegin;
    unsigned int _elemEnd;
    QString _elemSymbol;

} SpeciesInfo;
*/

/***/
class Converter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString posFile READ posFile WRITE setPosFile NOTIFY posFileChanged)
    Q_PROPERTY(QString specFile READ specFile WRITE setSpecFile NOTIFY specFileChanged)
    Q_PROPERTY(QString positionsText READ positionsText NOTIFY positionsTextChanged)
    Q_PROPERTY(QString statusText READ statusText WRITE setStatusText NOTIFY statusTextChanged)
    Q_PROPERTY(bool abortExport READ abortExport WRITE setAbortExport NOTIFY abortExportChanged)
    Q_PROPERTY(AtomicSystem* atomicSystem READ atomicSystem NOTIFY atomicSystemChanged)

    Q_PROPERTY(QQmlListProperty<SpeciesInfo> speciesList READ speciesList NOTIFY speciesListChanged)

public:
    explicit Converter(QObject *parent = 0);

    QString posFile() const;
    void setPosFile(const QString &_filename);

    QString specFile() const;
    void setSpecFile(const QString &_filename);

    QString positionsText() const;

    QString statusText() const;
    void setStatusText(const QString &_text);

    bool abortExport() const;
    void setAbortExport(const bool &_value);

    AtomicSystem* atomicSystem() const;

    QQmlListProperty<SpeciesInfo> speciesList();
    QList<SpeciesInfo*> speciesQList();
    //void setSpeciesList(QQmlListProperty<SpeciesInfo> list);

    //Q_INVOKABLE int getCount();
    //Q_INVOKABLE SpeciesInfo* getSpeciesAt(int index);

signals:
    void posFileChanged();
    void specFileChanged();
    void speciesListChanged();
    void positionsTextChanged();
    void statusTextChanged();
    void atomicSystemChanged();
    void abortExportChanged();
    void exportCompleted();

public slots:
    void loadSpeciesFile();
    void loadPositionsFile();
    void export2Hdf();
    void postExportCalls();
    //int getCount();

private:
    QString _posFilename;
    QString _specFilename;
    QString _xdatFileText;
    QString _statusText;
    bool    _abortExport;

    //std::vector<SpeciesInfo> SpeciesList;
    QList<SpeciesInfo*> SpeciesList;
    AtomicSystem *_atomicSystem;
    std::vector<std::vector<float> > _positions;    // vector of array of atom coordinates per timestep
    std::vector<std::vector<float> > _displacement; // vector of displacement array(row vector of floats) over time per atom
    std::vector<float> _avgDisplacement;            // two dimensional array of float to represent average displacement

    static void appendFunc(QQmlListProperty<SpeciesInfo> *list, SpeciesInfo *item);
    static SpeciesInfo* atFunc(QQmlListProperty<SpeciesInfo> *list, int index);
    static int countFunc(QQmlListProperty<SpeciesInfo> *list);

    float calculateDisplacement(const std::vector<float>&p1, const std::vector<float>&p2);
    float calculateDisplacement(const float *p1, const float *p2);
    int checkSpeciesHygiene();
    void computeAverageDisplacement(const int &binSize);
    size_t exportPositionsData();
    size_t processPositionsPerTimeFrame(QTextStream &_reader, const int &nAtoms, const int &nSteps);
    //size_t processPositionsPerTimeFrame(const QFile &_filePtr, const int &nAtoms, const int &nSteps);
    void resetQmlUI();    
    void unPeriodify(const float *curPos, float *prevPos);
};

#endif // CONVERTER_H
