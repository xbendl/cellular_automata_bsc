/**
* \file analyseRulesTab.h
* Zalozka s analyzou pravidel
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef ANALYSE_RULES_TAB_H_INCLUDED
#define ANALYSE_RULES_TAB_H_INCLUDED

#include <QtGui>
#include <QWidget>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "config.h"
#include "automaton.h"
#include "evolutionPlot.h"
#include "analyseManager.h"
#include "gridWidget.h"
#include "palette.h"

/// Zalozka s analyzou pravidel.
class AnalyseRulesTab : public QWidget
{
    Q_OBJECT

    private:
    QLabel *basicRulesGridHammingDistanceLabel;
    QLabel *generalRulesGridHammingDistanceLabel;
    QLabel *actualStepLabel;

    QPushButton *automatonStepButton;
    QPushButton *loadRulesButton;
    QPushButton *steppingButton;
    QPushButton *saveResultsButton;
    QPushButton *resetButton;

    QSpinBox *automatonStepLine;
    QSpinBox *steppingLine;
    QDoubleSpinBox *autoDamageLine;

    QCheckBox *damageCheckBox;

    Palette *palette;               //< Paleta pro vyber barev

    bool timerRunning;                    //< Udava, zda-li bezi casovac
    int timerId;                          //< Identifikator casovace
    int actualStep;                       //< Cislo aktualniho kroku
    Automaton::GridType zygote;           //< Tvar zygoty
    Automaton::GridType pattern;          //< Hledany vzor
    std::vector<int> cellPossibilities;   //< Mozne stavy bunek automatu
    GridWidget *basicRulesGridWidget;     //< Mrizka pro automat vyvijejici se podle zakladnich pravidel
    GridWidget *generalRulesGridWidget;   //< Mrizka pro automat vyvijejici se podle pravidel pro sebeopravu
    Automaton::RuleSet basicRules;        //< Zakladni pravidla pro vyvoj ze zygoty do hledaneho vzoru
    Automaton::RuleSet generalRules;      //< Pravidla schopne sebeopravy poskozeneho vzoru
    AnalyseManager analyseManager;        //< Spravce analyzy pravidel (sledovani automatu po poskozeni)
    Automaton::HammingDistanceStatistic basicRulesHammingDistanceStatistic;   //< Zavislost Hammingovi vzdalenosti na casovem kroku
    Automaton::HammingDistanceStatistic generalRulesHammingDistanceStatistic; //< Zavislost Hammingovy vzdalenosti na casovem kroku

    /**
    * Nastavi prislusne ovladaci prvky udaji z konfiguracniho souboru.
    */
    void SetConfigurationValues();

    /**
    * Nacte pravidla a ziska cestu k souboru s ulohou.
    * @param inputFile Cesta k vstupniho souboru s pravidly.
    * @param taskFile Cesta k souboru s ulohou - ziska se ze souboru s pravidly.
    * @return Udava, zda se podarilo nacist pravidla ze souboru.
    */
    bool LoadRules(std::string inputFile, std::string &taskFile);

    /**
    * Nacte ulohu ze souboru.
    * @param taskFile Cesta k vstupnimu souboru s ulohou.
    * @return Udava, zda se podarilo nacist ulohu ze souboru.
    */
    bool LoadTask(std::string taskFile);

    /**
    * Ulozi vysledky do souboru.
    * @param outputFile Cesta k vystupnimu souboru.
    * @return Udava, zda se podarilo ulozit vysledky do souboru.
    */
    bool SaveResults(std::string outputFile);

    protected:
    void timerEvent(QTimerEvent *);

    public:
    /**
    * Konstruktor.
    * @param parent Rodicovsky objekt.
    */
    AnalyseRulesTab(QWidget *parent = 0);

    private slots:
    /**
    * Zjisti od uzivatele cestu k souboru s ulozenymi pravidly.
    */
    void SetOpenRulesFile();

    /**
    * Zjisti od uzivatele cestu k souboru, do ktereho budou ulozeny vysledky.
    */
    void SetSaveResultsFile();

    /**
    * Provede restart ulohy - nastaveni mrizek a citacu do pocatecniho stavu.
    */
    void ResetTask();

    /**
    * Vykona nastaveny pocet kroku automatu.
    */
    void ExecuteSteps();

    /**
    * Prepina proces krokovani vyvoje automatu.
    */
    void ExecuteStepping();

    /**
    * Zobrazi zaslane stavy mrizek automatu s vyvojem podle zakladnich pravidel i s vyvojem podle pravidel pro sebeopravu.
    * @param basicRulesAutomatonGrid Aktualni stav mrizky automatu vyvijeneho se zakladnimi pravidly.
    * @param generalRulesAutomatonGrid Aktualni stav mrizky automatu vyvijeneho s pravidly pro sebeopravu.
    * @param basicRulesGridHammingDistance Pocet bunek, ve kterych se automat se zakladnimi pravidly lisi od hledaneho vzoru.
    * @param generalRulesGridHammingDistance Pocet bunek, ve kterych se automat s pravidly pro sebeopravu lisi od hledaneho vzoru.
    * @param lastGrid Udava, zda jsou zasilane stavy mrizky posledni.
    */
    void ShowGridGenerated(Automaton::GridType basicRulesAutomatonGrid, Automaton::GridType generalRulesAutomatonGrid, int basicRulesGridHammingDistance, int generalRulesGridHammingDistance, bool lastGrid);
};

#endif // ANALYSE_RULES_TAB_H_INCLUDED
