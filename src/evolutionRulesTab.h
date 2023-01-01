/**
* \file evolutionRulesTab.h
* Zalozka s evoluci pravidel.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef EVOLUTION_RULES_TAB_H_INCLUDED
#define EVOLUTION_RULES_TAB_H_INCLUDED

#include <QtGui>
#include <QWidget>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "config.h"
#include "automaton.h"
#include "evolutionManager.h"
#include "gridWidget.h"
#include "evolutionPlot.h"

/// Zalozka s evoluci pravidel.
class EvolutionRulesTab : public QWidget
{
    Q_OBJECT

    private:
    QLabel *actualFitnessLabel;
    QLabel *actualCellFitnessLabel;
    QLabel *projectStatus;

    QPushButton *loadTaskButton;
    QPushButton *evoluteRulesButton;
    QPushButton *stopEvoluteRulesButton;
    QPushButton *abortEvoluteRulesButton;
    QPushButton *saveRulesButton;

    QComboBox *neighbourhoodComboBox;

    QSpinBox *automatonStepLine;
    QSpinBox *maxEvolutionStepLine;
    QDoubleSpinBox *mutationProbabilityLine;
    QSpinBox *damagedConfigurationCountLine;
    QDoubleSpinBox *maxDamagePercentageLine;
    QSpinBox *lockStepCountLine;
    QDoubleSpinBox *resetCellPercentageLimitLine;
    QDoubleSpinBox *resetGridPercentageLimitLine;
    QSpinBox *initialPhaseStepLine;

    QCheckBox *respectFitnessImprovementCheckBox;
    QCheckBox *extendedRuleCalculationCheckBox;
    QCheckBox *lockStepCountCheckBox;
    QCheckBox *resetCellPercentageLimitCheckBox;
    QCheckBox *resetGridPercentageLimitCheckBox;
    QCheckBox *initialPhaseStepCheckBox;

    QProgressBar *evoluteRulesBar;

    GridWidget *zygoteGridWidget;         //< Mrizka pro zobrazeni zygoty
    GridWidget *patternGridWidget;        //< Mrizka pro zobrazeni hledaneho vzoru
    EvolutionPlot *evolutionPlot;         //< Graf pro zobrazeni fitness automatu
    EvolutionManager evolutionManager;    //< Spravce procesu evoluce pravidel
    time_t timeSummary;                   //< Celkovy cas evoluce (zakladni pravidla + pravidla pro sebeopravu)
    std::vector<int> cellPossibilities;   //< Mozne stavy bunek automatu
    std::vector<double> basicFitness;     //< Prumerne fitness v jednotlivych krocich pri vyvoji zakladnich pravidel
    std::vector<double> averageFitness;   //< Prumerne fitness v jednotlivych krocich pri vyvoji pravidel pro sebeopravu
    Automaton::RuleSet basicRules;        //< Zakladni pravidla pro vyvoj ze zygoty do hledaneho vzoru
    Automaton::RuleSet generalRules;      //< Pravidla schopne sebeopravy
    Automaton::GridType zygote;           //< Tvar zygoty
    Automaton::GridType pattern;          //< Hledany vzor
    double basicRulesFitness;             //< Fitness zakladnich pravidel (pro vyvoj ze zygoty do hledaneho vzoru)
    double generalRulesFitness;           //< Fitness pravidel se schopnosti sebeopravy

    /**
    * Nastavi prislusne ovladaci prvky udaji z konfiguracniho souboru.
    */
    void SetConfigurationValues();

    /**
    * Nacte ulohu ze souboru.
    * @param inputFile Cesta k vstupniho souboru.
    * @return Udava, zda se podarilo nacist ulohu ze souboru.
    */
    bool LoadTask(std::string inputFile);

    /**
    * Ulozi pravidla do souboru.
    * @param Cesta k vystupnimu souboru.
    * @return Udava, zda se podarilo ulozit pravidla do souboru.
    */
    bool SaveRules(std::string outputFile);

    public:
    /**
    * Konstruktor.
    * @param parent Rodicovsky objekt.
    */
    EvolutionRulesTab(QWidget *parent = 0);

    protected:
    void paintEvent(QPaintEvent *);

    private slots:
    /**
    * Zjisti od uzivatele cestu k souboru s ulohou.
    */
    void SetOpenFileName();

    /**
    * Zjisti od uzivatele cestu k souboru, do ktereho budou ulozena vyvinuta pravidla.
    */
    void SetSaveRulesFile();

    /**
    * Spusti proces evoluce pravidel.
    */
    void StartEvoluteRules();

    /**
    * Pozastavi proces evoluce pravidel.
    */
    void StopEvoluteRules();

    /**
    * Zrusi proces evoluce pravidel.
    */
    void AbortEvoluteRules();

    /**
    * Zmena stavu zaskrtavaciho policka polozky "Zamykani bunek".
    * newState Novy stav zaskrtavaciho policka.
    */
    void CheckLockStepCount(int newState);

    /**
    * Zmena stavu zaskrtavaciho policka polozky "Reset bunky".
    * newState Novy stav zaskrtavaciho policka.
    */
    void CheckResetPercentageLimit(int newState);

    /**
    * Zmena stavu zaskrtavaciho policka polozky "Reset mrizky".
    * newState Novy stav zaskrtavaciho policka.
    */
    void CheckGridPercenatageLimit(int newState);

    /**
    * Zmena stavu zaskrtavaciho policka polozky "Delka inicializacni faze".
    * newState Novy stav zaskrtavaciho policka.
    */
    void CheckInitialPhase(int newState);

    /**
    * Prijme informaci o vyvoji zakladnich pravidel po provedeni evolucniho kroku.
    * @param averageFitness Prumerne fitness v aktualnim evolucnim kroku.
    */
    void ReceiveActualBasicRuleFitness(double averageFitness);

    /**
    * Prijme informaci o vyvoji pravidel schopnych opravy poskozeneho vzoru po provedeni evolucniho kroku.
    * @param actAverageFitness Prumerne fitness v aktualnim evolucnim kroku.
    * @param bestAverageFitness Nejlepsi fitness v aktualnim evolucnim kroku.
    */
    void ReceiveActualDamagedConfigurationRuleFitness(double actAverageFitness, double bestAverageFitness);

    /**
    * Prijme vygenerovana zakladni pravidela.
    * @param rules Pravidla automatu.
    * @param timeBasicRules Doba generovani zakladnich pravidel.
    * @param fitness Fitness vyvinutych pravidel.
    */
    void AcceptBasicRules(Automaton::RuleSet rules, int timeBasicRules, double fitness);

    /**
    * Prijmuti vygenerovanych pravidel schopnych opravy poskozeneho vzoru.
    * @param rules Pravidla automatu.
    * @param timeSummary Celkova doba generovani pravidel.
    * @param fitness Fitness vyvinutych pravidel.
    */
    void AcceptGeneralRules(Automaton::RuleSet rules, int timeSummary, double fitness);
};

#endif // EVOLUTION_RULES_TAB_H_INCLUDED
