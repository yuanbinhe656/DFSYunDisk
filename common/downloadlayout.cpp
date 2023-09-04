#include "downloadlayout.h"

downloadlayout *downloadlayout::instance = new downloadlayout;

//static 类的析构函数在main退出后调用
downloadlayout::Garbo downloadlayout ::temp;

downloadlayout * downloadlayout::getinstance() // 保证唯一实例
{
    return instance;
}
void downloadlayout::setdownloadlayout(QWidget *p) //设置布局
{
    m_wg = new QWidget(p);
    QLayout *layout = p->layout();
    layout->addWidget(m_wg);
    layout->setContentsMargins(0,0,0,0);
    QVBoxLayout *vlayout = new QVBoxLayout;
    // 布局设置给窗口
    m_wg->setLayout(vlayout);

    // 边界间隔
    vlayout->setContentsMargins(0,0,0,0);
    m_layout = vlayout;
    vlayout->addStretch();

}
QLayout * downloadlayout::getdownloadlayout() //获取布局
{
    return m_layout;
}
