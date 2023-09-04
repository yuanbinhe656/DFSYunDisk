#include <QFile>
#include <QMap>
#include <QDir>
#include <QTime>
#include <QFileInfo>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QApplication>
#include <QJsonDocument>
#include <QFileInfoList>
#include <QDesktopWidget>
#include <QCryptographicHash>
#include "des.h"
#include "common.h"

// 初始化变量
QString common::m_typePath = FILETYPEDIR;
QStringList common::m_typeList = QStringList();
QNetworkAccessManager* common::m_netManager = new QNetworkAccessManager;

common::common(QObject *parent)
{
    Q_UNUSED(parent) //宏，用于忽略参数
}

common::~common()
{

}

// 窗口在屏幕中间显示
void common::moveToCenter(QWidget *tmp)
{
    // 显示窗口
    tmp->show();
    QDesktopWidget * desktop = QApplication::desktop();
    // 移动窗口
    tmp->move((desktop->width() - tmp->width())/2,(desktop->height() - tmp->height())/2);

}
/* -------------------------------------------*/
/**
 * @brief           从配置文件中得到相对应的参数
 *
 * @param title     配置文件title名称[title]
 * @param key       key
 * @param path      配置文件路径
 *
 * @returns
 *                  success: 得到的value
 *                  fail:    空串
 */
/* -------------------------------------------*/
QString common :: getCfgValue(QString title, QString key, QString path)
{
    QFile file(path);

    // 只读方式打开
    if( false == file.open(QIODevice::ReadOnly))
    {
        // 打开失败
        cout << "file open err";
        return "";
    }

    // 读取所有内容
    QByteArray json = file.readAll();

    // 关闭文件
    file.close();

    QJsonParseError error;

    // 将源数据json转化为jsondocument对象，用于读写操作
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    //doc 为双重复合的json对象
    // 如果没出现错误
    if (error.error == QJsonParseError::NoError)
    {
        // 如果doc为空
        if (doc.isNull() || doc.isEmpty())
        {
            cout << "doc.isNull || doc.isEmpty";
            return "";
        }
        // doc 不为空
        if (doc.isObject())
        {
            // 首先内部的应该json对象取出
            QJsonObject obj = doc.object();
            QJsonObject tmp = obj.value( title ).toObject();
            // 取出器所有的键，非值，用来进行键的判断
            QStringList list = tmp.keys();
            for(auto itr : list)
            {
                if( itr == key )
                {
                    return tmp.value( itr ).toString();
                }
            }


        }
    }
    else
    {
        cout << "err" <<error.errorString();
    }
    return "";

}
// 通过读取文件，得到文件类型，存放在typelist中 不同类型文件的图标图片存放目录，图标名对应类型名，通过文件后缀与其图标文件名进行匹配
void common::getFileTypeList()
{
    // QDir类使用相对或绝对文件路径来指向一个文件/目录
    QDir dir(m_typePath);
    if( !dir.exists() )
    {
        dir.mkdir(m_typePath);
        cout <<m_typePath << "创建成功！！！";

    }
    // 过滤文件 要正常文件，不是.(当前目录） ..（父目录） 不要符号连接文件
    dir.setFilter(QDir::Files | QDir::NoDot | QDir::NoDotDot | QDir::NoSymLinks);
    // 排序文件，按文件大小
    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList();

    for( auto itr : list)
    {
        m_typeList.append(itr.fileName());
    }


}

QString common::getFileType(QString type)
{
    if( true == m_typeList.contains(type) )
    {
        return m_typePath + "/" + type;
    }
    return m_typePath + "/other.png";
}

