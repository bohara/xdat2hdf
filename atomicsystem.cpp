#include "atomicsystem.h"

AtomicSystem::AtomicSystem(QObject *parent) :
    QObject(parent)
{
    _systemName.clear();
    _deltaTime = 0;
    _volPerAtom = 0;
    _kelvin = 0;
    _numOfAtoms = 0;
    _numTimeFrames = 0;

    _latticeDims.clear();
    _latticeDims.append("");
    _latticeDims.append("");
    _latticeDims.append("");
}

AtomicSystem::AtomicSystem(const AtomicSystem &_other)
{
    _systemName     = _other._systemName;
    _deltaTime      = _other._deltaTime;
    _volPerAtom     = _other._volPerAtom;
    _kelvin         = _other._kelvin;
    _numOfAtoms     = _other._numOfAtoms;
    _numTimeFrames  = _other._numTimeFrames;
}

AtomicSystem & AtomicSystem::operator=(const AtomicSystem &_other)
{
    if(this == &_other)
        return *this;

    _systemName     = _other._systemName;
    _deltaTime      = _other._deltaTime;
    _volPerAtom     = _other._volPerAtom;
    _kelvin         = _other._kelvin;
    _numOfAtoms     = _other._numOfAtoms;
    _numTimeFrames  = _other._numTimeFrames;

    return *this;
}

QString AtomicSystem::systemName() const
{
    return _systemName;
}

double AtomicSystem::deltaTime() const
{
    return _deltaTime;
}

double AtomicSystem::volPerAtom() const
{
    return _volPerAtom;
}

double AtomicSystem::kelvin() const
{
    return _kelvin;
}

int AtomicSystem::numOfAtoms() const
{
    return _numOfAtoms;
}

int AtomicSystem::numTimeFrames() const
{
    return _numTimeFrames;
}

QVariant AtomicSystem::latticeDim() const
{
    return QVariant::fromValue(_latticeDims);
}

void AtomicSystem::setSystemName(const QString &value)
{
    if(_systemName.compare(value) != 0)
    {
        _systemName = value;
        emit systemNameChanged();
    }
}

void AtomicSystem::setDeltaTime(const double &value)
{
    if(_deltaTime != value)
    {
        _deltaTime = value;
        emit deltaTimeChanged();
    }
}

void AtomicSystem::setVolPerAtom(const double &value)
{
    if(_volPerAtom != value)
    {
        _volPerAtom = value;
        emit volPerAtomChanged();
    }
}

void AtomicSystem::setKelvin(const double &value)
{
    if(_kelvin  != value)
    {
        _kelvin = value;
        emit kelvinChanged();
    }
}

void AtomicSystem::setNumOfAtoms(const int &value)
{
    if(_numOfAtoms != value)
    {
        _numOfAtoms = value;
        emit numOfAtomsChanged();
    }
}

void AtomicSystem::setNumTimeFrames(const int &value)
{
    if(_numTimeFrames != value)
    {
        _numTimeFrames = value;
        emit numTimeFramesChanged();
    }
}

void AtomicSystem::setLatticeDim(const QVariant &value)
{
    _latticeDims.clear();
    foreach (QVariant item, value.toList()) {
        bool ok = false;
        double key = item.toDouble(&ok);
        if (ok) {
            _latticeDims.append(key);
        }
    }
    //qDebug() << value.toList().size() << "from setLatticeDim()";
    emit latticeDimChanged();
}

QVariant AtomicSystem::getLatticeDim() const
{
    return QVariant::fromValue(_latticeDims);
}

