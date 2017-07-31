#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMoveEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    drawingWidth(0),
    drawingHeight(0),
    selectedPointSet(false),
    pointsTree(0),
    usePointsTree(true),
    showPointsTree(true)
{
    ui->setupUi(this);

    connect(ui->generateButton, QPushButton::pressed, this, onGenerate);
    connect(ui->clearButton, QPushButton::pressed, this, onClear);
    connect(ui->usePointsTreeCheckBox, QCheckBox::stateChanged, this, onUsePointsTree);
    connect(ui->showPointsTreeCheckBox, QCheckBox::stateChanged, this, onShowPointsTree);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onGenerate()
{
    clearPoints();

    for(int i = 0; i < ui->pointsCountSpinBox->value(); ++i)
    {
        QPoint point(LEFT_OFFSET + rand() % (drawingWidth - LEFT_OFFSET),
                     TOP_OFFSET + rand() % (drawingHeight - TOP_OFFSET));
        addPoint(point);
    }

    selectedPointSet = false;
    update();
}

void MainWindow::onClear()
{
    clearPoints();
    update();
}

void MainWindow::onUsePointsTree(int state)
{
    usePointsTree = (state == Qt::Checked);
}

void MainWindow::onShowPointsTree(int state)
{
    showPointsTree = (state == Qt::Checked);
    update();
}

void MainWindow::clearPoints()
{
    if(usePointsTree)
    {
        pointsTree.clear();
    }
    else
    {
        points.clear();
    }
}

void MainWindow::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    QRect base(LEFT_OFFSET, TOP_OFFSET, drawingWidth, drawingHeight);
    painter.fillRect(base, QBrush(QColor(200, 200, 200)));

    if(usePointsTree)
    {
        pointsTree.draw(painter, showPointsTree);
    }
    else
    {
        painter.setPen(QPen(QColor(240, 70, 70), POINT_SIZE));
        painter.drawPoints(points.data(), points.size());
    }

    if(selectedPointSet)
    {
        painter.setPen(QPen(QColor(240, 240, 70), POINT_SIZE));
        painter.drawPoint(selectedPoint);

        painter.setPen(QPen(QColor(240, 240, 70), 1));
        painter.drawLine(selectedLine);
    }
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    clearPoints();

    drawingWidth = width() - BUTTON_WIDTH;
    drawingHeight = height();

    pointsTree.clear();
    pointsTree.setBounds(QRect(LEFT_OFFSET, TOP_OFFSET, drawingWidth, drawingHeight));

    ui->buttonsFrame->move(drawingWidth, 0);
}


const QPoint & MainWindow::getClosestPoint(const QPoint & point)
{
    if(usePointsTree)
    {
        pointsTree.clearVisited();
        QRect startBounds(0, 0, 0, 0);
        return pointsTree.getClosestPoint(point, startBounds);
    }
    else
    {
        if(points.empty())
        {
            return point;
        }

        int resIndex = -1;
        PointsTree::findClosestPointFromVector(point, resIndex, points);

        return points[resIndex];
    }
}

void MainWindow::selectPoint(QMouseEvent *event)
{
    QPoint mousePos(event->x(), event->y());

    selectedPoint = mousePos;
    const QPoint & closest = getClosestPoint(mousePos);

    selectedLine = QLine(closest.x(), closest.y(), selectedPoint.x(), selectedPoint.y());

    selectedPointSet = true;
    update();
}

void MainWindow::addPoint(const QPoint & point)
{
    if(usePointsTree)
    {
        pointsTree.add(point);
    }
    else
    {
        points.push_back(point);
    }
    update();
}

void MainWindow::mousePressEvent(QMouseEvent * event)
{
    if(ui->addPointsCheckBox->checkState())
    {
        addPoint(QPoint(event->x(), event->y()));
    }
    else
    {
        selectPoint(event);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent * event)
{
    if(!ui->addPointsCheckBox->checkState())
    {
        selectPoint(event);
    }
}
