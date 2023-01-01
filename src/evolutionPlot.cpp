#include "evolutionPlot.h"

class ConfigScaleDraw : public QwtScaleDraw
{
    public:
    virtual QwtText label(double) const
    {
        QString output = "";
        return output;
    }
};

EvolutionPlot::EvolutionPlot(QWidget *parent, EvolutionManager *evolutionManager) : QwtPlot(parent)
{
    setAutoReplot(false);
    plotLayout()->setAlignCanvasToScales(true);
    setCanvasLineWidth(1);

    // Nastaveni vychozich hodnot
    actBestFitness = 0;
    actAverageFitness = 0;
    actEvolutionStep = 0;
    timerInterval = Config::GetInstance()->timeIntervalEvolutionRules;
    timerId = -1;
    timerRunning = false;
    valueCount = 1;
    bestFitness.push_back(0);
    averageFitness.push_back(1);

    realTimeData.assign(Config::GetInstance()->evolutionGraphValuesCount, 0);

    // Nastaveni legendy
    QwtLegend *legend = new QwtLegend(this);
    insertLegend(legend, QwtPlot::ExternalLegend);
    legend->setGeometry(58, 276, 120, 60);
    setCanvasBackground(Qt::white);

    // Nastaveni osy X
    QwtText helpVariable("Cas");
    helpVariable.setFont(QFont("Helvetica", 9, QFont::Bold));
    setAxisTitle(xBottom, helpVariable);
    setAxisScaleDraw(QwtPlot::xBottom, new ConfigScaleDraw());
    setAxisScale(QwtPlot::xBottom, 0, (Config::GetInstance()->evolutionGraphValuesCount - 1));

    // Nastaveni osy Y
    helpVariable.setText("Fitness");
    helpVariable.setFont(QFont("Helvetica", 10, QFont::Bold));
    setAxisTitle(yLeft, helpVariable);
    setAxisScale(QwtPlot::yLeft, 0, 100);

    // Nastaveni krivek
    QwtPlotCurve *curve;
    curve = new QwtPlotCurve("Nejlepsi fitness");
    curve->setPen(QPen(Qt::red));
    curve->attach(this);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve->setBrush(QBrush(Qt::red));
    bestCurve = curve;
    curve = new QwtPlotCurve("Prumerne fitness");
    curve->setPen(QPen(Qt::black));
    curve->setZ(curve->z()+1);
    curve->attach(this);
    averageCurve = curve;
    curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve->setBrush(QBrush(Qt::black));

    connect(evolutionManager, SIGNAL(ActualBasicRuleFitness(double)), this, SLOT(ReceiveActualBasicRuleFitness(double)));
    connect(evolutionManager, SIGNAL(ActualDamagedConfigurationRuleFitness(double, double)), this, SLOT(ReceiveActualDamagedConfigurationRuleFitness(double, double)));
}

void EvolutionPlot::timerEvent(QTimerEvent *)
{
    if(valueCount > Config::GetInstance()->evolutionGraphValuesCount)
    {   // Prvni hodnota se vyhodi
        bestFitness.erase(bestFitness.begin());
        averageFitness.erase(averageFitness.begin());
    }
    else
    {
        valueCount++;
    }

    bestFitness.push_back(actBestFitness * 100);       // Na konec se zaradi aktualni hodnota
    averageFitness.push_back(actAverageFitness * 100);

    double bestFitnessArray[Config::GetInstance()->evolutionGraphValuesCount];     // Prevod na pole
    double averageFitnessArray[Config::GetInstance()->evolutionGraphValuesCount];
    double timeData[Config::GetInstance()->evolutionGraphValuesCount];
    for(unsigned int i = 0; i < bestFitness.size(); i++)
    {
        bestFitnessArray[i] = bestFitness[i];
        averageFitnessArray[i] = averageFitness[i];
        timeData[i] = (Config::GetInstance()->evolutionGraphValuesCount - valueCount) + i;
    }

    bestCurve->setRawData(timeData, bestFitnessArray, valueCount);
    averageCurve->setRawData(timeData, averageFitnessArray, valueCount);

    replot();

    if(!timerRunning)
        killTimer(timerId);
}

void EvolutionPlot::StartTimer()
{
    if(IsTimerRunning())
    {   // Kontrola, zda-li jiz casovac nebezi
        killTimer(timerId);
        timerRunning = false;
    }

    timerRunning = true;
    timerId = startTimer(timerInterval);
}

void EvolutionPlot::KillTimer(bool immidiatelyReaction)
{
    if(IsTimerRunning())
    {
        timerRunning = false;

        if(immidiatelyReaction)
        {   // V pripade pozadovane nezpozdene reakce je timer vypnut ihned (nebude tak zobrazen posledni mezivysledek)
            killTimer(timerId);
        }
    }
}

bool EvolutionPlot::IsTimerRunning()
{
    return timerRunning;
}

void EvolutionPlot::ResetTimer()
{
    valueCount = 1;
    actEvolutionStep = 0;
    bestFitness.clear();
    bestFitness.push_back(0);
    averageFitness.clear();
    averageFitness.push_back(1);
    replot();
}

void EvolutionPlot::ReceiveActualDamagedConfigurationRuleFitness(double actAverageFitness, double actBestFitness)
{
    this->actAverageFitness = actAverageFitness;
    this->actBestFitness = actBestFitness;
    this->actEvolutionStep++;
}

void EvolutionPlot::ReceiveActualBasicRuleFitness(double actAverageFitness)
{
    this->actAverageFitness = actAverageFitness;
    this->actBestFitness = actAverageFitness;
    this->actEvolutionStep++;
}

