#include "pointstree.h"
#include <QPainter>
#include <iostream>

PointsTree::PointsTree(const int _level)
    : parent(nullptr), level(_level)
{

}

PointsTree::PointsTree(PointsTree * _parent, const int _level, const QRect &_bounds)
    : parent(_parent),
      level(_level),
      bounds(_bounds),
      visited(false)
{
    points.reserve(MAX_POINTS);
    nodes.reserve(4);
}

void PointsTree::clear()
{
    points.clear();

    for(auto & it: nodes)
    {
        it.clear();
    }
    nodes.clear();
}

void PointsTree::clearVisited()
{
    visited = false;
    for(auto & it: nodes)
    {
        it.clearVisited();
    }
}


void PointsTree::setBounds(const QRect & _bounds)
{
    bounds = _bounds;
}

void PointsTree::createNodes()
{
    int childWidth = bounds.width() / 2;
    int childHeight = bounds.height() / 2;
    int childLevel = level + 1;
    int midX = bounds.x() + childWidth;
    int midY = bounds.y() + childHeight;

    PointsTree nodeTopLeft(this, childLevel, QRect(bounds.x(), bounds.y(), childWidth, childHeight));
    nodes.push_back(nodeTopLeft);

    PointsTree nodeTopRight(this, childLevel, QRect(midX, bounds.y(), childWidth, childHeight));
    nodes.push_back(nodeTopRight);

    PointsTree nodeBottomLeft(this, childLevel, QRect(bounds.x(), midY, childWidth, childHeight));
    nodes.push_back(nodeBottomLeft);

    PointsTree nodeBottomRight(this, childLevel, QRect(midX, midY, childWidth, childHeight));
    nodes.push_back(nodeBottomRight);

    for(const auto & itPoint: points)
    {
        addPointToNodes(itPoint);
    }
    points.clear();
}

void PointsTree::addPointToNodes(const QPoint & point)
{
    for(auto & itNode: nodes)
    {
        itNode.add(point);
    }
}

bool PointsTree::isPointInsideNode(const QPoint & point) const
{
    return (point.x() >= bounds.x() && point.y() >= bounds.y()
    && point.x() < (bounds.x() + bounds.width())
    && point.y() < (bounds.y() + bounds.height()));
}

void PointsTree::add(const QPoint & point)
{
    if(isPointInsideNode(point))
    {
        if(nodes.empty())
        {
            if(points.size() < MAX_POINTS)
            {
                points.push_back(point);
            }
            else
            {
                createNodes();

                addPointToNodes(point);
            }
        }
        else
        {
            addPointToNodes(point);
        }
    }
}

void PointsTree::draw(QPainter & painter, const bool showBounds) const
{
    if(showBounds)
    {
        painter.fillRect(bounds, QColor(200, 200, 200));
        if(visited)
        {
            painter.fillRect(bounds, QColor(190, 190, 190));
        }

        painter.setPen(QPen(QColor(70, 240, 240)));
        painter.drawRect(bounds);
    }

    painter.setPen(QPen(QColor(70, 70, 240), POINT_SIZE));
    painter.drawPoints(points.data(), points.size());

    for(const auto & it: nodes)
    {
        it.draw(painter, showBounds);
    }
}

double PointsTree::getDistance(const QPoint & p1, const QPoint & p2)
{
    double dX = p1.x() - p2.x();
    double dY = p1.y() - p2.y();
    return sqrt(dX*dX + dY*dY);
}

double PointsTree::findClosestPointFromVector(const QPoint & point, int & resIndex, const QVector<QPoint> &points)
{
    double distance = std::numeric_limits<double>::max();
    resIndex = -1;
    for(int i = 0; i < points.size(); ++i)
    {
        double curDistance = getDistance(points[i], point);

        if(curDistance < distance)
        {
            distance = curDistance;
            resIndex = i;
        }
    }

    return distance;
}

const QPoint & PointsTree::getClosestFromNode(const QPoint & point, const QRect & searchBounds,
                                              const QPoint & bestPoint,
                                              double & distance, const QPoint * resPoint)
{
    auto checkNode = [&](PointsTree & node)
    {
        if(!node.visited && bounds.intersects(searchBounds) && !node.points.empty())
        {
            if(distance == std::numeric_limits<double>::max() ||
                    (isPointInsideNode(QPoint(point.x() + distance, point.y()))
                    || isPointInsideNode(QPoint(point.x() - distance, point.y()))
                    || isPointInsideNode(QPoint(point.x(), point.y() + distance))
                    || isPointInsideNode(QPoint(point.x(), point.y() - distance)))
                    )
            {
                node.visited = true;
                int resIndex = -1;
                double resDistance = findClosestPointFromVector(point, resIndex, node.points);
                if(resDistance < distance && resIndex >= 0 && resIndex < node.points.size())
                {
                    distance = resDistance;
                    resPoint = &node.points[resIndex];
                }
            }
        }
        return nullptr;
    };

    for(auto & it: nodes)
    {
        if(it.nodes.empty())
        {
            checkNode(it);
        }
        else
        {
            resPoint = &(it.getClosestFromNode(point, searchBounds, bestPoint, distance, resPoint));
        }
    }

    if(resPoint)
    {
        return *resPoint;
    }
    else
    {
        return bestPoint;
    }
}

const QPoint & PointsTree::getClosestPoint(const QPoint & point, QRect & bestPointBounds)
{
    auto setSearchBoundsForThisNode = [&, this]()
    {
        bestPointBounds.setX(bounds.x() - SEARCH_RECT_INCREASE);
        bestPointBounds.setY(bounds.y() - SEARCH_RECT_INCREASE);
        bestPointBounds.setWidth(bounds.width() + 2 * SEARCH_RECT_INCREASE);
        bestPointBounds.setHeight(bounds.height() + 2 * SEARCH_RECT_INCREASE);
    };

    if(!visited && isPointInsideNode(point))
    {
        double distance = std::numeric_limits<double>::max();
        if(points.size())
        {
            visited = true;
            int resIndex = -1;
            distance = findClosestPointFromVector(point, resIndex, points);
            if(resIndex >= 0 && resIndex < points.size())
            {
                setSearchBoundsForThisNode();
                return parent->getClosestFromNode(point, bestPointBounds, points[resIndex], distance, nullptr);
            }
            else
            {
                return point;
            }
        }
        else if(!nodes.empty())
        {
            for(auto & it: nodes)
            {
                const QPoint & result = it.getClosestPoint(point, bestPointBounds);
                if(result != point)
                {
                    if(parent)
                    {
                        double distance = getDistance(point, result);
                        return parent->getClosestFromNode(point, bestPointBounds, result, distance, nullptr);
                    }
                    else
                    {
                        return result;
                    }
                }
            }
        }
        else
        {
            visited = true;
            setSearchBoundsForThisNode();
            return parent->getClosestFromNode(point, bestPointBounds, point, distance, nullptr);
        }
    }

    return point;
}

