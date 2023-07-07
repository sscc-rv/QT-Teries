#include "mainwindow.h"
#include <QTextCodec>
#include <QTime>
#include <QApplication>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
    // 设置随机数的初始值
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    // 添加启动画面
    QSplashScreen *splash=new QSplashScreen;
    splash->setPixmap(QPixmap(":/images/libai.jpg"));
    splash->showMessage(QObject::tr("<h1><font color=white>欢迎来到俄罗斯方块!</h1>"),Qt::AlignCenter);
     splash->show();
     for(int i=0;i<1000;i++)
     {
         splash->repaint();
     }
     a.processEvents();

    MainWindow w;
    w.show();
    splash->finish(&w);
    delete splash;
    return a.exec();
}
