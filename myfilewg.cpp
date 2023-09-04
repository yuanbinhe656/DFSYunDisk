#include "myfilewg.h"
#include "ui_myfilewg.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QListWidgetItem>
#include "common/logininfoinstance.h"
#include "selfwidget/filepropertyinfo.h"
#include "common/downloadtask.h"
#include <QHttpMultiPart>
#include <QHttpPart>

MyFileWg::MyFileWg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyFileWg)
{
    ui->setupUi(this);

    // 初始化listwidget文件列表
    initListWidget();
    // 添加右键菜单
    addActionMenu();

    // http管理类对象
    m_manager = common::getNetManager();

    // 定时检查任务队列
    checkTaskList();
}

MyFileWg::~MyFileWg()
{
    delete ui;
}
// 初始化listwidget文件列表
void MyFileWg::initListWidget()
{
    ui->listWidget->setViewMode(QListView::IconMode);
    // 设置显示图标模式
    // 设置图标大小
    ui->listWidget->setIconSize(QSize(80,80));

    // 设置item大小
    ui->listWidget->setGridSize(QSize(100,100));

    // 设置Qlistview大小改变时，图标的调整模式，默认是固定，改为自动调整
    // 自动适应布局
    ui->listWidget->setResizeMode(QListView::Adjust);

    // 设置列表可拖动，如果想固定不能拖动，使用Qlistview：：static
    ui->listWidget->setMovement(QListView::Static);
    // 设置图标之间的间距，当setGRidsize（）时，此选项无效
    ui->listWidget->setSpacing(30);

    // listwidget右键菜单
    // 发出custoncontextmenurequeseted信号
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget,&QListWidget::itemPressed,this,[=](QListWidgetItem *item){
        QString str = item->text();
        if(str == "上传文件")
        {
            // 添加需要上传文件到上传任务列表
            addUploadFiles();
        }
    });
}
// 添加右键菜单
void MyFileWg::addActionMenu()
{
    // ======  菜单1==========
    m_menu = new MyMenu(this);

    // 初始化菜单项
    m_downloadAction = new QAction( "下载", this);
    m_shareAction = new QAction("分享", this);
    m_delAction = new QAction("删除", this);
    m_propertyAction = new QAction("属性", this);

    // 动作1添加到菜单1
    m_menu->addAction(m_downloadAction);
    m_menu->addAction(m_shareAction);
    m_menu->addAction(m_delAction);
    m_menu->addAction(m_propertyAction);

    //===================菜单2===================
    m_menuEmpty = new MyMenu( this );
    // 动作2
    m_pvAscendingAction = new QAction("按下载量升序", this);
    m_pvDescendingAction = new QAction("按下载量降序", this);
    m_refreshAction = new QAction("刷新", this);
    m_uploadAction = new QAction("上传", this);

    // 动作2添加到菜单2
    m_menuEmpty->addAction(m_pvAscendingAction);
    m_menuEmpty->addAction(m_pvDescendingAction);
    m_menuEmpty->addAction(m_refreshAction);
    m_menuEmpty->addAction(m_uploadAction);

    // ==========信号与槽==========
    // 下载
    connect(m_downloadAction, &QAction::triggered, [=](){
        cout << "下载动作";
        // 添加需要下载的文件到下载列表
        addDownloadFiles();
    });

    // 分享
    connect(m_shareAction, &QAction::triggered, [=]()
    {
        cout << "分享动作";
        dealSelectdFile("分享"); //处理选中的文件
    });

    // 删除
    connect(m_delAction, &QAction::triggered, [=]()
    {
        cout << "删除动作";
        dealSelectdFile("删除"); //处理选中的文件
    });

    // 属性
    connect(m_propertyAction, &QAction::triggered, [=]()
    {
        cout << "属性动作";
        dealSelectdFile("属性"); //处理选中的文件
    });

    // 按下载量升序
    connect(m_pvAscendingAction, &QAction::triggered, [=]()
    {
        cout << "按下载量升序";
        refreshFiles(PvAsc);
    });

    // 按下载量降序
    connect(m_pvDescendingAction, &QAction::triggered, [=]()
    {
        cout << "按下载量降序";
        refreshFiles(PvDesc);
    });

    //刷新
    connect(m_refreshAction, &QAction::triggered, [=]()
    {
        cout << "刷新动作";
        //显示用户的文件列表
        refreshFiles();
    });

    //上传
    connect(m_uploadAction, &QAction::triggered, [=]()
    {
        cout << "上传动作";
        //添加需要上传的文件到上传任务列表
        addUploadFiles();
    });
}

