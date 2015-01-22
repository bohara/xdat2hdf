#ifndef ATOMICSYSTEM_H
#define ATOMICSYSTEM_H

#include <QObject>
#include <QFile>
#include <QList>
#include <QTextStream>
#include <QQmlListProperty>

class AtomicSystem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString systemName READ systemName WRITE setSystemName NOTIFY systemNameChanged)
    Q_PROPERTY(double deltaTime READ deltaTime WRITE setDeltaTime NOTIFY deltaTimeChanged)
    Q_PROPERTY(double kelvin READ kelvin WRITE setKelvin NOTIFY kelvinChanged)
    Q_PROPERTY(double volPerAtom READ volPerAtom WRITE setVolPerAtom NOTIFY volPerAtomChanged)
    Q_PROPERTY(int numOfAtoms READ numOfAtoms WRITE setNumOfAtoms NOTIFY numOfAtomsChanged)
    Q_PROPERTY(int numTimeFrames READ numTimeFrames WRITE setNumTimeFrames NOTIFY numTimeFramesChanged)

    //Q_PROPERTY(QQmlListProperty<double> latticeDim READ latticeDim NOTIFY latticeDimChanged)
    Q_PROPERTY(QVariant latticeDim READ latticeDim WRITE setLatticeDim NOTIFY latticeDimChanged)

public:
    explicit AtomicSystem(QObject *parent = 0);
    AtomicSystem(const AtomicSystem &_other);
    AtomicSystem &operator =(const AtomicSystem &_other);

    QString systemName() const;
    double deltaTime() const;
    double volPerAtom() const;
    double kelvin() const;
    int numOfAtoms() const;
    int numTimeFrames() const;
    QVariant latticeDim() const;

    void setSystemName(const QString &value);
    void setDeltaTime(const double &value);
    void setVolPerAtom(const double &value);
    void setKelvin(const double &value);
    void setNumOfAtoms(const int &value);
    void setNumTimeFrames(const int &value);
    void setLatticeDim(const QVariant &value);

public slots:
    QVariant getLatticeDim() const;

signals:
    void systemNameChanged();
    void deltaTimeChanged();
    void kelvinChanged();
    void volPerAtomChanged();
    void numOfAtomsChanged();
    void numTimeFramesChanged();
    void latticeDimChanged();

private:
    QString _systemName;
    double _deltaTime;
    double _volPerAtom;
    double _kelvin;
    int _numOfAtoms;
    int _numTimeFrames;

    //QList<double> _latticeDims;
    QList<QVariant> _latticeDims;

};

#endif // ATOMICSYSTEM_H
