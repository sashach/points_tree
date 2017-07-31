#ifndef POINTSTREE_H
#define POINTSTREE_H

#include <QVector>
#include <QPoint>
#include <QRect>

class QPainter;

class PointsTree
{
public:
    PointsTree(const int _level = 0);
    PointsTree(PointsTree *_parent, const int _level, const QRect &_bounds);

    void clear();
    void setBounds(const QRect & _bounds);
    void add(const QPoint & point);
    void draw(QPainter & painter, const bool showBounds = false) const;

    const QPoint &getClosestPoint(const QPoint & point, QRect &bestPointBounds);

    static double findClosestPointFromVector(const QPoint & point, int &resIndex, const QVector<QPoint> & points);

    void clearVisited();

private:
    enum
    {
        POINT_SIZE = 3,
        SEARCH_RECT_INCREASE = 5,
        MAX_POINTS = 4
    };

    PointsTree * parent;
    QVector<QPoint> points;
    QVector<PointsTree> nodes;
    int level;
    QRect bounds;
    bool visited;

    void createNodes();
    void addPointToNodes(const QPoint & point);
    bool isPointInsideNode(const QPoint & point) const;

    const QPoint & getClosestFromNode(const QPoint & point,
                                      const QRect & searchBounds, const QPoint &bestPoint,
                                      double & distance, const QPoint *resPoint);

    static double getDistance(const QPoint & p1, const QPoint & p2);
};

#endif // POINTSTREE_H