// ====== 上传文件处理=========
// 添加需要上传的文件
void MyFileWg::addUploadFiles()
{
    // 切换到传输列表的上传界面
    emit gotoTransfer(TransferStatus::Upload);
    // 获取上传列表实例
    uploadtask * uploadList = uploadtask::getInstance();
    if(uploadList == NULL)
    {
        cout << " uploadtask::getInstance()== NULL";
        return ;
    }
    QStringList list = QFileDialog::getOpenFileNames();

    for(int i = 0; i < list.size(); i++)
    {
        //cout << "所选文件为："<<list.at(i);
        //  -1: 文件大于30m
        //  -2：上传的文件是否已经在上传队列中
        //  -3: 打开文件失败
        //  -4: 获取布局失败
        int res = uploadList->appendUploadList(list.at(i));
        if(res == -1)
        {
            QMessageBox::warning(this, "文件太大", "文件大小不能超过30M！！！");
        }
        else if(res == -2)
        {
            QMessageBox::warning(this, "添加失败", "上传的文件是否已经在上传队列中！！！");
        }
        else if(res == -3)
        {
            cout << "打开文件失败";
        }
        else if(res == -4)
        {
            cout << "获取布局失败";
        }
    }
}
// 设置md5信息的json包
QByteArray MyFileWg::setMd5json(QString user, QString token, QString md5, QString fileName)
{
    QMap<QString, QVariant> tmp;
    tmp.insert("user", user);
    tmp.insert("token", token);
    tmp.insert("md5", md5);
    tmp.insert("fileName", fileName);

    /*json数据如下
    {
        user:xxxx,
        token:xxxx,
        md5:xxx,
        fileName: xxx
    }
    */
    QJsonDocument jsonDocument = QJsonDocument::fromVariant(tmp);
    if(jsonDocument.isNull())
    {
        cout << "jsonDocument.isNull";
        return "" ;

    }
    return jsonDocument.toJson();

}
//上传文件处理，取出上传任务队列的队首任务，上传完成后，再取下一个任务  真正执行上传操作的函数
void MyFileWg::uploadFileAction()
{
    uploadtask * uploadList = uploadtask::getInstance();
    if(uploadList == NULL)
    {
        cout << "uploadtask::getInstance() = null";
        return ;
    }
    // 队列为空，无上传任务，终止
    if ( uploadList->isEmpty() )
    {
        return ;
    }

    // 查看是否有上传任务，单任务上传，当前有任务，不能上传
    if( uploadList->isUpload())
    {
        return ;
    }
    // 获取登录信息实例
    logininfoinstance *login = logininfoinstance::getInstance();


    // url
    QNetworkRequest request;
    QString url = QString("http://%1:%2/md5").arg( login->getIp()).arg(login->getPort());
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 取出第0个任务，如果任务队列没有任务上传，设置第0个任务上传
    UploadFileInfo *info = uploadList->takeTask();

    // Post数据包
    QByteArray array = setMd5json(login->getUser(),login->getToken(),info->md5,info->fileName);

    // 发送post请求
    QNetworkReply *reply = m_manager->post(request,array);
    if(reply == NULL)
    {
        cout << "reply is NULL";
        return ;
    }
    // 信号和槽连接
    connect(reply,&QNetworkReply::finished,[=]{
        if ( reply->error() != QNetworkReply::NoError)
        {
            cout << reply->errorString();
            // 释放资源
            reply->deleteLater();
            return ;
        }

        // 读取收到的回复，根据回复代码来执行下一步操作
        QByteArray array = reply->readAll();
        reply->deleteLater();

        /*
        秒传文件：
            文件已存在：{"code":"005"}
            秒传成功：  {"code":"006"}
            秒传失败：  {"code":"007"}
        token验证失败：{"code":"111"}

        */
        if( "005" == m_cm.getCode(array))
        {
            m_cm.writeRecord(login->getUser(),info->fileName,"005");
            // 删除已完成上传的任务
            uploadList->dealUploadTask();
        }
        else if ("006" == m_cm.getCode(array))
        {
            // 秒传成功，
            m_cm.writeRecord(login->getUser(),info->fileName,"006");
            // 删除已完成上传的任务
            uploadList->dealUploadTask();
        }
        else if ("007" == m_cm.getCode(array))
        {
            // 上传任务
            uploadFile(info);
        }
        else if ("111" == m_cm.getCode(array))
        {
            QMessageBox::warning(this,"账户异常", "青重新登录 ！！！");
            emit loginAgainSignal();
            return ;
        }
    });

}
// 上传真正的文件内容，不能秒传的前提下
void MyFileWg::uploadFile(UploadFileInfo *info)
{
    // 取出任务
    //取出上传任务
    QFile *file = info->file;           //文件指针
    QString fileName = info->fileName;  //文件名字
    QString md5 = info->md5;            //文件md5码
    qint64 size = info->size;           //文件大小
    DataProgress *dp = info->dp;        //进度条控件
    QString boundary = m_cm.getBoundary();   //产生分隔线

    // 获取登录信息实例
    logininfoinstance *login = logininfoinstance::getInstance();

    // 数据传输
    QHttpPart part;
    QString disp = QString("form-data; user=\"%1\"; filename=\"%2\"; md5=\"%3\"; size=%4")
            .arg(login->getUser()).arg(info->fileName).arg(info->md5).arg(info->size);
    part.setHeader(QNetworkRequest::ContentDispositionHeader, disp);
    part.setHeader(QNetworkRequest::ContentTypeHeader, "image/png"); // 传输的文件对应的content-type
    // 直接读取文件内容到post body中
    part.setBodyDevice(info->file);
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);
    multiPart->append(part);

    // 请求对象
    QNetworkRequest request;
    QString url = QString("http://%1:%2/upload").arg(login->getIp()).arg(login->getPort());
    request.setUrl(QUrl(url));

    // qt默认请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data");

    // 发送post请求
    QNetworkReply * reply = m_manager->post(request,multiPart);
    if(reply == NULL)
    {
        cout << "reply == NULL";
        return ;
    }

    // 有可用数据是更新
    connect( reply, &QNetworkReply::uploadProgress,[=](qint64 bytesRead, qint64 totalBytes){
        // 总大小不为0时才能进行
        if(totalBytes != 0)
        {
            dp->setProgress(bytesRead/1024,totalBytes/1024);
        }
    });

    // 获取请求的数据完成时，就会发送信号
    connect(reply, &QNetworkReply::finished,[=]{
        if( reply->error() != QNetworkReply::NoError)
        {
            cout << reply->errorString();
            reply->deleteLater();
            m_cm.writeRecord(login->getUser(), info->fileName, "009");
            uploadtask *uploadList = uploadtask::getInstance();
            uploadList->dealUploadTask(); //删除已经完成的上传任务
            return;
        }
        QByteArray array = reply->readAll();

        reply->deleteLater();
        // 析构对象
        multiPart->deleteLater();

        /*
            上传文件：
                成功：{"code":"008"}
                失败：{"code":"009"}
            */
        if( "008" == m_cm.getCode(array))
        {
             m_cm.writeRecord(login->getUser(), info->fileName, "008");
        }
        else if( "009" == m_cm.getCode(array) )
        {
             m_cm.writeRecord(login->getUser(), info->fileName, "009");
        }
        // 获得上传列表实例
        uploadtask *uploadList = uploadtask::getInstance();
        if( uploadList == NULL)
        {
            cout << "uploadtask::getInstance() == NULL";
            return ;
        }
        uploadList->dealUploadTask();
    });
}

