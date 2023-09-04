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
#include <QDebug>
#include <QUrl>
#include "common/logininfoinstance.h"
#include "common/des.h"
#include <QNetworkAccessManager>


Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    // 初始化 网络请求http类
    m_manager = common::getNetManager();
    // 无需父窗口
    m_mainWin = new MainWindow;
    // 窗口图标
    this->setWindowIcon(QIcon(":/logo.ico"));
    m_mainWin->setWindowIcon(QIcon(":/logo.ico"));
    // 去掉上边框
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    // 设置当前窗口的字体信息
    this->setFont(QFont("新宋体",12,QFont::Bold,false));

    // 设置三个密码输入框 密码显示模式
    ui->log_pwd->setEchoMode(QLineEdit::Password);
    ui->reg_passwd->setEchoMode(QLineEdit::Password);
    ui->reg_confirmPwd->setEchoMode(QLineEdit::Password);

    // 设置初始显示窗口
    ui->stackedWidget->setCurrentIndex(0);
    ui->log_usr->setFocus();
    // 数据格式提示
    ui->log_usr->setToolTip("合法字符:[a-z|A-Z|#|@|0-9|-|_|*],字符个数: 3~16");
    ui->reg_userName->setToolTip("合法字符:[a-z|A-Z|#|@|0-9|-|_|*],字符个数: 3~16");
    ui->reg_NickName->setToolTip("合法字符:[a-z|A-Z|#|@|0-9|-|_|*],字符个数: 3~16");;
    ui->log_pwd->setToolTip("合法字符:[a-z|A-Z|#|@|0-9|-|_|*],字符个数: 6~18");;
    ui->reg_passwd->setToolTip("合法字符:[a-z|A-Z|#|@|0-9|-|_|*],字符个数: 6~18");
    ui->reg_confirmPwd->setToolTip("合法字符:[a-z|A-Z|#|@|0-9|-|_|*],字符个数: 6~18");
    ui->myToolBar->setMyParent(this);
    this->setMouseTracking(true);
    // 读取配置文件信息，并进行初始化操作
    readCfg();
    m_cm.getFileTypeList();

#if 1
    // 测试数据

    ui->reg_userName->setText("Yevin_666");
    ui->reg_NickName->setText("Yevin@666");
    ui->reg_passwd->setText("123456");
    ui->reg_confirmPwd->setText("123456");
    ui->reg_phone->setText("11311111111");
    ui->reg_mail->setText("abc1@qq.com");
    ui->port->setText("80");
    ui->ip->setText("192.168.231.128");

