#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <mainwindow.h>
#include "common/common.h"

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

    // 设置登录用户信息的json包
    QByteArray setLoginJson(QString user, QString pwd);

    // 设置注册用户信息的json包
    QByteArray setRegisterJson(QString userName, QString nickName, QString firstPwd, QString phone, QString email);

    // 得到服务器回复的登录状态，状态码返回值为 000 或 001 ，登录的section
    QStringList getLoginStatus(QByteArray json);

protected:
    void paintEvent(QPaintEvent * event);

private slots:

    void on_regAccount_clicked();

    void on_regButton_clicked();


    void on_set_clicked();

    void on_login_btn_clicked();

private:
    Ui::Login *ui;

    // 读取配置信息,设置默认登录状态，设置登录信息
    void readCfg();

    // 处理网络请求类对象
    QNetworkAccessManager * m_manager;
    // 主窗口指针
    MainWindow * m_mainWin;
    common m_cm;
};

#endif // LOGIN_H