//=====文件item展示==========
// 清空文件列表
void MyFileWg::clearFileList()
{
    for( int i = 0; i < m_filelist.size(); i++)
    {
        FileInfo *tmp = m_filelist.takeFirst();
        delete tmp;
    }
}
// 清空所有的item项目
void MyFileWg::clearItems()
{
    int n = ui->listWidget->count();
    for ( int i = 0; i < n ; i++)
    {
        // 每次去除队首元素
        QListWidgetItem *item = ui->listWidget->takeItem(0);
        delete  item;
    }
}
// 添加上传文件项目item
void MyFileWg::addUploadItem(QString iconPath,QString name)
{
    ui->listWidget->addItem( new QListWidgetItem(QIcon(iconPath),name));
}
// 文件item展示
void MyFileWg::refreshFileItem()
{
    // 清空所有item项目
    clearItems();

    // 如果文件列表不为空，则显示文件列表,将文件列表中的数据同步到显示
    if( m_filelist.isEmpty() == false)
    {
        int n = m_filelist.size();
        for ( int i = 0; i < n; i++)
        {
            FileInfo *tmp = m_filelist.at(i);
            // 取出控件，添加
            QListWidgetItem *item = tmp->item;

            ui->listWidget->addItem(item);
        }
    }
    // 添加上传文件item
    this->addUploadItem();
}

// =====显示用户的文件列表======

