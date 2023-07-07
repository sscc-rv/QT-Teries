#include "mainwindow.h"
#include "mybox.h"
#include <QIcon>
#include <QPropertyAnimation>
#include <QGraphicsBlurEffect>
#include <QTimer>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QLabel>
#include <QRectF>

static const qreal INITSPEED = 900;//游戏的初始化速度

MainWindow::MainWindow(QWidget *parent)
    : QGraphicsView(parent)
{
    initView();
}

// 初始化游戏界面
void MainWindow::initView()
{
    // 使用抗锯齿渲染
    setRenderHint(QPainter::Antialiasing);
    // 设置缓存背景，这样可以加快渲染速度
    setCacheMode(CacheBackground);
    setWindowTitle(QString("我们的四维空间"));
    setWindowIcon(QIcon(":/images/icon.png"));
    setMinimumSize(810,510 );
    setMaximumSize(810, 510);

    // 设置场景
    QGraphicsScene *scene = new QGraphicsScene;
    scene->setSceneRect(5, 5, 800, 500);
    scene->setBackgroundBrush(QPixmap(":/images/libai22.jpg"));
    setScene(scene);

    // 方块可移动区域的四条边界线
    topLine = scene->addLine(197, 47, 403, 47);
    bottomLine = scene->addLine(197,453,403,453);
    leftLine = scene->addLine(197,47,197,453);
    rightLine = scene->addLine(403,47,403,453);


    // 当前方块组和提示方块组
    boxGroup = new BoxGroup;
    connect(boxGroup, SIGNAL(needNewBox()), this, SLOT(clearFullRows()));
    connect(boxGroup, SIGNAL(gameFinished()), this, SLOT(gameOver()));
    scene->addItem(boxGroup);
    nextBoxGroup = new BoxGroup;
    scene->addItem(nextBoxGroup);

    // 得分文本
    gameScoreText = new QGraphicsTextItem(0);
    gameScoreText->setFont(QFont("Times", 50, QFont::Bold));
    gameScoreText->setPos(650, 350);
    scene->addItem(gameScoreText);  
    // 级别文本
    gameLevelText = new QGraphicsTextItem(0);
    gameLevelText->setFont(QFont("Times", 30, QFont::Bold));
    gameLevelText->setPos(20, 150);
    scene->addItem(gameLevelText);

    // 设置初始为隐藏状态
    topLine->hide();
    bottomLine->hide();
    leftLine->hide();
    rightLine->hide();
    gameScoreText->hide();
    gameLevelText->hide();

    // 黑色遮罩
    QWidget *mask = new QWidget;
    mask->setAutoFillBackground(true);
    mask->setPalette(QPalette(QColor(0, 0, 0, 80)));
    mask->resize(900, 600);
    maskWidget = scene->addWidget(mask);
    maskWidget->setPos(-50, -50);
    // 设置其Z值为1，这样可以处于Z值为0的图形项上面
    maskWidget->setZValue(1);

    QPalette palette;
    // 帮助面板  就是游戏的玩法
    QWidget *help = new QWidget;
    QPushButton *helpCloseButton = new QPushButton(QString("关闭"), help);
    helpCloseButton->setStyleSheet("color:rgb(250,0,0)");
    helpCloseButton->setPalette(palette);
    helpCloseButton->move(100, 340);
    connect(helpCloseButton, SIGNAL(clicked()), help, SLOT(hide()));
    help->setAutoFillBackground(true);
    help->setPalette(QPalette(QColor(0, 0, 0, 180)));
    help->resize(300, 400);
    QGraphicsWidget *helpWidget = scene->addWidget(help);
    helpWidget->setPos(250, 50);
    helpWidget->setZValue(3);
    helpWidget->hide();

    QLabel *helpLabel = new QLabel(help);
    helpLabel->setText(QString("<h2><font color=white>空格:下落<br>左:向左移动<br>右:向右移动<br>上:转型<br>下:加速向下</font><h2>"));
    //设置对齐方式
    helpLabel->setAlignment(Qt::AlignCenter);
    helpLabel->move(100, 150);


    // 游戏欢迎文本
    gameWelcomeText = new QGraphicsTextItem(0);
    gameWelcomeText->setHtml(QString("<font color=white>   欢迎来到我们的四维空间"));
    gameWelcomeText->setFont(QFont("Times", 20, QFont::Bold));
    gameWelcomeText->setPos(200, 150);
    gameWelcomeText->setZValue(2);
    scene->addItem(gameWelcomeText);

    // 游戏暂停文本
    gamePausedText = new QGraphicsTextItem(0);
    gamePausedText->setHtml(QString("<font color=white>暂停!"));
    gamePausedText->setFont(QFont("Times", 30, QFont::Bold));
    gamePausedText->setPos(350, 100);
    gamePausedText->setZValue(2);
    gamePausedText->hide();
    scene->addItem(gamePausedText);

    // 游戏结束文本
    gameOverText = new QGraphicsTextItem(0);
    gameOverText->setHtml(QString("<font color=gray>游戏结束"));
    gameOverText->setFont(QFont("Times", 30, QFont::Bold));
    gameOverText->setPos(320, 100);
    gameOverText->setZValue(2);
    gameOverText->hide();
    scene->addItem(gameOverText);

    // 游戏中使用的按钮

    QPushButton *button1 = new QPushButton(QString("开始"));
    button1->setStyleSheet("background-color:rgb(175,238,238)");

    QPushButton *button3 = new QPushButton(QString("帮助"));
    button3->setStyleSheet("color:rgb(0,80,0)");
    QPushButton *button4 = new QPushButton(QString("退出"));
    button4->setStyleSheet("color:rgb(250,0,0)");

    QPushButton *button5 = new QPushButton(QString("重新开始"));
    button5->setStyleSheet("background-color:rgb(0,238,238)");
    QPushButton *button6 = new QPushButton(QString("暂停"));
    button6->setStyleSheet("background-color:rgb(175,238,238)");
    QPushButton *button7 = new QPushButton(QString("菜单"));
    button7->setStyleSheet("background-color:rgb(175,238,0)");
    QPushButton *button8 = new QPushButton(QString("返回"));
    button8->setStyleSheet("background-color:rgb(175,238,80)");
    QPushButton *button9 = new QPushButton(QString("结束"));


    connect(button1, SIGNAL(clicked()), this, SLOT(startGame()));
    connect(button3, SIGNAL(clicked()), help, SLOT(show()));
    connect(button4, SIGNAL(clicked()), qApp, SLOT(quit()));
    connect(button5, SIGNAL(clicked()), this, SLOT(restartGame()));
    connect(button6, SIGNAL(clicked()), this, SLOT(pauseGame()));
    connect(button7, SIGNAL(clicked()), this, SLOT(finishGame()));
    connect(button8, SIGNAL(clicked()), this, SLOT(returnGame()));
    connect(button9, SIGNAL(clicked()), this, SLOT(finishGame()));

    startButton = scene->addWidget(button1);
    helpButton = scene->addWidget(button3);
    exitButton = scene->addWidget(button4);
    restartButton = scene->addWidget(button5);
    pauseButton = scene->addWidget(button6);
    showMenuButton = scene->addWidget(button7);
    returnButton = scene->addWidget(button8);
    finishButton = scene->addWidget(button9);

    startButton->setPos(370, 250);
    helpButton->setPos(370, 300);
    exitButton->setPos(370, 350);
    restartButton->setPos(600, 150);
    pauseButton->setPos(600, 200);
    showMenuButton->setPos(600, 250);
    returnButton->setPos(370, 200);
    finishButton->setPos(370, 250);

    startButton->setZValue(2);
    helpButton->setZValue(2);
    exitButton->setZValue(2);
    restartButton->setZValue(2);
    returnButton->setZValue(2);
    finishButton->setZValue(2);

    restartButton->hide();
    finishButton->hide();
    pauseButton->hide();
    showMenuButton->hide();
    returnButton->hide();
}

