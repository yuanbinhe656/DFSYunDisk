#include "login.h"
#include "ui_login.h"
#include <QPainter>
#include <QString>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>



Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    ui->myToolBar->setMyParent(this);
this->setMouseTracking(true);
    //三个按键
    connect(ui->myToolBar, &MyTitleBar::showSetWindow, [=]()
            {
                ui->stackedWidget->setCurrentWidget(ui->setPage);
            });
    connect(ui->myToolBar, &MyTitleBar::showMinWindow, [=]()
            {
                // 窗口最小化
                this->showMinimized();
            });
    connect(ui->myToolBar,&MyTitleBar::closeMyWindow,[=](){

        if(ui->stackedWidget->currentWidget() == ui->setPage)
            {
            ui->stackedWidget->setCurrentIndex(0);
        }
        else if(ui->stackedWidget->currentWidget() == ui->regPage)
            {
            ui->stackedWidget->setCurrentWidget(ui->loginPage);
        }
        else
            {
            this->close();
        }
    });
}
void Login:: paintEvent(QPaintEvent * event)
{
    QPainter p(this);
    p.drawPixmap(0,0,this->width(),this->height(),QPixmap(":/login_bk.jpg"));
}
Login::~Login()
{
    delete ui;
}

void Login::on_regAccount_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->regPage);
}
void Login::on_regButton_clicked()
{
    // 处理动作
    // 1.取出用户输入的数据
    QString userName = ui->reg_userName->text();
    QString nickName = ui->reg_NickName->text();
    QString passwd = ui->reg_passwd->text();
    QString confimPwd = ui->reg_confirmPwd->text();
    QString mail = ui->reg_mail->text();
    QString phone = ui->reg_phone->text();
    // 2. 进行数据校验
    QRegularExpression regexp;
    // user 校验
    QString USER_REG = "^[a-zA-Z0-9_@#-\\*]\\{3,16\\}$";
    regexp.setPattern(USER_REG);
    QRegularExpressionMatch match = regexp.match(userName);
    if(!match.hasMatch())
    {
        QMessageBox::warning(this,"ERROR","用户名不正确");
        return;
    }
    //todo others reg
    // paasswd校验
    QString PASSWD_REG = ""

    //3.用户信息发送给服务器
    // -方法 使用http进行发送，使用post方法进行数据传输
    // -数据格式：json  todo 后期改为 Protocol
    QNetworkAccessManager * pManager = new QNetworkAccessManager(this);
    QNetworkRequest request;
    QString url = QString("http://%1:%2/reg").arg(ui->ip->text()).arg(ui->port->text());
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"aoolication/json");
    //todo 进行数据提交
    QJsonObject obj;
    obj.insert("userName",userName);
    obj.insert("nickName", nickName);
    obj.insert("firstPwd", passwd);
    obj.insert("phone", phone);
    obj.insert("email", mail);
    //obj-> doc
    QJsonDocument doc(obj);
    QByteArray json = doc.toJson();
    QNetworkReply *reply = pManager->post(request,json);
    // 4，进行接受数据响应
    connect(reply,&QNetworkReply::readyRead,this,[=](){
       //5.对服务器响应进行分析
       //5.1 接受数据
        QByteArray all = reply->readAll();
       QJsonDocument doc = QJsonDocument::fromJson(all);
        QJsonObject myobj = doc.object();
       QString status= myobj.value("code").toString();

        if("002" == status)
            {
           //成功
       }
        else if("003" == status) {
           //用户已经存在
        }
       else
           {
           // 失败
       }
    });

}

