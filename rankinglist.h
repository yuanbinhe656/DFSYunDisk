#ifndef RANKINGLIST_H
#define RANKINGLIST_H

#include <QWidget>
#include "common/common.h"

namespace Ui {
class RankingList;
}

// 文件信息
struct RankingFileInfo
{
    QString filename;  // 文件名字
    int pv;            // 下载量
};

class RankingList : public QWidget
{
    Q_OBJECT

public:
    explicit RankingList(QWidget *parent = nullptr);
    ~RankingList();

    // 设置tablewidget（）表头和属性
    void initTableWidget();

    // 清空文件列表
    void clearshareFileList();

    // 显示共享文件列表
    void refreshFiles();

    // 设置json包
    QByteArray setFileListJosn(int start, int count);
    // 获取共享文件列表
    void getUserFileList();
    // 解析文件列表json信息，存放在文件列表中
    void getFileJsonInfo(QByteArray data);

    // 更新排行榜列表
    void refreshList();


private:
    Ui::RankingList *ui;

    common m_cm;
    QNetworkAccessManager *m_manager;

    int m_start;                       // 文件起始位置
    int m_count;                       // 每次请求文件个数
    long m_userFilesCount;             // 用户文件数目
    QList<RankingFileInfo*> m_list;    // 文件列表
};

#endif // RANKINGLIST_H
