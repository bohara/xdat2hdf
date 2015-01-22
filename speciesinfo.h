#ifndef SPECIESINFO_H
#define SPECIESINFO_H

#include <QObject>
#include <QFile>
#include <QList>
#include <QTextStream>
#include <QQmlListProperty>

/***/
class SpeciesInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString symbol READ symbol NOTIFY symbolChanged)
    Q_PROPERTY(unsigned int atomId READ atomId NOTIFY atomIdChanged)
    Q_PROPERTY(unsigned int beginIndex READ beginIndex WRITE setBeginIndex NOTIFY beginIndexChanged)
    Q_PROPERTY(unsigned int endIndex READ endIndex WRITE setEndIndex NOTIFY endIndexChanged)

public:
    explicit SpeciesInfo(QObject *parent = 0);
    SpeciesInfo(const SpeciesInfo &_sInfo);
    SpeciesInfo &operator =(const SpeciesInfo &_sInfo);

    QString symbol() const;
    unsigned int atomId() const;
    unsigned int beginIndex() const;
    unsigned int endIndex() const;
    unsigned int atomsCount() const;

    void setSymbol(const QString &value);
    void setAtomId(const unsigned int &value);
    void setBeginIndex(const unsigned int &value);
    void setEndIndex(const unsigned int &value);

signals:
    void symbolChanged();
    void atomIdChanged();
    void beginIndexChanged();
    void endIndexChanged();

private:
    unsigned int _elemId;
    unsigned int _elemBegin;
    unsigned int _elemEnd;
    QString _elemSymbol;

};

#endif // SPECIESINFO_H