// 得到服务器的json
QStringList MyFileWg::getCountStatus(QByteArray json)
{
    QJsonParseError error;
    QStringList list;

    //cout << "json = " << json.data();

    //将来源数据json转化为JsonDocument
    //由QByteArray对象构造一个QJsonDocument对象，用于我们的读写操作
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    if (error.error == QJsonParseError::NoError) //没有出错
    {
        if (doc.isNull() || doc.isEmpty())
        {
            cout << "doc.isNull() || doc.isEmpty()";
            return list;
        }

        if( doc.isObject() )
        {
            QJsonObject obj = doc.object();//取得最外层这个大对象
            list.append( obj.value( "token" ).toString() ); //登陆token
            list.append( obj.value( "num" ).toString() ); //文件个数
        }
    }
    else
    {
        cout << "err = " << error.errorString();
    }

    return list;
}
// 显示用户的文件列表
void  MyFileWg::refreshFiles(Display cmd)
{
    // 先获取用户文件数目
    M_userFileCount = 0;
    QNetworkRequest request;

    // 获得登录信息实例
    logininfoinstance *login = logininfoinstance::getInstance();

    QString url = QString("http://%1:%2/myfiles?cmd=count").arg(login->getIp()).arg(login->getPort());
    request.setUrl(QUrl( url )); //设置url

    // qt默认的请求头
    // request.setRawHeader("Content-Type","text/html");
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QByteArray data = setGetCountJson( login->getUser(), login->getToken());

    // 发送post请求
    QNetworkReply * reply = m_manager->post( request, data );
    if(reply == NULL)
    {
        cout << "reply == NULL";
        return;
    }

    // 获取请求的数据完成时，就会发送信号SIGNAL(finished())
    connect(reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError) //有错误
        {
            cout << reply->errorString();
            reply->deleteLater(); //释放资源
            return;
        }

        // 服务器返回数据
        QByteArray array = reply->readAll();
        cout << "server return file ...: " << array;

        reply->deleteLater(); //释放

        // 得到服务器json文件
        QStringList list = getCountStatus(array);

        // token验证失败
        if( list.at(0) == "111" )
        {
            QMessageBox::warning(this, "账户异常", "请重新登陆！！！");

            emit loginAgainSignal(); //发送重新登陆信号

            return; //中断
        }

        // 转换为long
        M_userFileCount = list.at(1).toLong();
        cout << "userFilesCount = " << M_userFileCount;

        // 清空文件列表信息
        clearFileList();

        if(M_userFileCount > 0)
        {
            // 说明有用户文件，获取用户文件列表
            // 从0开始
            m_start = 0;
            // 每次请求十个
            m_count = 10;
            // 获取新的文件列表信息
            getUserFileList(cmd);
        }
        else // 没有文件
        {
            // 更新item
            refreshFileItem();
        }
    });
}
// 设置json包
QByteArray MyFileWg::setGetCountJson(QString user, QString token)
{
    QMap<QString, QVariant> tmp;
    tmp.insert("user", user);
    tmp.insert("token", token);

    /*json数据如下
    {
        user:xxxx
        token: xxxx
    }
    */
    QJsonDocument jsonDocument = QJsonDocument::fromVariant(tmp);
    if ( jsonDocument.isNull() )
    {
        cout << " jsonDocument.isNull() ";
        return "";
    }

    //cout << jsonDocument.toJson().data();
    return jsonDocument.toJson();
}
QByteArray MyFileWg::setFileListJson(QString user, QString token, int start, int count)
{
    /*{
        "user": "yoyo"
        "token": "xxx"
        "start": 0
        "count": 10
    }*/
    QMap<QString, QVariant> tmp;
    tmp.insert("user", user);
    tmp.insert("token", token);
    tmp.insert("start", start);
    tmp.insert("count", count);

    QJsonDocument jsonDocument = QJsonDocument::fromVariant(tmp);
    if ( jsonDocument.isNull() )
    {
        cout << " jsonDocument.isNull() ";
        return "";
    }

    //cout << jsonDocument.toJson().data();

    return jsonDocument.toJson();
}
// 获取用户文件列表
void MyFileWg::getUserFileList(Display cmd )
{
    if( M_userFileCount <= 0) //结束条件，函数递归的结束条件
    {
        cout << "获取用户文件列表条件结束";
        refreshFileItem(); // 更新item
        return ; // 中断条件

    }
    // 将要请求的文件数量小于剩余的文件数量
    else if( m_count > M_userFileCount )
    {
        m_count = M_userFileCount;
    }
    QNetworkRequest request; //请求对象

    // 获取登陆信息实例
    logininfoinstance *login = logininfoinstance::getInstance(); //获取单例

    // 获取用户文件信息 127.0.0.1:80/myfiles&cmd=normal
    // 按下载量升序 127.0.0.1:80/myfiles?cmd=pvasc
    // 按下载量降序127.0.0.1:80/myfiles?cmd=pvdesc
    QString url;

    QString tmp;
    //cmd取值，Normal：普通用户列表，PvAsc：按下载量升序， PvDesc：按下载量降序
    if(cmd == Normal)
    {
        tmp = "normal";
    }
    else if(cmd == PvAsc)
    {
        tmp = "pvasc";
    }
    else if(cmd == PvDesc)
    {
        tmp = "pvdesc";
    }

    url = QString("http://%1:%2/myfiles?cmd=%3").arg(login->getIp()).arg(login->getPort()).arg(tmp);
    request.setUrl(QUrl( url )); //设置url
    cout << "myfiles url: " << url;

    //qt默认的请求头
    //request.setRawHeader("Content-Type","text/html");
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    /*
    {
        "user": "yoyo"
        "token": "xxxx"
        "start": 0
        "count": 10
    }
    */
    QByteArray data = setFileListJson( login->getUser(), login->getToken(), m_start, m_count);

    //改变下一次请求时起点位置
    m_start += m_count;
    M_userFileCount -= m_count;

    //发送post请求
    QNetworkReply * reply = m_manager->post( request, data );
    if(reply == NULL)
    {
        cout << "reply == NULL";
        return;
    }

    //获取请求的数据完成时，就会发送信号SIGNAL(finished())
    connect(reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError) //有错误
        {
            cout << reply->errorString();
            reply->deleteLater(); //释放资源
            return;
        }

        //服务器返回用户的数据
        QByteArray array = reply->readAll();
        cout << "file info:" << array;

        reply->deleteLater();

        //token验证失败
        if("111" == m_cm.getCode(array) ) //common.h
        {
            QMessageBox::warning(this, "账户异常", "请重新登陆！！！");
            emit loginAgainSignal(); //发送重新登陆信号

            return; //中断
        }

        //不是错误码就处理文件列表json信息
        if("015" != m_cm.getCode(array) ) //common.h
        {
            //cout << array.data();
            getFileJsonInfo(array);//解析文件列表json信息，存放在文件列表中

            //继续获取用户文件列表
            getUserFileList(cmd);
        }
    });
}
// 解析文件列表json信息，存放在文件列表中
void MyFileWg::getFileJsonInfo(QByteArray data)
{
    QJsonParseError error;

    /*
    {
    "user": "yoyo",
    "md5": "e8ea6031b779ac26c319ddf949ad9d8d",
    "time": "2017-02-26 21:35:25",
    "filename": "test.mp4",
    "share_status": 0,
    "pv": 0,
    "url": "http://192.168.31.109:80/group1/M00/00/00/wKgfbViy2Z2AJ-FTAaM3As-g3Z0782.mp4",
    "size": 27473666,
     "type": "mp4"
    }
    */
    //-- user	文件所属用户
    //-- md5 文件md5
    //-- createtime 文件创建时间
    //-- filename 文件名字
    //-- shared_status 共享状态, 0为没有共享， 1为共享
    //-- pv 文件下载量，默认值为0，下载一次加1
    //-- url 文件url
    //-- size 文件大小, 以字节为单位
    //-- type 文件类型： png, zip, mp4……

    //将来源数据json转化为JsonDocument
    //由QByteArray对象构造一个QJsonDocument对象，用于我们的读写操作
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error == QJsonParseError::NoError) //没有出错
    {
        if (doc.isNull() || doc.isEmpty())
        {
            cout << "doc.isNull() || doc.isEmpty()";
            return;
        }

        if( doc.isObject())
        {
            //QJsonObject json对象，描述json数据中{}括起来部分
            QJsonObject obj = doc.object();//取得最外层这个大对象

            //获取games所对应的数组
            //QJsonArray json数组，描述json数据中[]括起来部分
            QJsonArray array = obj.value("files").toArray();

            int size = array.size();   //数组个数
            cout << "size = " << size;

            for(int i = 0; i < size; ++i)
            {
                QJsonObject tmp = array[i].toObject(); //取第i个对象
                /*
                    //文件信息
                    struct FileInfo
                    {
                        QString md5;        //文件md5码
                        QString filename;   //文件名字
                        QString user;       //用户
                        QString time;       //上传时间
                        QString url;        //url
                        QString type;       //文件类型
                        qint64 size;        //文件大小
                        int shareStatus;    //是否共享, 1共享， 0不共享
                        int pv;             //下载量
                        QListWidgetItem *item; //list widget 的item
                    };

                    {
                    "user": "yoyo",
                    "md5": "e8ea6031b779ac26c319ddf949ad9d8d",
                    "time": "2017-02-26 21:35:25",
                    "filename": "test.mp4",
                    "share_status": 0,
                    "pv": 0,
                    "url": "http://192.168.31.109:80/group1/M00/00/00/wKgfbViy2Z2AJ-FTAaM3As-g3Z0782.mp4",
                    "size": 27473666,
                     "type": "mp4"
                    }
                */
                FileInfo *info = new FileInfo;
                info->user = tmp.value("user").toString(); //用户
                info->md5 = tmp.value("md5").toString(); //文件md5
                info->time = tmp.value("time").toString(); //上传时间
                info->filename = tmp.value("filename").toString(); //文件名字
                info->shareStatus = tmp.value("share_status").toInt(); //共享状态
                info->pv = tmp.value("pv").toInt(); //下载量
                info->url = tmp.value("url").toString(); //url
                info->size = tmp.value("size").toInt(); //文件大小，以字节为单位
                info->type = tmp.value("type").toString();//文件后缀
                QString type = info->type + ".png";
                info->item = new QListWidgetItem(QIcon( m_cm.getFileType(type) ), info->filename);

                //list添加节点
                m_filelist.append(info);
            }
        }
    }
    else
    {
        cout << "err = " << error.errorString();
    }
}

