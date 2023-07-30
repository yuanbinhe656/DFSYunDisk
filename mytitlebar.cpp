#include "mytitlebar.h"
#include "ui_mytitlebar.h"
#include "QMouseEvent"
MyTitleBar::MyTitleBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyTitleBar)
{
    ui->setupUi(this);
    ui->logo->setPixmap(QPixmap(":/logo.ico").scaled(40,40));

}
void MyTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    // 鼠标移动到了什么位置? -> 通过参数globalPos();
    // 得到位置之后 -> 让窗口跟随鼠标移动 -> move();
    // 移动的应该是外部的login窗口
    // event->globalPos()鼠标移动到了屏幕的什么位置, 要求的是窗口左上角的点
    // 左上角的位置 = 当前鼠标位置 - 相对位置;
    // 判断持续状态, 鼠标左键
    if(event->buttons() & Qt::LeftButton)
    {
        m_parent->move(QPoint(event->globalX(),event->globalY()) - m_pt);
        //m_parent->move(event->globalPosition().toPoint() - m_pt);
    }
}

void MyTitleBar::mousePressEvent(QMouseEvent *event)
{
    // 如果是鼠标左键, 瞬间状态
    if(event->button() == Qt::LeftButton)
    {
        // 相对位置 = 当前鼠标位置 - 左上角的位置;
        m_pt = QPoint(event->globalX() - m_parent->geometry().x(),event->globalY()- m_parent->geometry().y());

    }
}
MyTitleBar::~MyTitleBar()
{
    delete ui;
}

void MyTitleBar::on_setBtn_clicked()
{
     emit showSetWindow();
}


void MyTitleBar::on_minBtn_clicked()
{
     emit showMinWindow();
}


void MyTitleBar::on_closeBtn_clicked()
{
      emit closeMyWindow();
}
void MyTitleBar::setMyParent(QWidget *parent)
{
      m_parent = parent;
}