// 开始游戏
void MainWindow::startGame()
{
    gameWelcomeText->hide();
    startButton->hide();
    helpButton->hide();
    exitButton->hide();
    maskWidget->hide();

    initGame();
}

// 初始化游戏
void MainWindow::initGame()
{
    boxGroup->createBox(QPointF(300, 70));
    boxGroup->setFocus();
    boxGroup->startTimer(INITSPEED);
    gameSpeed = INITSPEED;
    nextBoxGroup->createBox(QPointF(500, 70));
    scene()->setBackgroundBrush(QPixmap(":/images/xiangxiang1.jpg"));
    gameScoreText->setHtml(QString("<font color=green>0</font>"));
    gameLevelText->setHtml(QString("<font color=purple>First"));
    restartButton->show();
    pauseButton->show();
    showMenuButton->show();
    gameScoreText->show();
    gameLevelText->show();
    topLine->show();
    bottomLine->show();
    leftLine->show();
    rightLine->show();
    // 可能以前返回主菜单时隐藏了boxGroup
    boxGroup->show();
}

// 清空满行
void MainWindow::clearFullRows()
{
    // 获取比一行方格较大的矩形中包含的所有小方块
    for (int y = 429; y > 50; y -= 20)
    {
        QList<QGraphicsItem *> list = scene()->items(QRectF(199, y, 202, 22), Qt::ContainsItemShape);
        // 如果该行已满
        if (list.count() == 10)
        {
            foreach (QGraphicsItem *item, list)
            {
                OneBox *box = (OneBox*) item;
                //设置特效(blur:模糊的效果）
                QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect;
                box->setGraphicsEffect(blurEffect);
                QPropertyAnimation *animation = new QPropertyAnimation(box, "scale");
                //使用缓和曲线
                animation->setEasingCurve(QEasingCurve::OutBounce);
                //持续的时间
                animation->setDuration(250);
                animation->setStartValue(10);
                animation->setEndValue(0.25);
                animation->start(QAbstractAnimation::DeleteWhenStopped);
                connect(animation, SIGNAL(finished()), box, SLOT(deleteLater()));
            }
            // 保存满行的位置
            rows << y;
        }
    }
    // 如果有满行，下移满行上面的各行再出现新的方块组
    // 如果没有满行，则直接出现新的方块组
    if(rows.count() > 0)
   {

        QTimer::singleShot(400, this, SLOT(moveBox()));//经过0.4秒发射信号

   }
    else
    {
        boxGroup->createBox(QPointF(300, 70), nextBoxGroup->getCurrentShape());
        // 清空并销毁提示方块组中的所有小方块
        nextBoxGroup->clearBoxGroup(true);
        nextBoxGroup->createBox(QPointF(500, 70));
    }
}

