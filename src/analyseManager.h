/**
* \file analyseManager.h
* Spravce analyzy pravidel.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef ANALYSE_MANAGER_H_INCLUDED
#define ANALYSE_MANAGER_H_INCLUDED
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QMetaType>
#include "automaton.h"
#include "config.h"

/// Spravce analyzy pravidel.
class AnalyseManager : public QThread
{
    Q_OBJECT

    private:
    Automaton::GridType zygote;            //< Zygota
    Automaton::GridType pattern;           //< Hledany vzor
    bool abortFlag;                        //< Priznak ukonceni evolucnich vypoctu
    bool sleepFlag;                        //< Priznak pozastaveni evolucnich vypoctu
    Automaton *basicRulesAutomaton;        //< Deterministicky pracujici automat
    Automaton *generalRulesAutomaton;      //< Nedeterministicky pracujici automat
    std::vector<int> cellPossibilities;    //< Mozne stavy bunek automatu
    int executeCycles;                     //< Pocet prechodu automatu, ktere maji byt provedeny
    double damageProbability;              //< Pravdepodobnost poskozeni
    QMutex mutex;
    QWaitCondition condition;

    /**
    * Vystupy pravidel tvorene primo hodnotami se zmeni v mrizce na vystupy pravidel tvorene poradovymi cisly hodnot.
    * @param originalGrid Puvodni mrizka.
    */
    Automaton::GridType TranslateGridConfiguration(Automaton::GridType originalGrid);

    /**
    * Vystupy pravidel tvorene poradovymi cisly hodnot se zmeni v mrizce na vystupy pravidel tvorene primo hodnotami.
    * @param originalGrid Puvodni mrizka.
    */
    Automaton::GridType TranslateBackGridConfiguration(Automaton::GridType originalGrid);

    public:
    /**
    * Konstruktor.
    * @param parent Rodicovsky objekt.
    */
    AnalyseManager(QObject *parent = 0);

    /**
    * Destruktor.
    */
    ~AnalyseManager();

    /**
    * Smycka evolucniho vypoctu.
    */
    void run();

    /**
    * Sleduje chovani automatu pri vyvoji podle nactenych pravidel.
    * @param grid Zygota.
    * @param pattern Hledany vzor.
    * @param basicRules Zakladni pravidla automatu pro vyvoj ze zygoty do hledaneho vzoru.
    * @param generalRules Pravidla automatu schopne sebeopravy v pripade poskozeni stavu bunek.
    * @param cellPossibilities Mozne stavy bunek automatu.
    */
    void ObserveAfterDamageRule(Automaton::GridType zygote, Automaton::GridType pattern, Automaton::RuleSet basicRules, Automaton::RuleSet generalRules, std::vector<int> cellPossibilities);

    /**
    * Provede stanoveny pocet prechodu sledovanych automatu.
    * @param basicRulesGrid Mrizka automatu pracujiciho se zakladnimi pravidly.
    * @param generalRulesGrid Mrizka automatu pracujiciho s pravidly pro sebeopravu.
    * @param cyclesCount Pocet cyklu, ktere maji byt provedeny.
    * @param damageProbability Mira poskozeni v kazdem kroku vyvoje.
    */
    void SetCycles(Automaton::GridType basicRulesGrid, Automaton::GridType generalRulesGrid, int cyclesCount, double damageProbability = 0);

    signals:
    /**
    * Signal zaslany po vykonani jednoho kroku sledovanych automatu.
    * @param basicRulesGrid Mrizka automatu pracujiciho se zakladnimi pravidly.
    * @param generalRulesGrid Mrizka automatu pracujiciho s pravidly pro sebeopravu.
    * @param basicRulesGridHammingDistance Pocet bunek, ve kterych se automat se zakladnimi pravidly lisi od hledaneho vzoru.
    * @param generalRulesGridHammingDistance Pocet bunek, ve kterych se automat s pravidly pro sebeopravu lisi od hledaneho vzoru.
    * @param lastGrid Udava, zda jsou zasilane stavy mrizky posledni.
    */
    void GridGenerated(Automaton::GridType basicRulesGrid, Automaton::GridType generalRulesGrid, int basicRulesGridHammingDistance, int generalRulesGridHammingDistance, bool lastGrid);
};

#endif // ANALYSE_MANAGER_H_INCLUDED
