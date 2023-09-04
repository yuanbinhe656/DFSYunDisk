#ifndef LOGININFOINSTANCE_H
#define LOGININFOINSTANCE_H

#include <QString>
#include "common.h"

// 单例模式，主要保存当前登录用户，服务器信息

class logininfoinstance
{
public:
    // 静态函数保证唯一实例
    static logininfoinstance * getInstance();
    // 释放堆空间
    static void destoy();
    //设置登录信息
    void setLoginInfo(QString tmpUser, QString tmpIp, QString tmpPort, QString token = "");
    // 获取登录用户
    QString getUser() const;
    // 获取服务器ip
    QString getIp() const;
    // 获取服务器端口
    QString getPort() const;
    // 获取登录token
    QString getToken() const;

// 单例模式，将构造函数放到私有成员中
private:
    logininfoinstance();
    ~logininfoinstance();
    // 把复制构造函数和 = 操作符也设置为私有，防止其被复制
    logininfoinstance( const logininfoinstance&);
    logininfoinstance& operator = (const logininfoinstance &);

    // Garbo 唯一工作在析构中删除Singleton的实例
    class Garbo
    {
    public:
        ~Garbo()
        {
            // 释放堆空间
            logininfoinstance::destoy();
        }
    };
    // 在类外定义一个静态成员变量，程序结束时，系统自动调用其析构函数
    // static 类的构造函数在main（）退出后调用
    static Garbo tmp; // 静态数据成员，类中声明，类外定义

    // 静态数据成员，，类中声明，类外定义
    static logininfoinstance *instance;

    QString user;
    QString token;
    QString ip;
    QString port;
};


#endif // LOGININFOINSTANCE_H
