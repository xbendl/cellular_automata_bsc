/**
* \file evolutionPlot.h
* Graf vyvoje fitness
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef EVOLUTION_PLOT
#define EVOLUTION_PLOT

#include <stdlib.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include "evolutionManager.h"

/// Graf vyvoje fitness
class EvolutionPlot : public QwtPlot
{
    Q_OBJECT

    public:

    /**
    * Konstruktor.
    */
    EvolutionPlot(QWidget *parent, EvolutionManager *evolutionManager);

    /**
    * Spusti casovac.
    */
    void StartTimer();

    /**
    * Vypne casovac.
    * @param immidiatelyReaction Urci rychlost reakce (zda ma dobehnout aktualni krok).
    */
    void KillTimer(bool immidiatelyReaction);

    /**
    * Vynuluje nastaveni casovace.
    */
    void ResetTimer();

    /**
    * Zjisti stav casovace.
    * @return Stav casovace.
    */
    bool IsTimerRunning();

    protected:
    void timerEvent(QTimerEvent *timerEvent);

    private:
    int helping;
    bool timerRunning;
    int valueCount;     // Pocet doposud uchovanych hodnot
    int maxValueCount;  // Maximalni pocet uchovavanych hodnot
    int timerInterval;  // Interval tiku casovace
    int timerId;        // ID casovace
    double actBestFitness;     // Aktualni nejlepsi fitness
    double actAverageFitness;  // Aktualni prumerne fitness
    double actEvolutionStep;   // Aktualni evolucni krok
    std::vector<double> bestFitness;     // Hodnoty pro krivku zobrazujici nejlepsi fitness
    std::vector<double> averageFitness;  // Hodnoty pro krivku zobrazujici prumerny fitness
    std::vector<int> realTimeData;       // Cisla evolucnich kroku zobrazovane na ose X
    QwtPlotCurve *bestCurve;             // Ukazatel na krivku zobrazujici nejlepsi fitness
    QwtPlotCurve *averageCurve;          // Ukazatel na krivku zobrazujici prumerny fitness

    private slots:
    /**
    * Prijme informaci o vyvoji zakladnich pravidel po provedeni evolucniho kroku.
    * @param averageFitness Prumerne fitness v aktualnim evolucnim kroku.
    */
    void ReceiveActualBasicRuleFitness(double averageFitness);

    /**
    * Prijme informaci o vyvoji pravidel schopnych opravy poskozeneho vzoru po provedeni evolucniho kroku.
    * @param averageFitness Prumerne fitness v aktualnim evolucnim kroku.
    * @param bestFitness Nejlepsi fitness v aktualnim evolucnim kroku.
    */
    void ReceiveActualDamagedConfigurationRuleFitness(double averageFitness, double bestFitness);
};

#endif // EVOLUTION_PLOT
