#include "mybox.h"
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>

OneBox::OneBox(const QColor &color)
    : brushColor(color)
{
}
//重新实现两个纯虚公共函数
//限制边界
QRectF OneBox::boundingRect() const
{
    //画笔宽度为1
    qreal penWidth = 1;

    return QRectF(-10 - penWidth / 2, -10 - penWidth / 2,
                  20 + penWidth, 20 + penWidth);
}

void OneBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // 为小方块使用准备好的图片
    painter->drawPixmap(-10, -10, 20, 20, QPixmap(":/images/box.gif"));
    //使用画刷
    painter->setBrush(brushColor);
    QColor penColor = brushColor;
    // 将颜色的透明度减小
    penColor.setAlpha(220);
    painter->setPen(penColor);
    //绘制一个边长为20的小正方形
    painter->drawRect(-10, -10, 20, 20);
}

// 形状比边框矩形小0.5像素，这样方块组中的小方块才不会发生碰撞
QPainterPath OneBox::shape() const
{
    QPainterPath path;
    path.addRect(-9.5, -9.5, 19, 19);
    return path;
}

// 方块组类

BoxGroup::BoxGroup()
{
    //使图形项获得焦点
    setFlags(QGraphicsItem::ItemIsFocusable);
    // 保存变换矩阵，当BoxGroup进行旋转后，可以使用它来进行恢复
    oldTransform = transform();
    old_trans = transform();
    timer = new QTimer(this);//进行方块组自动下移
    connect(timer, SIGNAL(timeout()), this, SLOT(moveOneStep()));
    currentShape = RandomShape;

}
//容纳整个方块组
QRectF BoxGroup::boundingRect() const
{
    qreal penWidth = 1;
    return QRectF(-40 - penWidth / 2, -40 - penWidth / 2,
                  80 + penWidth, 80 + penWidth);
}
//用键盘事件实现方块的变化
// 处理方块组的键盘事件
void BoxGroup::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    //按下方向键下
    case Qt::Key_Down :
        moveBy(0, 20);             // 将方块组向下移动一格
        if (isColliding())         // 判断是否与其他方块碰撞
        {
            moveBy(0, -20);        // 如果发生碰撞，将方块组移回原位
            clearBoxGroup();       // 将方块组中的小方块从方块组中移除到场景中
            emit needNewBox();     // 发出信号，需要显示新的方块
        }
        break;
    //左方向键
    case Qt::Key_Left :
        moveBy(-20, 0);           // 将方块组向左移动一格
        if (isColliding())        // 判断是否与其他方块碰撞
        {
            moveBy(20, 0);        // 如果发生碰撞，将方块组移回原位
        }
        break;
    //右方向键
    case Qt::Key_Right :
        moveBy(20, 0);            // 将方块组向右移动一格
        if (isColliding())        // 判断是否与其他方块碰撞
        {
            moveBy(-20, 0);       // 如果发生碰撞，将方块组移回原位
        }
        break;
    //方向键上为变形
    case Qt::Key_Up :
        setTransform(old_trans);  // 将方块组恢复到原来的位置和角度
        //旋转90度
        new_trans = old_trans.rotate(90);  // 将方块组顺时针旋转90度

        if(isColliding())         // 判断是否与其他方块碰撞
        {
            setTransform(new_trans);  // 如果发生碰撞，将方块组恢复到原来的位置和角度
        }

        break;

    // 空格键实现坠落
    case Qt::Key_Space :
        moveBy(0, 20);           // 将方块组向下移动一格
        while (!isColliding())   // 判断是否与其他方块碰撞
        {
            moveBy(0, 20);       // 如果没有碰撞，将方块组继续向下移动一格
        }
        moveBy(0, -20);          // 如果发生碰撞，将方块组移回原位
        clearBoxGroup();         // 将方块组中的小方块从方块组中移除到场景中
        emit needNewBox();       // 发出信号，需要显示新的方块
        break;
    }
}

