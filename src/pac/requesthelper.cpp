#include "requesthelper.h"

RequestBodyHelper::RequestBodyHelper(QHttpRequest* req, QHttpResponse* resp)
    : req(req), resp(resp)
{
}

void RequestBodyHelper::waitBody()
{
    req->storeBody();
    connect(req, SIGNAL(end()), this, SLOT(end()));
}
