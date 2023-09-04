#include "downloadtask.h"
#include "downloadlayout.h"
DownloadTask * DownloadTask::instance ;
DownloadTask::Garbo  DownloadTask::temp;

// 唯一实例
DownloadTask  * DownloadTask:: getInstance()
{
    if( instance == NULL)
    {
        instance = new DownloadTask();
    }
    return instance;
}

// 清空上传列表
void DownloadTask:: clearList()
{
    int n = list.size();
    for (int i = 0; i < n ; i++)
    {
        DownloadInfo *temp = list.takeFirst();
        delete  temp;
    }
}

// 判断队列是否为空
bool DownloadTask:: isEmpty()
{
    return list.isEmpty();
}

// 是否有文件在下载
bool DownloadTask:: isDownload()
{
    // 遍历队列
    for( int i = 0; i != list.size(); i++)
    {
        if( list.at(i)->isDownload = true)
        {
            return true;
        }
    }
    return false;
}

// 第一个任务是否为共享文件的任务
bool DownloadTask:: isShareTask()
{
    // 遍历队列
    if(isEmpty())
    {
        return NULL;
    }
    return list.at(0)->isShare;
}

//取出第0个任务
DownloadInfo * DownloadTask:: takeTask()
{
    if( isEmpty())
    {
        return NULL;
    }
    list.at(0)->isDownload = true;
    return list.at(0);
}

// 删除完成下载的任务
void DownloadTask:: dealDownloadTask()
{
    for (int i = 0; i < list.size() ; i++)
    {
        // 有下载任务
        if( list.at(i)->isDownload == true)
        {
            // 移除文件，已经上传完成
            DownloadInfo *temp = list.takeAt(i);

            // 获取布局
            downloadlayout *downloadLayout = downloadlayout::getinstance();
            QLayout *layout = downloadLayout->getdownloadlayout();

            // 删除该文件对应的下载进度控件
            layout->removeWidget(temp->dp);
            delete temp->dp;
            // 关闭文件指针
            QFile *file = temp->file;
            file->close();
            delete file;

            delete  temp;
            return;

        }
    }
}


// 追加任务到下载队列
// 参数： info：下载文件信息 filePathName：文件保存路径 isshare：是否为共享文件下载，默认为false；
// 成功 : 0  失败：-1 下载的文件已经在队列中，-2 打开文件失败
int DownloadTask:: appendDownloadList( FileInfo *info, QString filePathName, bool isShare)
{
    // 遍历查看一下下载的文件是否已经在下载队列中
    for(int i = 0; i < list.size(); i++)
    {
        if(list.at(i)->user == info->user && list.at(i)->filename == info->filename)
        {
            cout << info->filename << "已经在下载队列";
            return -1;
        }

    }
    QFile *file = new QFile(filePathName);
    // 打开文件失败
    if( !file->open(QIODevice::WriteOnly))
    {
        cout << "file open err";
        delete  file;
        file = NULL;
        return -2;
    }

    // 动态创建节点
    DownloadInfo *tmp = new DownloadInfo;
    tmp->user = info->user;   //用户
    tmp->file = file;         //文件指针
    tmp->filename = info->filename; //文件名字
    tmp->md5 = info->md5;           //文件md5
    tmp->url = info->url;           //下载网址
    tmp->isDownload = false;        //没有在下载
    tmp->isShare = isShare;         //是否为共享文件下载
    DataProgress *p = new DataProgress;
    p->setFileName(tmp->filename);

    // 获取布局
    downloadlayout * downloadlay =  downloadlayout::getinstance();
    QVBoxLayout *layout = (QVBoxLayout*)downloadlay->getdownloadlayout() ;
    tmp->dp = p;
    // 添加到布局
    // 最后一个是弹簧，需要添加到倒数第二个
    layout->insertWidget(layout->count()-1,p);
    cout << info->url;

    // 插入节点
    list.append(tmp);
    return 0;
}

