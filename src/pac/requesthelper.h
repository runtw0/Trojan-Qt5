#ifndef REQUESTHELPER_H
#define REQUESTHELPER_H

#include <QObject>
#include "qhttprequest.h"
#include "qhttpresponse.h"
#include "qhttpserver.h"

class RequestBodyHelper : public QObject
{
    Q_OBJECT
public:
    RequestBodyHelper(QHttpRequest* req, QHttpResponse* resp);

    void waitBody();

public slots:
    void end()
    {
        emit finished(req, resp);
        this->deleteLater();
    }

signals:
    void finished(QHttpRequest* req, QHttpResponse* resp);

private:
    QHttpRequest* req;
    QHttpResponse* resp;

};

#endif //REQUESTHELPER_H