#endif

    //三个按键 mytitlebar中的
    // 设置界面
    connect(ui->myToolBar, &MyTitleBar::showSetWindow, [=]()
            {
                ui->stackedWidget->setCurrentWidget(ui->setPage);
            });
    // 窗口最小化
    connect(ui->myToolBar, &MyTitleBar::showMinWindow, [=]()
            {
                // 窗口最小化
                this->showMinimized();
            });
    // 关闭按钮
    connect(ui->myToolBar,&MyTitleBar::closeMyWindow,[=](){
        // 如果是设置界面，切换到登录界面
        if(ui->stackedWidget->currentWidget() == ui->setPage)
            {
            // 清除数据
            ui->ip->clear();
            ui->port->clear();
            // 切换界面
            ui->stackedWidget->setCurrentIndex(0);
            ui->log_usr->setFocus();
        }
        // 如果是注册界面，切换到登录界面
        else if(ui->stackedWidget->currentWidget() == ui->regPage)
            {
            // 清除数据
            ui->reg_userName->clear();
            ui->reg_NickName->clear();
            ui->reg_passwd->clear();
            ui->reg_confirmPwd->clear();
            ui->reg_phone->clear();
            ui->reg_mail->clear();
            // 切换界面
            ui->stackedWidget->setCurrentWidget(ui->loginPage);
            ui->log_usr->setFocus();
        }
        else // 如果是登录窗口关闭
            {
            this->close();
        }
    });

    // 切换用户 changeUser
    connect(m_mainWin,&MainWindow::changeUser,[=](){
        m_mainWin->hide();
        this->show();
    });

}
void Login:: paintEvent(QPaintEvent * event)
{
    QPainter p(this);
    p.drawPixmap(0,0,this->width(),this->height(),QPixmap(":/login_bk.jpg"));
}
// 读取配置信息，设置默认登录状态 默认设置信息
void Login:: readCfg()
{
    // 取出配置文件中的用户信息
    QString user = m_cm.getCfgValue("login","user");
    QString pwd = m_cm.getCfgValue("login","pwd");
    QString remeber = m_cm.getCfgValue("login","remember");
    int ret = 0;
    // 是否记住密码
    if( remeber == "yes")
    {
        // 密码解密
        unsigned char encPWD[512] = {0};
        int encPwdLen = 0;
        // toLocal18Bit(),转化为本地字符集，默认windows则为gbk，linux为utf-8
        QByteArray tmp = QByteArray::fromBase64(pwd.toLocal8Bit());
        ret = DesDec( (unsigned char *)tmp.data(), tmp.size(), encPWD, &encPwdLen);
        if(ret != 0)
        {
            cout <<"DesDec";
            return ;
        }
        // 如果是windows平台，本地字符集转化为utf-8
#ifdef _WIN32
       ui->log_pwd->setText( QString:: fromLocal8Bit( (const char *)encPWD,encPwdLen));
#else
        ui->log_pwd->setText( (const char *)encPwd);
#endif
    ui->rember_pwd->setChecked(true);
    }
    else  // 没有记住密码
    {
        ui->log_pwd->setText("");
        ui->rember_pwd->setChecked(true);
    }

    //用户解密
    unsigned char encUsr[512] = {0};
    int encUsrLen = 0;
    QByteArray tmp = QByteArray::fromBase64( user.toLocal8Bit());
    ret = DesDec( (unsigned char *)tmp.data(), tmp.size(), encUsr, &encUsrLen);
    if(ret != 0)
    {
        cout << "DesDec";
        return;
    }

    #ifdef _WIN32 //如果是windows平台
        //fromLocal8Bit(), 本地字符集转换为utf-8
        ui->log_usr->setText( QString::fromLocal8Bit( (const char *)encUsr, encUsrLen ) );
    #else //其它平台
        ui->log_usr->setText( (const char *)encUsr );
    #endif

        QString ip = m_cm.getCfgValue("web_server", "ip");
        QString port = m_cm.getCfgValue("web_server", "port");
        ui->ip->setText(ip);
        ui->port->setText(port);
}
Login::~Login()
{
    delete ui;
}

// 登录界面没有账号跳到注册界面
void Login::on_regAccount_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->regPage);

}

