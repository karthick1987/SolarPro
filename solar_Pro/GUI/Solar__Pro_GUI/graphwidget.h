#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include <QGraphicsItem>

class Node;

class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
    QGraphicsScene *scene1;
    Node *centerNode;
    Node *n[8];
    int16_t x[8],y[8];
    uint8_t baseStation;
    const char nodeName[9] = "12345678";


    GraphWidget(QWidget *parent = nullptr);
    void itemMoved();
    void addNodes();

    bool getBsColourSet() const;
    void setBsColourSet(bool value);

    void addNode(int nodeNum);
    void *getNodePtr(int nodeIndex);

public slots:
    void shuffle();
    void zoomIn();
    void zoomOut();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif
    void drawBackground(QPainter *painter, const QRectF &rect) override;

    void scaleView(qreal scaleFactor);

private:
    int timerId;
    bool bsColourSet = false;
    int nodeCount;
};

#endif // GRAPHWIDGET_H
