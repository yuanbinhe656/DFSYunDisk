#include "uploadtask.h"
#include <QFileInfo>
#include "uploadlayout.h"
#include "common.h"

uploadtask* uploadtask::instance = new uploadtask;
uploadtask::Garbo uploadtask::temp;

uploadtask *uploadtask::getInstance()
{
    return instance;
}

// 追加上传文件到上传列表中
// 参数 path文件上传路径 返回值 成功返回0 失败 -1 文件大于30m，-2：上传的文件已经在队列中 -3： 打开文件失败 -4：获取布局失败
int uploadtask::appendUploadList(QString path)
{
    qint64 size = QFileInfo(path).size();
    if( size > 30 * 1024 *1024) // 最大文件只支持30m
    {
        cout << "file is to big \n";
        return -1;
    }
    // 遍历查看一下，下载的文件是否已经在上传队列中
    for(int i = 0; i < list.size(); i++)
    {
        if( list.at(i)->path == path)
        {
            return -2;
        }
    }
    QFile *file = new QFile(path); // 打开文件
    if(!file->open(QIODevice::ReadOnly))
    {
        // 如果文件打开失败，则删除file， 并使file指针为null，返回
        cout << "file open error";
        delete  file;
        file = NULL;
        return -3;
    }

    // 获取文件信息
    QFileInfo info(path);

    // 动态创建节点
    common mc;
    UploadFileInfo *tmp = new UploadFileInfo;
    tmp->md5 = mc.getFileMd5(path); //获取文件的md5码, common.h
    tmp->file = file; //文件指针
    tmp->fileName = info.fileName();//文件名字
    tmp->size = size; //文件大小
    tmp->path = path; //文件路径
    tmp->isUpload = false;//当前文件没有被上传

    DataProgress *p = new DataProgress;
    p->setFileName(tmp->fileName);
    tmp->dp = p;

    // 获取布局
    UploadLayout *pupload = UploadLayout::getInstance();
    if( pupload == NULL)
    {
        cout << " UploadLayout::getInstance() == NULL";
        return -4;
    }
    QVBoxLayout *layout = (QVBoxLayout *)pupload->getUploadLayout();
    // 添加布局，最后一个为弹簧
    layout->insertWidget(layout->count()-1, p);
    cout << tmp->fileName.toUtf8().data() << "已经在上传列表中";
    // 插入节点
    list.append(tmp);
    return 0;
}
// 判断上传队列是否为空
bool uploadtask::isEmpty()
{
    return list.isEmpty();
}
// 是否有文件正在上传
bool uploadtask::isUpload()
{
    // 遍历列表
    for(int i = 0; i != list.size(); i++)
    {
        if(list.at(i)->isUpload == true)
        {
            return true;
        }
    }
   return false;
}

// 取出第0个任务  每次只能上传一个任务，上传前将其设置为上传标志
UploadFileInfo * uploadtask::takeTask()
{
    if(list.isEmpty())
    {
        return NULL;
    }
    list.at(0)->isUpload = true;
    return list.at(0);
}

// 删除上传完成的任务  每次上传结束后检查对列中是否有上传任务
void uploadtask::dealUploadTask()
{
    for (int i = 0; i < list.size() ; i++)
    {
        if( list.at(i)->isUpload == true)
        {
            // 移除文件
            UploadFileInfo *tmp = list.takeAt(i);

            // 获得布局
            UploadLayout *pupload = UploadLayout::getInstance();
            QLayout *layout = pupload->getUploadLayout();
            layout->removeWidget(tmp->dp);
            QFile *file = tmp->file;
            file->close();
            delete file;
            delete  tmp->dp;
            // 释放空间
            delete  tmp;
            return ;
        }
    }
}

// 清空下载列表
void uploadtask::clearList()
{
    int n = list.size();
    for (int i = 0; i < n ; i++)
    {
        UploadFileInfo *tmp = list.takeFirst();
        delete tmp;
    }
}