// 注册账号
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

    regexp.setPattern(USER_REG);


    QRegularExpressionMatch match = regexp.match(userName);
    if(!match.hasMatch())
    {
        QMessageBox::warning(this,"ERROR","用户名格式不正确");

        ui->reg_userName->clear();
        ui->reg_userName->setFocus();
        return;
    }

    match = regexp.match(nickName);
    if(!match.hasMatch())
    {
        QMessageBox::warning(this,"ERROR","昵称格式不正确");
        ui->reg_NickName->clear();
        ui->reg_NickName->setFocus();
        return;
    }
    regexp.setPattern(PASSWD_REG);
    match = regexp.match(passwd);
    if(!match.hasMatch())
    {
        QMessageBox::warning(this,"ERROR","密码格式不正确");
        ui->reg_passwd->clear();
        ui->reg_passwd->setFocus();
        return;
    }

    if(confimPwd != passwd)
    {
        QMessageBox::warning(this,"ERROR","两次输入密码不匹配");
        ui->reg_confirmPwd->clear();
        ui->reg_confirmPwd->setFocus();
        return;
    }
    regexp.setPattern(PHONE_REG);
    match = regexp.match(phone);
    if(!match.hasMatch())
    {
        QMessageBox::warning(this,"ERROR","手机号格式不正确");
        ui->reg_phone->clear();
        ui->reg_phone->setFocus();
        return;
    }
    regexp.setPattern(EMAIL_REG);
    match = regexp.match(mail);
    if(!match.hasMatch())
    {
        QMessageBox::warning(this,"ERROR","邮箱格式不正确");
        ui->reg_mail->clear();
        ui->reg_mail->setFocus();
        return;
    }
    //3.用户信息发送给服务器
    // -方法 使用http进行发送，使用post方法进行数据传输
    // -数据格式：json
    // todo 后期改为 Protocol
    // 注册信息打包成json格式
    QByteArray array = setRegisterJson(userName, nickName, m_cm.getStrMd5(passwd), phone, mail);
    qDebug()<< "register json data" << array;
    cout << "register";
    // 设置连接服务器要发送的url
    QNetworkRequest request;
    QString url = QString("http://%1:%2/reg").arg(ui->ip->text()).arg(ui->port->text());
    request.setUrl(QUrl(url));
    qDebug()<< "register json data" << QString("http://%1:%2/reg").arg(ui->ip->text()).arg(ui->port->text());
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("aoolication/json"));
    request.setHeader(QNetworkRequest::ContentLengthHeader,QVariant(array.size()));
    // 发送数据
    QNetworkReply *reply = m_manager->post(request,array);
    connect(reply, &QNetworkReply::readyRead,[=]{

        // 读server返回的数据
        QByteArray jsonData = reply->readAll();
        cout <<jsonData;
        /*
         * 注册 -- server 返回的json格式数据：
         * 成功         {“code”：“002”}
         * 该用户已存在  {“code”：“003”}
         * 失败         {“code”：“004”}
         */
        // 判断状态码
        // 注册成功
        if("002" == m_cm.getCode(jsonData))
        {
            // 注册成功提示窗口
            QMessageBox::information(this, "注册成功","注册成功，请登录");

            // 清空则测界面编辑的内容
            ui->reg_mail->clear();
            ui->reg_phone->clear();
            ui->reg_passwd->clear();
            ui->reg_NickName->clear();
            ui->reg_userName->clear();
            ui->reg_confirmPwd->clear();
            // 将登录界面信息自动填充
            ui->log_usr->setText(userName);
            ui->log_pwd->setText(passwd);
            // 切换到登录界面
            ui->stackedWidget->setCurrentWidget(ui->loginPage);
        }
        // 用户已经存在
        else if("003" == m_cm.getCode(jsonData))
        {
            // 该用户已注册，注册失败
            QMessageBox::warning(this, "注册失败", QString("[%1]该用户已存在！！！").arg(userName));

        }
        // 注册失败
        else if("004" == m_cm.getCode(jsonData))
        {
             QMessageBox::warning(this, "注册失败", "注册失败");
        }
        delete  reply;
    });
}

// 用户设置操作
void Login::on_set_clicked()
{
    QString ip = ui->ip->text();
    QString port = ui->port->text();

    // 数据判断
    // 判断ip
    QRegExp regexp(IP_REG);
    if(!regexp.exactMatch(ip))
    {
        QMessageBox::warning(this, "警告", "您输入的ip格式不正确");
        ui->ip->clear();
        return ;
    }
    regexp.setPattern(PORT_REG);
    if(!regexp.exactMatch(port))
    {
        QMessageBox::warning(this, "警告", "您输入的端口格式不正确");
        ui->port->clear();
        return ;
    }
    //  跳到登录界面
    ui->stackedWidget->setCurrentWidget(ui->loginPage);

    // 将配置信息写入配置文件中
    m_cm.writeWebInfo(ip,port);

}