// =========分享 删除文件=========
// 处理选中的文件
void MyFileWg::dealSelectdFile(QString cmd)
{
    // 获取当前选中的item
    QListWidgetItem *item = ui->listWidget->currentItem();
    if(item == NULL)
    {
        return ;
    }

    // 查找文件列表匹配的元素
    for( int i = 0; i < m_filelist.size(); i++)
    {
         if(m_filelist.at(i)->item == item)
        {
              if(cmd == "分享")
              {
                  // 分享这个文件
                  shareFile(m_filelist.at(i));
              }
              else if(cmd == "删除")
              {
                  delFile( m_filelist.at(i) ); //删除某个文件
              }
              else if(cmd == "属性")
              {
                  getFileProperty( m_filelist.at(i) ); //获取属性信息
              }
              break; //跳出循环


        }
    }
}

// 设置json包
QByteArray MyFileWg::setDealFileJson(QString user, QString token, QString md5, QString filename)
{
    /*
    {
        "user": "yoyo",
        "token": "xxxx",
        "md5": "xxx",
        "filename": "xxx"
    }
    */
    QMap<QString, QVariant> tmp;
    tmp.insert("user", user);
    tmp.insert("token", token);
    tmp.insert("md5", md5);
    tmp.insert("filename", filename);

    QJsonDocument jsonDocument = QJsonDocument::fromVariant(tmp);
    if ( jsonDocument.isNull() )
    {
        cout << " jsonDocument.isNull() ";
        return "";
    }
    //cout << jsonDocument.toJson().data();

    return jsonDocument.toJson();
}

