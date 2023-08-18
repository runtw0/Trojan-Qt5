#include "pacserver.h"
#include "qhttprequest.h"
#include "qhttpresponse.h"
#include "qhttpserver.h"
#include "confighelper.h"
#include "logger.h"
#include "utils.h"
#if defined (Q_OS_WIN)
#include <QApplication>
#endif

#include <QTextCodec>

#include "requesthelper.h"
#include "logger.h"

extern PACHelper *pachelper;

PACServer::PACServer()
{
    connect(&server, &QHttpServer::newRequest, this, &PACServer::onHandleRequest);
}

PACServer::~PACServer()
{
    server.close();
}

void PACServer::listen()
{
    ConfigHelper *conf = Utils::getConfigHelper();

    QString addr = conf->getInboundSettings().enableIpv6Support ? (conf->getInboundSettings().shareOverLan ? "::" : "::1") : (conf->getInboundSettings().shareOverLan ? "0.0.0.0" : "127.0.0.1");
    bool status = server.listen(QHostAddress(addr), conf->getInboundSettings().pacLocalPort);
    if (!status)
        Logger::warning(QString("[PAC Server] failed to listen on %1:%2, PAC will not be functional").arg(addr).arg(conf->getInboundSettings().pacLocalPort));

    delete conf;
    conf = nullptr;
}

QString PACServer::loadFile(QString fileName)
{
#if defined (Q_OS_WIN)
    QFile file(qApp->applicationDirPath() + "/pac/" + fileName);
#else
    QFile file(QDir::homePath() + "/.config/trojan-qt5/pac/" + fileName);
#endif
    file.open(QIODevice::ReadOnly);
    QString data = file.readAll();
    file.close();
    return data;
}

void PACServer::onHandleRequest(QHttpRequest *req, QHttpResponse *rsp)
{
    if (req->path() == "/proxy.pac" && req->method() == QHttpRequest::HTTP_GET) {
        rsp->setHeader("Server", "Trojan-Qt5");
        rsp->setHeader("Content-Type", "application/x-ns-proxy-autoconfig");
        rsp->setHeader("Connection", "close");
        rsp->writeHead(200);
        rsp->end(loadFile("proxy.pac").toUtf8().data());
    } else if (req->path() == "/user-rule.html") {
        if (req->method() == QHttpRequest::HTTP_GET) {
            rsp->setHeader("Server", "Trojan-Qt5");
            rsp->setHeader("Content-Type", "text/html; charset=utf-8");
            rsp->setHeader("Connection", "close");
            rsp->writeHead(200);
            rsp->end(loadFile("user-rule.html").replace(QString("__REPLY_MSG__"), QString("")).toUtf8().data());
        } else if (req->method() == QHttpRequest::HTTP_POST) {
            RequestBodyHelper *reqHelper = new RequestBodyHelper(req, rsp);
            reqHelper->waitBody();
            connect(reqHelper, SIGNAL(finished(QHttpRequest *, QHttpResponse *)), this, SLOT(onHandleRequestBody(QHttpRequest *, QHttpResponse *)));
        }
    } else {
        rsp->setHeader("Server", "Trojan-Qt5");
        rsp->setHeader("Connection", "close");
        rsp->writeHead(404);
        rsp->end();
    }
}

void PACServer::onHandleRequestBody(QHttpRequest *req, QHttpResponse *resp)
{
    QString body(req->body());
    Logger::debug("recv post data:" + body);

    QString replyMsg  = "";
    QStringList list = body.split("=");
    if ((list.length() != 2) || (list[1].trimmed().replace("+", "") == "")) {
        replyMsg = "<p class=\"msg_style\">数据提交格式错误</p>";
    } else {
        QString url = list[1].trimmed();
        QString fileData = loadFile("user-rule.txt");
        if (fileData.indexOf(url) != -1) {
            replyMsg = ""; //重复提交
        } else {

#if defined (Q_OS_WIN)
            QFile file(qApp->applicationDirPath() + "/pac/user-rule.txt");
#else
            QFile file(QDir::homePath() + "/.config/trojan-qt5/pac/user-rule.txt");
#endif

            file.open(QIODevice::Append);
            QTextStream out(&file);
            out.setCodec(QTextCodec::codecForName("utf-8"));
            out << endl;
            out << QString("||" + url)   << endl;
            out << QString("||*." + url) << endl;
            file.close();

            if (pachelper != NULL)
                pachelper->typeModify("GFWLIST"); 

            replyMsg = "<p class=\"msg_style\">数据提交成功</p>";
        }
    }

    resp->setHeader("Server", "Trojan-Qt5");
    resp->setHeader("Content-Type", "text/html; charset=utf-8");
    resp->setHeader("Connection", "close");
    resp->writeHead(200);
    resp->end(loadFile("user-rule.html").replace(QString("__REPLY_MSG__"), replyMsg).toUtf8().data());
}