// 用户登录操作
void Login::on_login_btn_clicked()
{
    QString user = ui->log_usr->text();
    QString pwd = ui->log_pwd->text();
    QString ip = ui->ip->text();
    QString port = ui->port->text();
    // 数据校验
    QRegExp regexp(USER_REG);
    if( !regexp.exactMatch(user) )
    {
        QMessageBox::warning(this, "警告", "用户名格式不正确");
        ui->log_usr->clear();
        ui->log_usr->setFocus();
        return;
    }
    regexp.setPattern(PASSWD_REG);
    if( !regexp.exactMatch(pwd) )
    {
        QMessageBox::warning(this, "警告", "密码格式不正确");
        ui->log_pwd->clear();
        ui->log_pwd->setFocus();
        return;
    }
    // 登录信息写入cfg.json
    // 登录信息加密
    m_cm.writeLoginInfo(user, pwd, ui->rember_pwd->isChecked());
    // 设置登录信息json包， 密码经过md5加密，getstrMd5（）
    QByteArray array = setLoginJson(user, m_cm.getStrMd5(pwd));
    // 设置登录的url
    QNetworkRequest request;
    QString url = QString("http://%1:%2/login").arg(ip).arg(port);
    request.setUrl(QUrl(url));
    // 请求头设置
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(array.size()));
    // 向浏览器发送post请求
    QNetworkReply *reply = m_manager->post(request,array);
    connect(reply, &QNetworkReply::readyRead, [=]{
        // 出错了
        if( reply->error() != QNetworkReply::NoError)
        {
            cout <<reply->errorString();
            reply->deleteLater();
            return ;
        }
        // 将server返回的信息读出
        QByteArray json = reply->readAll();
        /*
         * 登录-- 服务器返回的json格式
         *      成功：{"code":"000"}
         *      失败：{“code”：“001”}
         */
         cout << "server return value:"<< json;
         // 取出json中的数据
         QStringList  tmpList = getLoginStatus(json);

         // 登录成功
         if( tmpList.at(0) == "000")
         {
             cout << "登录成功";

             //设置登录信息，显示文件列表界面需要使用这些信息
             logininfoinstance *p = logininfoinstance::getInstance();
             // 获取单例
             p->setLoginInfo(user, ip,port,tmpList.at(1));
             cout << p->getUser().toUtf8().data() << "," << p->getIp() << "," << p->getPort() << tmpList.at(1);

             // 当前窗口隐藏
             this->hide();
             // 主窗口显示
             m_mainWin->showMainWindow();

         }
         else
         {
             QMessageBox::warning(this, "登录失败", "用户名或密码不正确！！！");
         }
         reply->deleteLater();
    });

}

// 设置登录用户信息的json包
QByteArray Login::setLoginJson(QString user, QString pwd)
{
    QMap<QString,QVariant> login;
    login.insert("user", user);
    login.insert("pwd", pwd);

    /*
     * json 格式如下
     * {
     *  user：xxxx，
     *  pwd： xxxx
     * }
     * */
    QJsonDocument jsonDocument = QJsonDocument::fromVariant(login);
    if( jsonDocument.isNull() )
    {
        cout << " jsonDocument.isNull()";
        return "";
    }
    return jsonDocument.toJson();

}

// 设置注册用户信息的json包
QByteArray Login::setRegisterJson(QString userName, QString nickName, QString firstPwd, QString phone, QString email)
{

    QMap<QString,QVariant> reg;
    reg.insert("userName", userName);
    reg.insert("nickName", nickName);
    reg.insert("firstPwd", firstPwd);
    reg.insert("phone", phone);
    reg.insert("email", email);


    /*
     * json 格式如下
     * {
     *  userName：xxxx，
     *  nickName：xxxx，
        firstPwd：xxxx，
        phone：xxxx，
        email：xxxx，

     * }
     * */
    QJsonDocument jsonDocument = QJsonDocument::fromVariant(reg);
    if( jsonDocument.isNull() )
    {
        cout << " jsonDocument.isNull()";
        return "";
    }
    return jsonDocument.toJson();
}

// 得到服务器回复的登录状态，状态码返回值为 000 或 001 ，登录的section
QStringList Login::getLoginStatus(QByteArray json)
{
    QJsonParseError error;
    QStringList list;

    // 将json转化为jsondocument
    // 由QBytearrye对象构造一个Qjsondocument对象，用于读写
    QJsonDocument doc = QJsonDocument::fromJson(json,&error);
    if( error.error == QJsonParseError::NoError )
    {
        if( doc.isNull() || doc.isEmpty())
        {
            cout << "doc.isNull() || doc.isEmpty()";
            return list;
        }
        if ( doc.isObject())
        {
            // 取得最外层这个大对象
            QJsonObject obj = doc.object();
            cout << "服务器返回的数据" << obj;
            // 状态码
            list.append( obj.value( "code" ).toString() );
            // 登录token
            list.append( obj.value( "token" ).toString() );
        }
    }
    // 出现错误
    else
    {
        cout << "err = " << error.errorString();
    }
    return list;
}
