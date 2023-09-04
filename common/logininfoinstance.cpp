#include "logininfoinstance.h"

// 类外定义静态变量 类型为GarBo ，
logininfoinstance ::Garbo logininfoinstance ::tmp;
logininfoinstance * logininfoinstance::instance = new logininfoinstance;

logininfoinstance *logininfoinstance::getInstance()
{
    return instance;
}

void logininfoinstance::destoy()
{
    if( NULL != logininfoinstance::instance)
    {
        delete logininfoinstance::instance;
        logininfoinstance::instance = NULL;
        cout << "instance is detele";
    }
}
// 设置登录信息
void logininfoinstance::setLoginInfo(QString tmpUser, QString tmpIp, QString tmpPort, QString token)
{
    user = tmpUser;
    ip = tmpIp;
    port = tmpPort;
    this->token = token;
}

QString logininfoinstance::getUser() const
{
    return  user;
}

QString logininfoinstance::getIp() const
{
    return ip;
}

QString logininfoinstance::getPort() const
{
    return port;
}

QString logininfoinstance::getToken() const
{
    return token;
}

logininfoinstance::logininfoinstance()
{

}

logininfoinstance::~logininfoinstance()
{

}

logininfoinstance::logininfoinstance(const logininfoinstance &)
{

}

logininfoinstance &logininfoinstance::operator =(const logininfoinstance &)
{
    return *this;
}