void common::writeLoginInfo(QString user, QString pwd, bool isRemeber, QString path)
{
    // webserver 信息
    QString ip = getCfgValue("web_server","ip");
    QString port = getCfgValue("web_server","port");

    // 将webserver打包成一个对象呢
    QMap<QString,QVariant> web_server;
    web_server.insert("ip", ip);
    web_server.insert("port", port);

    //type_path 信息
    QMap<QString,QVariant> type_path;
    type_path.insert("path",m_typePath);

    // login信息
    QMap<QString,QVariant> login;
    // 登录信息加密， 程序字符序列-》机器本地字符序列-》base64加密
    int ret = 0;

    // 登录用户加密
    unsigned char encUser[1024] = {0};
    int encUserLen;
    ret = DesDec((unsigned char *)user.toLocal8Bit().data(), user.toLocal8Bit().size(), encUser, &encUserLen);
    if( ret != 0)
    {
        cout <<"DesEnc err";
        return ;
    }
    // 用户密码加密
    unsigned char encPwd[512] = {0};
    int encPwdLen;
    ret = DesDec((unsigned char *)pwd.toLocal8Bit().data(), pwd.toLocal8Bit().size(), encPwd, &encPwdLen);
    if( ret != 0)
    {
        cout <<"DesEnc err";
        return ;
    }
    // base64加密
    login.insert("user",QByteArray((char *)encUser,encUserLen).toBase64());
    login.insert("pwd",QByteArray((char *)encPwd,encPwdLen).toBase64());

    if(isRemeber == true)
    {
        login.insert("remember", "yes");

    }
    else
    {
        login.insert("remember", "no");
    }

    // 将三个对象整合为一个json对象
    QMap<QString,QVariant> json;
    json.insert("web_server",web_server);
    json.insert("type_path", type_path);
    json.insert("login", login);
    QJsonDocument doc = QJsonDocument::fromVariant(json);
    if(doc.isNull() | doc.isEmpty())
    {
        cout <<  "QJsonDoucument：：fromVariant(json) err";
    }
    // 写入文件
    QFile file(path);
    if(file.open(QIODevice::WriteOnly))
    {
        cout << "file open err";
        return ;
    }

    // json 写入文件
    file.write(doc.toJson());
    file.close();
}

void common::writeWebInfo(QString ip, QString port, QString path)
{
    // web_server信息
    QMap<QString, QVariant> web_server;
    web_server.insert("ip", ip);
    web_server.insert("port", port);

    // type_path信息
    QMap<QString, QVariant> type_path;
    type_path.insert("path", m_typePath);

    // login信息
    QString user = getCfgValue("login", "user");
    QString pwd = getCfgValue("login", "pwd");
    QString remember = getCfgValue("login", "remember");


    QMap<QString, QVariant> login;
    login.insert("user", user);
    login.insert("pwd", pwd);
    login.insert("remember", remember);


    // QVariant类作为一个最为普遍的Qt数据类型的联合
    // QVariant为一个万能的数据类型--可以作为许多类型互相之间进行自动转换。
    QMap<QString, QVariant> json;
    json.insert("web_server", web_server);
    json.insert("type_path", type_path);
    json.insert("login", login);


    QJsonDocument jsonDocument = QJsonDocument::fromVariant(json);
    if ( jsonDocument.isNull() == true)
    {
        cout << " QJsonDocument::fromVariant(json) err";
        return;
    }

    QFile file(path);

    if( false == file.open(QIODevice::WriteOnly) )
    {
        cout << "file open err";
        return;
    }

    file.write(jsonDocument.toJson());
    file.close();
}

QString common::getFileMd5(QString filePath)
{
    QFile localFile(filePath);
    if ( !localFile.open(QIODevice::ReadOnly))
    {
        cout << "file open err";
        return 0;
    }
    // 设置哈希函数为md5
    QCryptographicHash ch(QCryptographicHash::Md5);

    // 相关变量
    quint64 totalBytes = 0;     // 总大小
    quint64 bytesWritten = 0;   // 已经写下的大小
    quint64 bytesToWrite = 0;    // 还剩下要写入的大小
    quint64 loadSize = 1024 * 4; // 每次取出的大小
    QByteArray buf;

    totalBytes = localFile.size();
    bytesToWrite = totalBytes;

    // 进行加密
    while(1)
    {
        if(bytesToWrite > 0)
        {
            buf = localFile.read(qMin( bytesToWrite,loadSize));
            ch.addData(buf);
            bytesWritten += buf.length();
            bytesToWrite -= buf.length();
            buf.resize(0);
        }
        else
        {
            break;
        }
        if(bytesWritten == totalBytes)
        {
            break;
        }
    }
    localFile.close();
    QByteArray md5 = ch.result();
    return md5.toHex();
}
// 字符串md5加密
QString common::getStrMd5(QString str)
{
    QByteArray array;
    array = QCryptographicHash::hash( str.toLocal8Bit(), QCryptographicHash::Md5);
    return array.toHex();
}

