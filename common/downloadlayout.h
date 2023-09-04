#ifndef DOWNLOADLAYOUT_H
#define DOWNLOADLAYOUT_H
#include "common.h"
#include <QVBoxLayout>

// 下载进度布局类 ，单例模式
class downloadlayout
{
public:
    static downloadlayout*getinstance(); // 保证唯一实例
    void setdownloadlayout(QWidget *p); //设置布局
    QLayout *getdownloadlayout(); //获取布局

private:
    downloadlayout()
    {

    }
    ~downloadlayout()  // 析构设为私有
    {

    }
    // 静态数据成员
    static downloadlayout *instance;
    QLayout *m_layout;
    QWidget *m_wg;

    // 在析构函数中删除signleton实例
    class Garbo
    {
    public:
        ~Garbo()
        {
            if( NULL != downloadlayout::instance )
            {
                delete downloadlayout::instance;
                downloadlayout::instance = NULL;
                cout << "instance is delete";
            }
        }
    };

    // 定义一个静态成员变量，程序结束时，自动调用析构函数
    static Garbo  temp;
};

#endif // DOWNLOADLAYOUT_H
