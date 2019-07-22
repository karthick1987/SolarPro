#include "graphwidget.h"
#include "edge.h"
#include "node.h"
#include "mainwindow.h"

#include <stdlib.h>     /* srand, rand */
#include <math.h>
#include <QKeyEvent>

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), timerId(0)
{
    scene1 = new QGraphicsScene(this);
    scene1->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene1->setSceneRect(-240, -255, 490, 520);
    setScene(scene1);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setWindowTitle(tr(""));

/*
    scene->addItem(new Edge(centerNode, node1));
    scene->addItem(new Edge(centerNode, node2));
    scene->addItem(new Edge(node2, node3));
    scene->addItem(new Edge(centerNode, node4));
    scene->addItem(new Edge(node4, node5));
    scene->addItem(new Edge(node6, node7));
    scene->addItem(new Edge(centerNode, node6));
    scene->addItem(new Edge(node7, node8));
*/

    //scene->addItem(new Edge(node6, node7));
    //scene->addItem(new Edge(node6, node8));
    //scene->addItem(new Edge(node7, node9));
    //scene->addItem(new Edge(node9, node8));


}

void GraphWidget::addNodes()
{
    //char c[] = "1234567";
    for (int i=0;i<7;i++)
    {
        n[i] = new Node(this,'1'+i);
        scene1->addItem(n[i]);
    }
    centerNode = new Node(this,'0');

    /*
    //QGraphicsScene *scene1 = new QGraphicsScene(this);
    n1 = new Node(this,'1');
    n2 = new Node(this,'2');
    n3 = new Node(this,'3');
    n4 = new Node(this,'4');
    n5 = new Node(this,'5');
    n6 = new Node(this,'6');
    n7 = new Node(this,'7');
    */

    scene1->addItem(centerNode);


    n[0]->setPos(0, -50);
    n[1]->setPos(-50, 0);
    n[2]->setPos(-100, 50);
    n[3]->setPos(50, 50);
    centerNode->setPos(0, 0);
    n[4]->setPos(100, 100);
    n[5]->setPos(50, 0);
    n[6]->setPos(100, -50);
}

void GraphWidget::itemMoved()
{
    if(!timerId)
        timerId = startTimer(1000/25);
}


void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_Up:
        centerNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        centerNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        centerNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        centerNode->moveBy(20, 0);
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        shuffle();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}


void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    QList<Node *> nodes;
    foreach(QGraphicsItem *item, scene()->items()){
        if(Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    foreach(Node *node, nodes)
        node->calculateForces();

    bool itemsMoved = false;
    foreach(Node *node, nodes){
        if(node->advancePosition())
            itemsMoved = true;
    }

    if(!itemsMoved){
        killTimer(timerId);
        timerId = 0;
    }
}

#if QT_CONFIG(wheelevent)

void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}

#endif

void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    //shadow
    QRectF sceneRect = this->sceneRect();
    /*QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);

    if(rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if(bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);*/

    //fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    //text
    /*QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4, sceneRect.width() - 4, sceneRect.height() - 4);
    QString message(tr("Click and drag the nodes around, and zoom with the mouse wheel or the '+' and '-' keys"));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);*/
}


void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if(factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}



void GraphWidget::shuffle()
{
    foreach(QGraphicsItem *item, scene()->items()){
        if(qgraphicsitem_cast<Node *>(item))
            item->setPos(-150 + (rand()%301), -150 + (rand()%301));
    }
}

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}


void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}





