#ifndef MYBOX_H
#define MYBOX_H
#include <QGraphicsObject>
#include <QGraphicsItemGroup>
#include <QTransform>

// 小方块类
class OneBox : public QGraphicsObject
{
public:
    OneBox(const QColor &color = Qt::yellow);
    //使用自定义的图形
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    //绘制形状
    QPainterPath shape() const;

private:
    QColor brushColor;

};

// 设计方块组类
class BoxGroup : public QObject, public QGraphicsItemGroup
{
    //先进行宏定义
    Q_OBJECT

public:
    //定义好枚举类型
    enum BoxShape {IShape, JShape, LShape, OShape, SShape,TShape, ZShape, RandomShape};
    BoxGroup();
    QRectF boundingRect() const;

//碰撞检测函数
    bool isColliding();
    void createBox(const QPointF &point = QPointF(0, 0), BoxShape shape = RandomShape);//浮点型
    void clearBoxGroup(bool destroyBox = false);
    BoxShape getCurrentShape()
    {
        return currentShape;
    }

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void needNewBox();
    void gameFinished();

public slots:
    void moveOneStep();
    void startTimer(int interval);
    void stopTimer();

private:
    BoxShape currentShape;
    QTransform oldTransform;
    QTransform new_trans;
    QTransform old_trans;
    QTimer *timer;
};
#endif // MYBOX_H