QString common::getBoundary()
{
    // 随机种子
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    QString tmp;

    // 48~122 '0'-'A'-'z'
    for(int i = 0; i < 16; i++)
    {
        tmp[i] = qrand() % (122 - 48) +48 ;
    }
    return QString("------WebKitFormBoundary%1").arg(tmp);

}

QString common::getCode(QByteArray json)
{
    QJsonParseError error;

    // 将来源数据json转化为jsondocument

    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    if( error.error ==  QJsonParseError::NoError)
    {
        if(doc.isNull() || doc.isEmpty())
        {
            cout << " doc is null or empty";
            return "";
        }
        if ( doc.isObject() )
        {
            // 取得最外层这个大对象
            QJsonObject obj = doc.object();
            return obj.value("code").toString();
        }
    }
    else
    {
        cout << "err" << error.errorString();
    }
    return "";
}

// 传输数据记录到本地文件，user操作用户 name 操作文件 code：操作码，path：本地文件路径
// 本地文件保存的格式为 首行文件信息 创建时间，操作结果 文件名
// todo filename where
void common::writeRecord(QString user, QString name, QString code, QString path)
{
    // 文件名字，登录用户即为文件名
    QString fileName = path + user;
    // 检查目录是否存在，若不存在，则创建目录
    QDir dir(path);
    cout << "path" << path;
    if(!dir.exists())
    {
        // 目录不存在 创建
        if( dir.mkpath(path))
        {
            cout << path << "目录创建成功";
        }
        else
        {
            cout << path << "目录创建失败";
        }
    }
    cout << "fileName" << fileName.toUtf8().data();
    QByteArray array;
    QFile file(fileName);

    // 如果文件存在，先读取文件记录原来内容
    // 新的文件记录保存在上面，先把之前的文件内容保存，在添加一条新记录，最后将老文件记录添加到新记录的后面
    if( true == file.exists())
    {
        if( false == file.open(QIODevice::ReadOnly))
        {
            cout << "file open err";
        }
        // 读取文件原来内容
        array = file.readAll();
        file.close();
    }
    if (false == file.open(QIODevice::WriteOnly))
    {
        cout << " file open writeonly err";
        return ;
    }
    // 记录包操作
    // xxx.jpg 2023年8月19日12：04：49 秒传成功
    // 获取当前时间
    QDateTime time = QDateTime::currentDateTime();// 获取系统当前时间
    QString timeStr = time.toString("yyyy-MM-dd hh:mm:ss ddd"); // 设置格式
    /*
       秒传文件：
            文件已存在：{"code":"005"}
            秒传成功：  {"code":"006"}
            秒传失败：  {"code":"007"}
        上传文件：
            成功：{"code":"008"}
            失败：{"code":"009"}
        下载文件：
            成功：{"code":"010"}
            失败：{"code":"011"}
            */
    QString actionStr;
    if( code == "005" )
    {
        actionStr = "上传失败，文件已存在";
    }
    else if ( code == "006")
    {
        actionStr = "秒传成功";
    }
    else if ( code == "008")
    {
        actionStr = "上传成功";
    }
    else if ( code == "009")
    {
        actionStr = "上传失败";
    }
    else if ( code == "0010")
    {
        actionStr = "下载成功";
    }
    else if ( code == "0011")
    {
        actionStr = "下载失败";
    }
    QString str = QString("[%1]\t%2\t[%3]\r\n").arg(name).arg(timeStr).arg(actionStr);
    cout <<str.toUtf8().data();

    //  先转化为本地字符集，tolocal
    file.write( str.toLocal8Bit() );
    if( array.isEmpty() == false )
    {
        // 将原来内容添加到新记录后面
        file.write(array);
    }
    file.close();

}

QNetworkAccessManager *common::getNetManager()
{
    // 应用程序一般只有一个
    return m_netManager;
}
