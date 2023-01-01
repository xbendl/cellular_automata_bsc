/**
* \file evolutionManager.h
* Spravce evoluce pravidel.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef AUTOMATON_MANAGER_H_INCLUDED
#define AUTOMATON_MANAGER_H_INCLUDED
#include <iostream>
#include <math.h>
#include <time.h>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QMetaType>
#include <stdlib.h>
#include "automaton.h"
#include "config.h"

/// Spravce evoluce pravidel.
class EvolutionManager : public QThread
{
    Q_OBJECT

    private:
    Automaton::GridType zygote;           //< Zygota
    Automaton::GridType pattern;          //< Hledany vzor
    int size;                             //< Rozmer mrizky
    int cellCount;                        //< Pocet bunek automatu
    int rulesCount;                       //< Celkovy pocet pravidel pro jednu bunku automatu (odvozeno od poctu moznych stavu bunky)
    int initialStep;                      //< Krok, od ktereho se meri uspesnost pravidel / mrizky a je uvazovano o jejich restartu
    time_t timeSummary;                  //< Celkovy cas behu simulace
    double bestBasicRulesFitness;        //< Nejlepsi fitness zakladnich pravidel dosazene evoluci
    double bestGeneralRulesFitness;      //< Nejlepsi fitness pravidel pro sebeopravu dosazene evoluci
    Automaton::RuleSet rules;             //< Pomocna pro evoluci pravidel
    Automaton::RuleSet bestBasicRules;    //< Zakladni pravidla pro vyvoj ze zygoty do hledaneho vzoru
    Automaton::RuleSet bestGeneralRules;  //< Pravidla schopne sebeopravy
    bool abortFlag;                       //< Priznak ukonceni evolucnich vypoctu
    bool sleepFlag;                       //< Priznak pozastaveni evolucnich vypoctu
    Automaton::CellFitness fitness;       //< Fitness jednotlivych bunek
    std::vector<int> maxFitness;          //< Pocet po sobe jdoucich stavu, v kterych ma bunka cilovou hodnotu
    std::vector<int> fitnessSum;          //< Pomocna promenna pro soucty fitness
    std::vector<int> cellPossibilities;   //< Mozne stavu bunky automatu
    std::vector<std::vector<bool> > basicRules;  //< Priznak urcujici, zda-li pravidlo spada mezi "zakladni"

    QMutex mutex;
    QWaitCondition condition;

    /**
    * Vygeneruje sadu pravidel pro jednu bunku.
    * @param cellNumber Cislo bunky automatu, ke ktere bude prirazena vygenerovana sada pravidel.
    * @param basicRulesControl Urci, zda-li se ma pri generovani pravidla brat ohled na zakladni pravidla.
    */
    void GenerateRule(int cellNumber, bool basicRulesControl);

    /**
    * Vygeneruje mnozinu zamerne poskozenych mrizek odvozenych od hledaneho vzoru.
    * @param damagedGrids Vektor pro ulozeni vygenerovanych, zamerne poskozenych mrizek.
    */
    void GenerateDamagedGrids(std::vector<Automaton::GridType> &damagedGrids);

    /**
    * Vygeneruje sadu pravidel pro kazdou automatu.
    * @param basicRulesControl Urci, zda-li se ma pri resetu pravidla brat ohled na zakladni pravidla.
    */
    void GenerateRules(bool basicRulesControl);

    /**
    * Prelozi hodnoty stavu bunek na poradova cisla stavu hodnot bunek (aby sla pouzit soustava o zakladu N, kde N je pocet stavu).
    * @param originalGrid Puvodni mrizka.
    * @return Prelozena mrizka.
    */
    Automaton::GridType TranslateGridConfiguration(Automaton::GridType originalGrid);

    /**
    * Srovna ukazatele na sady pravidel bunek podle fitness jednotlivych bunek.
    * @param position Vektor se sadami pravidel jednotlivych bunek.
    */
    void SortByFitness(std::vector<int> &position);

    /**
    * Zjisti pocet a pravidla lepe ohodnocenych sousedu aktualni bunky.
    * @param cellNumber Cislo bunky, pro ktere zjistujeme lepe ohodnocene sousedy.
    * @param fitterNeighbour Uloziste pro pravidla sousedu s lepsim fitness.
    * @return Pocet sousedu s lepsim fitness.
    */
    int GetFitterNeighbours(int cellNumber, Automaton::RuleSet &fitterNeighbour);

    /**
    * Zkopiruje pravidlo.
    * @param sourceRule Zdrojove pravidlo.
    * @param destRule Cilove pravidlo.
    * @param destCellNumber Cislo bunky, kam se cilove pravidlo bude kopirovat.
    */
    void MoveRule(Automaton::Rule &sourceRule, Automaton::Rule &destRule, int destCellNumber);

    /**
    * Provede mutaci pravidla bunky.
    * @param selectedRule Mutovane pravidlo.
    * @param probability Explicitni urceni pravdepodobnosti mutace.
    */
    void MutateRule(Automaton::Rule &selectedRule, double probability);

    /**
    * Provede mutaci pravidla bunky, bere ohled na zakladni pravidla.
    * @param selectedRule Mutovane pravidlo.
    * @param probability Explicitni urceni pravdepodobnosti mutace.
    * @param cellNumber Cislo bunky.
    */
    void MutateRule(Automaton::Rule &selectedRule, double probability, int cellNumber);

    /**
    * Provede krizeni dvou pravidel.
    * @param selectedRuleSet Prvni pravidlo pro krizeni.
    * @param selectedRuleSet2 Druhe pravidlo pro krizeni.
    * @param newRuleSet Pravidlo vznikle krizenim.
    */
    void CrossingRule(Automaton::Rule &selectedRuleSet, Automaton::Rule &selectedRuleSet2, Automaton::Rule &newRuleSet);

    /**
    * Provede krizeni dvou pravidel s ohledem na zakladni pravidla.
    * @param selectedRuleSet Prvni pravidlo pro krizeni.
    * @param selectedRuleSet2 Druhe pravidlo pro krizeni.
    * @param newRuleSet Pravidlo vznikle krizenim.
    * @param cellNumber Cislo bunky.
    */
    void CrossingRule(Automaton::Rule &selectedRuleSet, Automaton::Rule &selectedRuleSet2, Automaton::Rule &newRuleSet, int cellNumber);

    /**
    * Nastavuje pro kazdou bunku po kazdem vyvojovem kroku pocet po sobe jdoucich kroku, kdy mela bunka maximalni fitness.
    * @param configurationCount Pocet konfiguraci uvazovanych v procesu evoluce.
    */
    void CheckConfigurations(int configurationCount);

    /**
    * V pripade dlouhodobe neuspesnosti pravidla bunky provede reset tohoto pravidla.
    * @param evolutionStep Aktualni krok vyvojoveho cyklu.
    * @param configurationCount Pocet konfiguraci uvazovanych v procesu evoluce.
    * @param basicRulesControl Urci, zda-li se ma pri resetu pravidla brat ohled na zakladni pravidla.
    */
    void SolveLongtermUnsuccess(int evolutionStep, int configurationCount, bool basicRulesControl);

    /**
    * Zjisti, zda-li uspesnost pravidel v ramci cele mrizky je natolik nizka, za je nutno pravidla resetovat.
    * @param evolutionStep Aktualni krok vyvojoveho cyklu.
    * @param configurationCount Pocet konfiguraci uvazovanych v procesu evoluce.
    * @return Doporuceni, zda-li se maji pravidla vsech bunek restartovat.
    */
    bool IsRecommendResetGrid(int evolutionStep, int configurationCount);

    /**
    * Zjisti, zda-li vsechny poskozene automaty provedly zotaveni.
    * @return Priznak urcujici, zda-li vsechny poskozene automaty provedly zotaveni.
    */
    bool IsFinalConfigurationAttained();

    public:
    /**
    * Konstruktor.
    * @param parent Rodicovsky objekt.
    */
    EvolutionManager(QObject *parent = 0);

    /**
    * Destruktor.
    */
    ~EvolutionManager();

    /**
    * Hleda pravidla pro generovani pozadovaneho vzoru populaci CA.
    * @param zygote Zygota.
    * @param pattern Hledany vzor.
    * @param cellPossibilities Mozne stavy bunky automatu.
    */
    void FindSolution(Automaton::GridType zygote, Automaton::GridType pattern, std::vector<int> cellPossibilities);

    /**
    * Pozastavi evoluci pravidel.
    */
    void StopEvoluteRules();

    /**
    * Zrusi evoluci pravidel.
    */
    void AbortEvoluteRules();

    /**
    * Vrati pravidla automatu.
    * @param basicRules Zakladni pravidla pro vyvoj ze zygoty do hledaneho vzoru.
    * @param generalRules Pravidla schopne sebeopravy poskozeneho vzoru.
    * @param basicRulesFitness Fitness zakladnich pravidel.
    * @param generalRulesFitness Fitness pravidel schopnych sebeopravy poskozeneho vzoru.
    * @param timeSummary Dosavadni doba vypoctu.
    */
    void GetActualResults(Automaton::RuleSet &basicRules, Automaton::RuleSet &generalRules, double &basicRulesFitness, double &generalRulesFitness, time_t &timeSummary);

    /**
    * Smycka evolucniho vypoctu.
    */
    void run();

    signals:
    /**
    * Signal zaslany po vykonani jednoho evolucniho kroku pri evoluci zakladnich pravidel automatu.
    * @param averageFitness Prumerne fitness v aktualnim evolucnim kroku.
    */
    void ActualBasicRuleFitness(double actAverageFitness);

    /**
    * Signal zaslany po vykonani jednoho evolucniho kroku pri evoluci pravidel pro poskozene automaty.
    * @param averageFitness Prumerne fitness v aktualnim evolucnim kroku.
    * @param bestFitness Nejlepsi fitness v aktualnim evolucnim kroku.
    */
    void ActualDamagedConfigurationRuleFitness(double actAverageFitness, double actBestFitness);

    /**
    * Signal zaslany po dokonceni evoluce zakladnich pravidel.
    * @param rules Vyvinuta pravidla.
    * @param timeSummary Dosavadni cas doby vypoctu zakladnich pravidel.
    * @param bestAverageFitness Fitness vyvinutych pravidel.
    */
    void BasicRulesGenerated(Automaton::RuleSet rules, int timeSummary, double fitness);

    /**
    * Signal zaslany po dokonceni evoluce pravidel schopnych opravy poskozeneho vzoru
    * @param rules Vyvinuta pravidla.
    * @param timeSummary Celkovy cas doby vypoctu pravidel pro opravu poskozeneho vzoru.
    * @param bestAverageFitness Fitness vyvinutych pravidel.
    */
    void GeneralRulesGenerated(Automaton::RuleSet rules, int timeSummary, double fitness);
};

#endif //AUTOMATON_MANAGER_H_INCLUDED