// 分享文件
void MyFileWg::shareFile(FileInfo *info)
{
    // 文件已经分享
    if(info->shareStatus == 1)
    {
        QMessageBox::warning(this, "此文件已经分享", "此文件已经分享！！！");
        return ;
    }
    QNetworkRequest request; //请求对象

    //获取登陆信息实例
    logininfoinstance *login = logininfoinstance::getInstance(); //获取单例

    //127.0.0.1:80/dealfile?cmd=share
    QString url = QString("http://%1:%2/dealfile?cmd=share").arg(login->getIp()).arg(login->getPort());
    request.setUrl(QUrl( url )); //设置url

    //qt默认的请求头
    //request.setRawHeader("Content-Type","text/html");
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    /*
    {
        "user": "yoyo",
        "token": "xxxx",
        "md5": "xxx",
        "filename": "xxx"
    }
    */
    QByteArray data = setDealFileJson( login->getUser(),  login->getToken(), info->md5, info->filename);

    //发送post请求
    QNetworkReply * reply = m_manager->post( request, data );
    if(reply == NULL)
    {
        cout << "reply == NULL";
        return;
    }

    //获取请求的数据完成时，就会发送信号SIGNAL(finished())
    connect(reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError) //有错误
        {
            cout << reply->errorString();
            reply->deleteLater(); //释放资源
            return;
        }

        //服务器返回用户的数据
        QByteArray array = reply->readAll();

        reply->deleteLater();

        /*
            分享文件：
                成功：{"code":"010"}
                失败：{"code":"011"}
                别人已经分享此文件：{"code", "012"}

            token验证失败：{"code":"111"}

            */
        if("010" == m_cm.getCode(array) ) //common.h
        {
            //修改文件列表的属性信息
            info->shareStatus = 1; //设置此文件为已分享
            QMessageBox::information(this, "分享成功", QString("[%1] 分享成功!!!").arg(info->filename));
        }
        else if("011" == m_cm.getCode(array))
        {
            QMessageBox::warning(this, "分享失败", QString("[%1] 分享失败!!!").arg(info->filename));
        }
        else if("012" == m_cm.getCode(array))
        {
            QMessageBox::warning(this, "分享失败", QString("[%1] 别人已分享此文件!!!").arg(info->filename));
        }
        else if("111" == m_cm.getCode(array)) //token验证失败
        {
            QMessageBox::warning(this, "账户异常", "请重新登陆！！！");
            emit loginAgainSignal(); //发送重新登陆信号

            return;
        }
    });
}
// 删除文件
void MyFileWg::delFile(FileInfo *info)
{
    QNetworkRequest request; //请求对象

    //获取登陆信息实例
    logininfoinstance *login = logininfoinstance::getInstance(); //获取单例

    //127.0.0.1:80/dealfile?cmd=del
    QString url = QString("http://%1:%2/dealfile?cmd=del").arg(login->getIp()).arg(login->getPort());
    request.setUrl(QUrl( url )); //设置url

    //qt默认的请求头
    //request.setRawHeader("Content-Type","text/html");
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    /*
    {
        "user": "yoyo",
        "token": "xxxx",
        "md5": "xxx",
        "filename": "xxx"
    }
    */
    QByteArray data = setDealFileJson( login->getUser(),  login->getToken(), info->md5, info->filename);

    //发送post请求
    QNetworkReply * reply = m_manager->post( request, data );
    if(reply == NULL)
    {
        cout << "reply == NULL";
        return;
    }

    //获取请求的数据完成时，就会发送信号SIGNAL(finished())
    connect(reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError) //有错误
        {
            cout << reply->errorString();
            reply->deleteLater(); //释放资源
            return;
        }

        //服务器返回用户的数据
        QByteArray array = reply->readAll();

        reply->deleteLater();

        /*
            删除文件：
                成功：{"code":"013"}
                失败：{"code":"014"}
            */
        if ( "013" == m_cm.getCode(array) )
        {
            QMessageBox::information(this, "文件删除成功", QString("[%1]删除成功！！！").arg(info->filename));
            // 从文件列表中移除该文件，移除列表视图中的此item
            for(int i = 0; i < m_filelist.size(); i++)
            {
                if( m_filelist.at(i) == info)
                {
                    QListWidgetItem *item = info->item;
                    // 从列表视图中删除此item
                    ui->listWidget->removeItemWidget(item);
                    delete  item;

                    m_filelist.removeAt(i);
                    delete info;
                    break;
                }
            }
        }
        else if ( "014" == m_cm.getCode(array) )
        {
            QMessageBox::warning(this ,"文件删除失败", QString("[%1]删除失败！！！").arg(info->filename));
        }
        else if("111" == m_cm.getCode(array)) // token验证失败
        {
            QMessageBox::warning(this ,"账户异常", "请重新登录！！！");
            emit loginAgainSignal();  // 发送重新登录信号

            return ;
        }
    });
}
// 获取文件属性
void MyFileWg::getFileProperty(FileInfo *info)
{
    FilePropertyInfo dlg; // 创建对话框
    dlg.setInfo(info);
    dlg.exec();
}
// 下载文件处理
// 添加需要下载的文件到下载任务列表
void MyFileWg::addDownloadFiles()
{
    DownloadTask *p = DownloadTask::getInstance();
    if(p == NULL)
    {
        cout << "DownloadTask::getInstance() == NULL";
        return;
    }

    if( p->isEmpty() ) //如果队列为空，说明没有任务
    {
        return;
    }

    if( p->isDownload() ) //当前时间没有任务在下载，才能下载，单任务
    {
        return;
    }

    //看是否是共享文件下载任务，不是才能往下执行, 如果是，则中断程序
    if(p->isShareTask() == true)
    {
        return;
    }

    DownloadInfo *tmp = p->takeTask(); //取下载任务


    QUrl url = tmp->url;
    QFile *file = tmp->file;
    QString md5 = tmp->md5;
    QString user = tmp->user;
    QString filename = tmp->filename;
    DataProgress *dp = tmp->dp;

    //发送get请求
    QNetworkReply * reply = m_manager->get( QNetworkRequest(url) );
    if(reply == NULL)
    {
        p->dealDownloadTask(); //删除任务
        cout << "get err";
        return;
    }

    //获取请求的数据完成时，就会发送信号SIGNAL(finished())
    connect(reply, &QNetworkReply::finished, [=]()
    {
        cout << "下载完成";
        reply->deleteLater();

        p->dealDownloadTask();//删除下载任务

        m_cm.writeRecord(user, filename, "010"); //下载文件成功，记录

        dealFilePv(md5, filename); //下载文件pv字段处理
    });

    //当有可用数据时，reply 就会发出readyRead()信号，我们这时就可以将可用的数据保存下来
    connect(reply, &QNetworkReply::readyRead, [=]()
    {
        //如果文件存在，则写入文件
        if (file != NULL)
        {
            file->write(reply->readAll());
        }
    });

    //有可用数据更新时
    connect(reply, &QNetworkReply::downloadProgress, [=](qint64 bytesRead, qint64 totalBytes)
    {
        dp->setProgress(bytesRead, totalBytes);//设置进度
    }
    );
}
// 下载文件处理，取出下载任务列表的队首任务，下载完成后再去下一个任务点
void MyFileWg::downloadFilesAction()
{
    DownloadTask *p = DownloadTask::getInstance();
    if(p == NULL)
    {
        cout << "DownloadTask::getInstance() == NULL";
        return;
    }

    if( p->isEmpty() ) //如果队列为空，说明没有任务
    {
        return;
    }

    if( p->isDownload() ) //当前时间没有任务在下载，才能下载，单任务
    {
        return;
    }

    //看是否是共享文件下载任务，不是才能往下执行, 如果是，则中断程序
    if(p->isShareTask() == true)
    {
        return;
    }

    DownloadInfo *tmp = p->takeTask(); //取下载任务


    QUrl url = tmp->url;
    QFile *file = tmp->file;
    QString md5 = tmp->md5;
    QString user = tmp->user;
    QString filename = tmp->filename;
    DataProgress *dp = tmp->dp;

    //发送get请求
    QNetworkReply * reply = m_manager->get( QNetworkRequest(url) );
    if(reply == NULL)
    {
        p->dealDownloadTask(); //删除任务
        cout << "get err";
        return;
    }

    //获取请求的数据完成时，就会发送信号SIGNAL(finished())
    connect(reply, &QNetworkReply::finished, [=]()
    {
        cout << "下载完成";
        reply->deleteLater();

        p->dealDownloadTask();//删除下载任务

        m_cm.writeRecord(user, filename, "010"); //下载文件成功，记录

        dealFilePv(md5, filename); //下载文件pv字段处理
    });

    //当有可用数据时，reply 就会发出readyRead()信号，我们这时就可以将可用的数据保存下来
    connect(reply, &QNetworkReply::readyRead, [=]()
    {
        //如果文件存在，则写入文件
        if (file != NULL)
        {
            file->write(reply->readAll());
        }
    });

    //有可用数据更新时
    connect(reply, &QNetworkReply::downloadProgress, [=](qint64 bytesRead, qint64 totalBytes)
    {
        dp->setProgress(bytesRead, totalBytes);//设置进度
    }
    );
}

