/**
* \file palette.h
* Paleta barev reprezentujici stavy bunek automatu.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef PALETTE_H_INCLUDED
#define PALETTE_H_INCLUDED

#include <QtGui>
#include <QWidget>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "colorSampler.h"

/// Paleta barev reprezentujici stavy bunek automatu.
class Palette : public QWidget
{
    Q_OBJECT

    private:
    QRect colorBox;                        //< Obdelnik ohranicujici box s barvami
    int colorCellWidth;                    //< Sirka policka s barvou
    int colorCellHeight;                   //< Vyska policka s barvou
    int actualColorIndex;                  //< Index aktualni vybrane barvy
    std::vector<int> permittedCellValues;  //< Pripustne barvy reprezentujici stavy bunek

    protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *mouseEvent);

    public:
    /**
    * Konstruktor.
    */
    Palette(QWidget *parent = 0, int width = 0, int height = 0);

    /**
    * Nastavi barvy viditelne na palete.
    * @param cellPossibilities Viditelne barvy - pripustne stavy bunek.
    */
    void SetPalette(std::vector<int> &cellPossibilities);

    signals:
    /**
    * Signal zaslany po zmene aktualni barvy.
    * @param colorIndex Index aktualni barvy.
    */
    void ActualColorChanged(int colorIndex);
};

#endif // PALETTE_H_INCLUDED

