#ifndef UPLOADTASK_H
#define UPLOADTASK_H

#include "common.h"
#include <QVBoxLayout>
#include <QFile>
#include "selfwidget/dataprogress.h"

// 上传文件信息
struct UploadFileInfo
{
    QString md5;       // 文件md5
    QFile *file;       //文件指针
    QString fileName;  //文件名字
    qint64 size;       //文件大小
    QString path;      //文件路径
    bool isUpload;     //是否已经上传
    DataProgress *dp;  //上传进度条
};
// 单例模式
class uploadtask
{
public:
    static uploadtask *getInstance();

    // 追加上传文件到上传列表中
    // 参数 path文件上传路径 返回值 成功返回0 失败 -1 文件大于30m，-2：上传的文件已经在队列中 -3： 打开文件失败 -4：获取布局失败
    int appendUploadList(QString path);
    // 判断上传队列是否为空
    bool isEmpty();
    // 是否有文件正在上传
    bool isUpload();

    // 取出第0个任务
    UploadFileInfo * takeTask();

    // 删除上传完成的任务
    void dealUploadTask();

    // 清空下载列表
    void clearList();
private:
    uploadtask()    //构造函数为私有
    {
    }

    ~uploadtask()    //析构函数为私有
    {
    }

    //静态数据成员，类中声明，类外必须定义
    static uploadtask *instance;

    //它的唯一工作就是在析构函数中删除Singleton的实例
    class Garbo
    {
    public:
        ~Garbo()
        {
          if(NULL != uploadtask::instance)
          {
            uploadtask::instance->clearList();

            delete uploadtask::instance;
            uploadtask::instance = NULL;
            cout << "instance is detele";
          }
        }
    };

    //定义一个静态成员变量，程序结束时，系统会自动调用它的析构函数
    //static类的析构函数在main()退出后调用
    static Garbo temp; //静态数据成员，类中声明，类外定义

    QList<UploadFileInfo *> list; //上传任务列表(任务队列)
};

#endif // UPLOADTASK_H
