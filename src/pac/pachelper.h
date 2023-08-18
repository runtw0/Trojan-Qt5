#ifndef PACHELPER_H
#define PACHELPER_H

#include <QObject>
#include <QDir>
#include <QTimer>

class PACHelper: public QObject
{
    Q_OBJECT
public:
    PACHelper();
    ~PACHelper();

    QJsonDocument loadRules();

    void modify(QString filename);

    QByteArray request(QString url, bool useProxy = true);

    QString LocalIpAddress();

    void PACHelper::onHandleRemotePac();

public slots:
    void typeModify(QString type);
    void copyPACUrl();
    void editLocalPACFile();
    void editUserRule();

private:
    QDir configDir;
    QString configFile;
    QString gfwList;
    QString userRule;
    QString pac;
    QTimer* timer;
};

#endif // PACHELPER_H