// 使用方块组中的每一个小方块来进行碰撞判断
bool BoxGroup::isColliding()
{
    QList<QGraphicsItem *> itemList = childItems();  // 获取方块组中的所有小方块
    QGraphicsItem *item;
    //遍历每一个小方块
    foreach (item, itemList)
    {
        if(item->collidingItems().count() > 1)  // 判断是否与其他方块碰撞
            return true;                        // 如果有碰撞，返回true
    }
    return false;                               // 没有碰撞，返回false
}

// 删除方块组中的所有小方块
void BoxGroup::clearBoxGroup(bool destroyBox)
{
    QList<QGraphicsItem *> itemList = childItems();  // 获取方块组中的所有小方块
    QGraphicsItem *item;
    foreach (item, itemList)                      // 遍历每一个小方块
    {
        removeFromGroup(item);                    // 从方块组中移除小方块到场景中
        if (destroyBox)                           // 如果需要销毁小方块
        {
            OneBox *box = (OneBox*) item;
            box->deleteLater();
        }
    }
}

// 创建方块
void BoxGroup::createBox(const QPointF &point, BoxShape shape)
{
    //七种不同的形状对应不同的颜色
    static const QColor colorTable[7] = {
                QColor(100, 0, 0, 100), QColor(25, 250, 0, 200),
                QColor(10, 0, 250, 150), QColor(10, 200, 10, 100),
                QColor(10, 200, 255, 100), QColor(200, 10, 255, 100),
                QColor(250, 100, 200, 100) };

    int shapeID = shape;

    if (shape == RandomShape)
    {
        // 产生0-6之间的随机数，对应不同的形状
        shapeID = qrand() % 7;
    }

    QColor color = colorTable[shapeID];

    QList<OneBox* > list;
    //恢复方块组的变换矩阵
    setTransform(oldTransform);
    for (int i = 0; i < 4; ++i)
    {
        OneBox *temp = new OneBox(color);
        list << temp;
        addToGroup(temp);
    }

    switch (shapeID) {
    case IShape :
        currentShape = IShape;
        list.at(0)->setPos(-30, -10);
        list.at(1)->setPos(-10, -10);
        list.at(2)->setPos(10, -10);
        list.at(3)->setPos(30, -10);
        break;

    case JShape :
        currentShape = JShape;
        list.at(0)->setPos(10, -10);
        list.at(1)->setPos(10, 10);
        list.at(2)->setPos(-10, 30);
        list.at(3)->setPos(10, 30);
        break;

    case LShape :
        currentShape = LShape;
        list.at(0)->setPos(-10, -10);
        list.at(1)->setPos(-10, 10);
        list.at(2)->setPos(-10, 30);
        list.at(3)->setPos(10, 30);
        break;

    case OShape :
        currentShape = OShape;
        list.at(0)->setPos(-10, -10);
        list.at(1)->setPos(10, -10);
        list.at(2)->setPos(-10, 10);
        list.at(3)->setPos(10, 10);
        break;

    case SShape :
        currentShape = SShape;
        list.at(0)->setPos(10, -10);
        list.at(1)->setPos(30, -10);
        list.at(2)->setPos(-10, 10);
        list.at(3)->setPos(10, 10);
        break;

    case TShape :
        currentShape = TShape;
        list.at(0)->setPos(-10, -10);
        list.at(1)->setPos(10, -10);
        list.at(2)->setPos(30, -10);
        list.at(3)->setPos(10, 10);
        break;

    case ZShape :
        currentShape = ZShape;
        list.at(0)->setPos(-10, -10);
        list.at(1)->setPos(10, -10);
        list.at(2)->setPos(10, 10);
        list.at(3)->setPos(30, 10);
        break;

    default : break;
    }
    // 设置位置
    setPos(point);
    // 如果开始就发生碰撞，说明已经结束游戏
    if (isColliding())
    {
        stopTimer();
        emit gameFinished();
    }
}

// 开启定时器
void BoxGroup::startTimer(int interval)
{
    timer->start(interval);
}

// 向下移动一步
void BoxGroup::moveOneStep()
{
    moveBy(0, 20);
    if (isColliding())
    {
        moveBy(0, -20);
        // 将小方块从方块组中移除到场景中
        clearBoxGroup();
        emit needNewBox();
    }
}

// 停止定时器
void BoxGroup::stopTimer()
{
    timer->stop();
}