// 下移满行上面的所有小方块
void MainWindow::moveBox()
{
    // 从位置最靠上的满行开始
    for (int i = rows.count(); i > 0;  --i)
    {
        int row = rows.at(i - 1);
        foreach (QGraphicsItem *item, scene()->items(QRectF(199, 49, 202, row - 47), Qt::ContainsItemShape))
        {
            item->moveBy(0, 20);
        }
    }
    // 更新分数
    updateScore(rows.count());
    // 将满行列表清空为0
    rows.clear();
    // 等所有行下移以后再出现新的方块组
    boxGroup->createBox(QPointF(300, 70), nextBoxGroup->getCurrentShape());
    nextBoxGroup->clearBoxGroup(true);
    nextBoxGroup->createBox(QPointF(500, 70));
}

// 更新分数
void MainWindow::updateScore(const int fullRowNum)
{
    int score = fullRowNum * 10;
    int currentScore = gameScoreText->toPlainText().toInt();
    currentScore += score;
    // 显示当前分数
    gameScoreText->setHtml(QString("<h3><font color=yellow>%2</font><h3>").arg(currentScore));
    // 判断级别
    if (currentScore < 50)
    {
        // 第一级,什么都不用做
    }
    else if (currentScore < 100)
    {
        // 第二级
         gameLevelText->setHtml(QString("<font color=black>Second"));
         scene()->setBackgroundBrush(QPixmap(":/images/libai02.jpg"));
         //下落速度700
         gameSpeed = 700;
         boxGroup->stopTimer();
         boxGroup->startTimer(gameSpeed);
    }
    else if (currentScore < 150)
    {
        // 第三级
         gameLevelText->setHtml(QString("<font color=green>Third"));
         scene()->setBackgroundBrush(QPixmap(":/images/libai01.jpg"));
         //下落速度
         gameSpeed = 600;
         boxGroup->stopTimer();
         boxGroup->startTimer(gameSpeed);
    }
    else
    {
        // 添加下一个级别的设置
        gameLevelText->setHtml(QString("<font color=yellow>Fourth"));
        scene()->setBackgroundBrush(QPixmap(":/images/Tetris_background_1.png"));
        gameSpeed = 500;
        boxGroup->stopTimer();
        boxGroup->startTimer(gameSpeed);
    }
}

