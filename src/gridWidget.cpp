#include <QtGui>
#include <math.h>
#include <iostream>
#include "gridWidget.h"

GridWidget::GridWidget(QWidget *parent, int maxSize, QWidget *colorSender) : QWidget(parent)
{
    this->maxSize = maxSize;
    this->editableFlag = false;
    this->actualColorIndex = 0;
    this->cellWidth = 0;
    this->mousePressed = false;

    // Globalni nastaveni
    setAutoFillBackground(true);
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);

    if(colorSender != NULL)
    {   // Signal o zmene barvy ma smysl prijimat pouze v pripade, ze je mrizka editovatelna
        connect(colorSender, SIGNAL(ActualColorChanged(int)), this, SLOT(ActualColorChanged(int)));
        this->editableFlag = true;
    }
}

void GridWidget::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    // Vykresleni mrizky
    int posX = 0, posY = 0;
    QColor lineColor(20, 20, 20);  // Odstin pro vykresleni delicich car mezi bunkami
    if(grid.size() > 0)
    {   // Mrizka byla inicializovana

        // Vymazani pozadi
        int length = grid.size() * cellWidth;                       // Celkova delka stany automatu v px
        painter.setPen(Qt::transparent);
        painter.setBrush(Qt::transparent);
        painter.drawRect(0, 0, (this->width() - 1), (this->height() - 1));

        // Vykresleni delicich car
        painter.setPen(lineColor);
        painter.setBrush(Qt::white);
        for(unsigned int i = 0; i < grid.size() + 1; i++)
        {
            painter.drawLine(QPointF(0, posY), QPointF(length, posY));
            painter.drawLine(QPointF(posX, 0), QPointF(posX, length));
            posX += cellWidth;
            posY += cellWidth;
        }

        // Vykresleni jednotlivych policek
        for(unsigned int row = 0; row < grid.size(); row++)
        {
            for(unsigned int col = 0; col < grid.size(); col++)
            {
                painter.setBrush(ColorSampler::sampler[grid[row][col]]);
                painter.drawRect(col * cellWidth, row * cellWidth, cellWidth, cellWidth);
            }
        }
    }
    else
    {   // Vykresleni prazdne plochy (po startu programu)
        painter.setPen(Qt::black);
        painter.setBrush(Qt::white);
        painter.drawRect(0, 0, (this->width() - 1), (this->width() - 1));
    }

    // Vykresleni obdelniku okolo policka, nad kterym pohybujeme mysi
    if((grid.size() > 0) && underMouse())
    {   // Kurzor je nad oblasti mrizky

        // Vypocet ohranicujiciho obdelniku
        QRect activeCell = QRect(((actMousePos.x()-1) / cellWidth) * cellWidth, ((actMousePos.y()-1) / cellWidth) * cellWidth, cellWidth, cellWidth);

        // Vykresleni ohranicujiciho obdelniku
        QPen pen(Qt::magenta, 2);
        painter.setPen(pen);
        painter.setBrush(Qt::transparent);
        painter.drawRect(activeCell);
    }
}

void GridWidget::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    actMousePos.setX(mouseEvent->x());
    actMousePos.setY(mouseEvent->y());

    if(editableFlag && mousePressed && (mouseEvent->x() > 0) && (mouseEvent->x() < this->width()) && (mouseEvent->y() > 0) && (mouseEvent->y() < this->height()))
    {   // Drzi-li uzivatel leve tlacitko mysi, pak je bunka vyplnena aktualni vybranou barvou
        grid[(mouseEvent->y() - 2) / cellWidth][(mouseEvent->x() - 2) / cellWidth] = actualColorIndex;
    }

    update();
}

void GridWidget::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

void GridWidget::mousePressEvent(QMouseEvent *mouseEvent)
{
    mousePressed = true;

    if((grid.size() > 0) && editableFlag)
    {   // Mrizka byla inicializovana
        grid[(mouseEvent->y() - 1) / cellWidth][(mouseEvent->x() - 1) / cellWidth] = actualColorIndex;
        update();
    }
}

void GridWidget::SetEmptyGrid(int size)
{
    // Vynulovani mrizky
    Automaton::CellVector emptyCellVector;
    emptyCellVector.assign(size, (Automaton::CellType)0);
    grid.assign(size, emptyCellVector);

    // Nastaveni sirky mrizky
    int newLength = grid.size() * ((maxSize - 1) / grid.size()) + 1;
    setFixedSize(newLength, newLength);

    // Nastaveni sirky bunky
    this->cellWidth = floor((this->width() - 1) / grid.size());

    update();                             // Update vede k vykresleni aktualni - vynulovane - mrizky
}

void GridWidget::SetGrid(Automaton::GridType &pattern)
{
    this->grid = pattern;
    int newLength = grid.size() * ((maxSize - 1) / grid.size()) + 1;
    setFixedSize(newLength, newLength);

    // Nastaveni sirky bunky
    this->cellWidth = floor((this->width() - 1) / grid.size());

    update();
}

void GridWidget::ActualColorChanged(int colorIndex)
{
    actualColorIndex = colorIndex;
}

Automaton::GridType GridWidget::GetGrid()
{
    return grid;
}
