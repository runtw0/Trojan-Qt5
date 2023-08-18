#ifndef PACSERVER_H
#define PACSERVER_H

#include <QObject>
#include "qhttprequest.h"
#include "qhttpresponse.h"
#include "qhttpserver.h"

class PACServer: public QObject
{
    Q_OBJECT
public:
    PACServer();
    ~PACServer();

    void listen();
    QString loadFile(QString fileName);

public slots:
    void onHandleRequest(QHttpRequest *req, QHttpResponse *rsp);
    void onHandleRequestBody(QHttpRequest *req, QHttpResponse *resp);

private:
    QHttpServer server;
};

#endif // PACSERVER_H
