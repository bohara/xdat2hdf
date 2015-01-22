#include "speciesinfo.h"


SpeciesInfo::SpeciesInfo(QObject *parent) :
    QObject(parent)
{
    _elemId = 0;
    _elemBegin = 0;
    _elemEnd = 0;
    _elemSymbol.clear();
}

SpeciesInfo::SpeciesInfo(const SpeciesInfo &_sInfo)
{
    _elemId     = _sInfo.atomId();
    _elemBegin  = _sInfo.beginIndex();
    _elemEnd    = _sInfo.endIndex();
    _elemSymbol = _sInfo.symbol();
}

SpeciesInfo & SpeciesInfo::operator=(const SpeciesInfo &_sInfo)
{
    if(this == &_sInfo)
        return *this;

    _elemId     = _sInfo.atomId();
    _elemBegin  = _sInfo.beginIndex();
    _elemEnd    = _sInfo.endIndex();
    _elemSymbol = _sInfo.symbol();
    return *this;
}

QString SpeciesInfo::symbol() const
{
    return _elemSymbol;
}

unsigned int SpeciesInfo::atomId() const
{
    return _elemId;
}

unsigned int SpeciesInfo::atomsCount() const
{
    return (endIndex() - beginIndex() + 1);
}

unsigned int SpeciesInfo::beginIndex() const
{
    return _elemBegin;
}

unsigned int SpeciesInfo::endIndex() const
{
    return _elemEnd;
}

void SpeciesInfo::setSymbol(const QString &value)
{
    _elemSymbol.clear();
    _elemSymbol = value;
}

void SpeciesInfo::setAtomId(const unsigned int &value)
{
    if(value > 0)
        _elemId = value;
}

void SpeciesInfo::setBeginIndex(const unsigned int &value)
{
    if(_elemBegin != value)
    {
        _elemBegin = value;
        emit beginIndexChanged();
    }
}

void SpeciesInfo::setEndIndex(const unsigned int &value)
{
    if(_elemEnd != value)
    {
        _elemEnd = value;
        emit endIndexChanged();
    }
}

