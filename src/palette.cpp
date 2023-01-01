#include "palette.h"

using namespace std;

Palette::Palette(QWidget *parent, int width, int height) : QWidget(parent)
{
    actualColorIndex = 0;
    colorCellHeight = 20;

    setFixedSize(width, height);
    colorCellWidth = floor((width - 1) / ((ColorSampler::sampler).size() + 2));   // Sirka policka s barvou

    colorBox.moveTo(0, 0);                                                // Obdelnik s vyberem barev
    colorBox.setSize(QSize(((ColorSampler::sampler).size() * colorCellWidth), colorCellHeight));
}

void Palette::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    // Vykresleni boxu pro vyber barev
    painter.setPen(Qt::transparent);

    if(permittedCellValues.empty())
    {   // Pokud neni mnozina pripustnych hodnot bunek nastavena, zobrazi se cela paleta
        for(unsigned int colorIndex = 0; colorIndex < (ColorSampler::sampler).size(); colorIndex++)
        {
            painter.setBrush(ColorSampler::sampler[colorIndex]);
            painter.drawRect((colorIndex * colorCellWidth), 0, colorCellWidth, colorCellHeight);
        }
    }
    else
    {   // Zobrazi se jenom mnozina pripustnych hodnot bunek
        for(unsigned int colorIndex = 0; colorIndex < permittedCellValues.size(); colorIndex++)
        {
            painter.setBrush(ColorSampler::sampler[permittedCellValues[colorIndex]]);
            painter.drawRect((colorIndex * colorCellWidth), 0, colorCellWidth, colorCellHeight);
        }
    }

    // Vykresleni mrizky okolo boxu pro vyber barev
    painter.setPen(Qt::black);
    painter.setBrush(Qt::transparent);
    painter.drawRect(colorBox);

    // Vykresleni aktualni barvy
    painter.setPen(Qt::black);
    painter.setBrush(ColorSampler::sampler[actualColorIndex]);
    painter.drawRect((this->width() - colorCellWidth - 1), 0, colorCellWidth, colorCellHeight);
}

void Palette::mousePressEvent(QMouseEvent *mouseEvent)
{
    if((mouseEvent->x() > colorBox.x()) && (mouseEvent->y() > colorBox.y()) && (mouseEvent->x() < (colorBox.x() + colorBox.width())) && (mouseEvent->y() < (colorBox.y() + colorBox.height())))
    {   // Mrizka byla inicializovana
        actualColorIndex = mouseEvent->x() / colorCellWidth;

        if(permittedCellValues.size() > 0)
        {
            actualColorIndex = permittedCellValues[actualColorIndex];
        }

        emit ActualColorChanged(actualColorIndex);   // Signal informujici o zmene aktualni barvy
        update();
    }
}

void Palette::SetPalette(std::vector<int> &cellPossibilities)
{
    permittedCellValues = cellPossibilities;   // Nastaveni pripustnych stavu bunek
    colorBox.setSize(QSize((permittedCellValues.size() * colorCellWidth), colorCellHeight));   // Nastaveni nove sirky ramecku
    update();
}