// ====下载文件标志处理======
// 下载文件pv字段处理
void MyFileWg::dealFilePv(QString md5, QString filename)
{
    QNetworkRequest request; //请求对象

    //获取登陆信息实例
    logininfoinstance *login = logininfoinstance::getInstance(); //获取单例

    //127.0.0.1:80/dealfile?cmd=pv
    QString url = QString("http://%1:%2/dealfile?cmd=pv").arg(login->getIp()).arg(login->getPort());
    request.setUrl(QUrl( url )); //设置url

    //qt默认的请求头
    //request.setRawHeader("Content-Type","text/html");
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    /*
    {
        "user": "yoyo",
        "token": "xxx",
        "md5": "xxx",
        "filename": "xxx"
    }
    */
    QByteArray data = setDealFileJson( login->getUser(), login->getToken(), md5, filename); //设置json包

    //发送post请求
    QNetworkReply * reply = m_manager->post( request, data );
    if(reply == NULL)
    {
        cout << "reply == NULL";
        return;
    }

    //获取请求的数据完成时，就会发送信号SIGNAL(finished())
    connect(reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError) //有错误
        {
            cout << reply->errorString();
            reply->deleteLater(); //释放资源
            return;
        }

        //服务器返回用户的数据
        QByteArray array = reply->readAll();

        reply->deleteLater();

        /*
            下载文件pv字段处理
                成功：{"code":"016"}
                失败：{"code":"017"}
            */
        if("016" == m_cm.getCode(array) ) //common.h
        {
            //该文件pv字段+1
            for(int i = 0; i < m_filelist.size(); ++i)
            {
                FileInfo *info = m_filelist.at(i);
                if( info->md5 == md5 && info->filename == filename)
                {
                    int pv = info->pv;
                    info->pv = pv+1;

                    break; //很重要的中断条件
                }
            }
        }
        else
        {
            cout << "下载文件pv字段处理失败";
        }
    });
}
// 清除上传下载任务
void MyFileWg::clearALLTask()
{
    // 获取上传列表实例
    uploadtask * uploadList = uploadtask::getInstance();
    if(uploadList == NULL)
    {
        cout << "UploadTask::getInstance() == NULL";
        return;
    }

    uploadList->clearList();

    //获取下载列表实例
    DownloadTask *p = DownloadTask::getInstance();
    if(p == NULL)
    {
        cout << "DownloadTask::getInstance() == NULL";
        return;
    }

    p->clearList();
}
// 定时检查处理任务队列中的任务
void MyFileWg::checkTaskList()
{
    // 定时检查上传队列是否有任务需要上传
    connect(&m_uploadFileTimer ,&QTimer::timeout,[=](){
        // 上传文件处理
        uploadFileAction();
    });
    // 启动定时器，500ms间隔
    // 每隔500ms，检测上传任务，每次上传一个
    m_uploadFileTimer.start(500);

    // 定时检查下载队列是否有任务需要下载
    connect(&m_downloadTimer, &QTimer::timeout,[=](){
        // 上传文件处理
        uploadFileAction();
    });
    m_downloadTimer.start(500);
}
// 右键菜单信号的槽函数
void MyFileWg::rightMenu(const QPoint &pos)
{
    QListWidgetItem *item = ui->listWidget->itemAt(pos);
    // 没有点图标
    if(item == NULL)
    {
        // 在鼠标点击的地方显示右键菜单
        m_menuEmpty->exec(QCursor::pos() );
    }
    else
    {
        ui->listWidget->setCurrentItem(item);
        if(item->text() == "上传文件") // 如果是上传文件，没有右击菜单
        {
            return;
        }
        m_menu->exec(QCursor::pos());
    }
}

void MyFileWg::contexMenuEvent(QContextMenuEvent *event)
{

}