// 游戏结束
void MainWindow::gameOver()
{
    pauseButton->hide();
    showMenuButton->hide();
    maskWidget->show();
    gameOverText->show();
    restartButton->setPos(370, 200);
    finishButton->show();
}

// 重新开始游戏
void MainWindow::restartGame()
{
    maskWidget->hide();
    gameOverText->hide();
    finishButton->hide();
    restartButton->setPos(600, 150);

    // 销毁提示方块组和当前方块移动区域中的所有小方块
    nextBoxGroup->clearBoxGroup(true);
    boxGroup->clearBoxGroup();
    boxGroup->hide();
    foreach (QGraphicsItem *item, scene()->items(QRectF(199, 49, 202, 402), Qt::ContainsItemShape))
    {
        // 先从场景中移除小方块，因为使用deleteLater()是在返回主事件循环后才销毁
        // 小方块的，为了在出现新的方块组时不发生碰撞，所以需要先从场景中移除小方块
        scene()->removeItem(item);
        OneBox *box = (OneBox*) item;
        box->deleteLater();
    }

    initGame();
}

// 结束当前游戏
void MainWindow::finishGame()
{
    gameOverText->hide();
    finishButton->hide();
    restartButton->setPos(600, 150);
    restartButton->hide();
    pauseButton->hide();
    showMenuButton->hide();
    gameScoreText->hide();
    gameLevelText->hide();

    topLine->hide();
    bottomLine->hide();
    leftLine->hide();
    rightLine->hide();

    nextBoxGroup->clearBoxGroup(true);
    boxGroup->clearBoxGroup();
    boxGroup->hide();

    foreach (QGraphicsItem *item, scene()->items(QRectF(199, 49, 202, 402), Qt::ContainsItemShape))
    {
        OneBox *box = (OneBox*) item;
        box->deleteLater();
    }

    // 可能是在进行游戏时按下“主菜单”按钮
    maskWidget->show();
    gameWelcomeText->show();
    startButton->show();
//    chartButton->show();
    helpButton->show();
    exitButton->show();
    scene()->setBackgroundBrush(QPixmap(":/images/libai22.jpg"));
}

// 暂停游戏
void MainWindow::pauseGame()
{
    boxGroup->stopTimer();
    restartButton->hide();
    pauseButton->hide();
    showMenuButton->hide();
    maskWidget->show();
    gamePausedText->show();
    returnButton->show();
}


// 返回游戏，处于暂停状态时
void MainWindow::returnGame()
{
    returnButton->hide();
    gamePausedText->hide();
    maskWidget->hide();
    restartButton->show();
    pauseButton->show();
    showMenuButton->show();
    boxGroup->startTimer(gameSpeed);
}

// 如果正在进行游戏，当键盘按下时总是方块组获得焦点
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (pauseButton->isVisible())
        boxGroup->setFocus();
    else
        boxGroup->clearFocus();
    QGraphicsView::keyPressEvent(event);
}


