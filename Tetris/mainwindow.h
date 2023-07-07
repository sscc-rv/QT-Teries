#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QGraphicsView>

class BoxGroup;
class MainWindow : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *event);

public slots:
    void startGame();
    void clearFullRows();
    void moveBox();
    void gameOver();

    void restartGame();
    void finishGame();
    void pauseGame();
    void returnGame();
private:
    BoxGroup *boxGroup;
    BoxGroup *nextBoxGroup;
    //四条直线
    QGraphicsLineItem *topLine;
    QGraphicsLineItem *bottomLine;
    QGraphicsLineItem *leftLine;
    QGraphicsLineItem *rightLine;
    //下落速度
    qreal gameSpeed;
    QList<int> rows;//行
    void initView();
    void initGame();
    void updateScore(const int fullRowNum = 0);

    QGraphicsTextItem *gameScoreText;
    QGraphicsTextItem *gameLevelText;   

    // 遮罩面板
    QGraphicsWidget *maskWidget;

    // 按钮
    QGraphicsWidget *startButton;
    QGraphicsWidget *finishButton;
    QGraphicsWidget *restartButton;
    QGraphicsWidget *pauseButton;
    QGraphicsWidget *chartButton;
    QGraphicsWidget *returnButton;
    QGraphicsWidget *helpButton;
    QGraphicsWidget *exitButton;
    QGraphicsWidget *showMenuButton;

    // 文本
    QGraphicsTextItem *gameWelcomeText;
    QGraphicsTextItem *gamePausedText;
    QGraphicsTextItem *gameOverText;
};
#endif // MAINWINDOW_H
