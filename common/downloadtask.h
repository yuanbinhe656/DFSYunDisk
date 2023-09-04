#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H
#include "common.h"

#include <QVBoxLayout>
#include <QUrl>
#include <QFile>
#include "selfwidget/dataprogress.h"
#include <QList>

// 下载文件信息
struct DownloadInfo{
    QFile *file;       // 文件指针
    QString user;      // 下载用户
    QString filename;  // 文件名字
    QString md5;       // 文件md5
    QUrl url;          //下载连接
    bool isDownload;   // 是否已经在下载
    DataProgress *dp;  // 下载进度控件
    bool isShare;      // 是否为共享文件下载
};

// 下载任务列表类，单例模式，一个程序只能有一个下载任务列表
class DownloadTask
{
public:
    // 唯一实例
    static DownloadTask  *getInstance();

    // 清空上传列表
    void clearList();
    // 判断队列是否为空
    bool isEmpty();
    // 是否有文件在下载
    bool isDownload();
    // 第一个任务是否为共享文件的任务
    bool isShareTask();
    //取出第0个任务
    DownloadInfo *takeTask();
    // 删除完成下载的任务
    void dealDownloadTask();

    // 追加任务到下载队列
    // 参数： info：下载文件信息 filePathName：文件保存路径 isshare：是否为共享文件下载，默认为false；
    // 成功 : 0  失败：-1 下载的文件已经在队列中，-2 打开文件失败
    int appendDownloadList( FileInfo *info, QString filePathName, bool isShare = false);

private:
    DownloadTask()    //构造函数为私有
    {
    }

    ~DownloadTask()    //析构函数为私有
    {
    }

    //静态数据成员，类中声明，类外必须定义
    static DownloadTask *instance;

    //它的唯一工作就是在析构函数中删除Singleton的实例
    class Garbo
    {
    public:
        ~Garbo()
        {
          if(NULL != DownloadTask::instance)
          {
            DownloadTask::instance->clearList();//清空上传列表

            delete DownloadTask::instance;
            DownloadTask::instance = NULL;
            cout << "instance is detele";
          }
        }
    };

    //定义一个静态成员变量，程序结束时，系统会自动调用它的析构函数
    //static类的析构函数在main()退出后调用
    static Garbo temp; //静态数据成员，类中声明，类外定义

    // 下载的任务队列
    QList <DownloadInfo *> list;
};

#endif // DOWNLOADTASK_H
