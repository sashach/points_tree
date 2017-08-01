#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pointstree.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    int drawingWidth, drawingHeight;
    QPoint selectedPoint;
    bool selectedPointSet;
    QLine selectedLine;

    QVector<QPoint> points;

    PointsTree pointsTree;
    bool usePointsTree;
    bool showPointsTree;

    enum
    {
        LEFT_OFFSET = 0,
        POINT_SIZE = 3,
        TOP_OFFSET = 0,
        BUTTON_WIDTH = 100
    };

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void resizeEvent(QResizeEvent*) override;

private:
    const QPoint &getClosestPoint(const QPoint &point);
    void selectPoint(QMouseEvent * event);
    void addPoint(const QPoint & point);
    void clearPoints();

public slots:
    void onGenerate();
    void onClear();
    void onUsePointsTree(bool checked);
    void onShowPointsTree(int state);
};

#endif // MAINWINDOW_H
