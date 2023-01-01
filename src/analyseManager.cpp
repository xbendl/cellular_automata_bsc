#include "analyseManager.h"

using namespace std;

AnalyseManager::AnalyseManager(QObject *) : QThread()
{
    basicRulesAutomaton = NULL;
    generalRulesAutomaton = NULL;

    // Nastaveni priznaku ukonceni a uspani
    abortFlag = false;
    sleepFlag = true;

    executeCycles = 0;
    damageProbability = 0;
}

AnalyseManager::~AnalyseManager()
{
    // Ukonceni vlakna
    mutex.lock();
    abortFlag = true;      // Nastaveni priznaku pro ukonceni
    condition.wakeOne();   // Probuzeni spiciho vlakna
    mutex.unlock();
    wait();                // Pockani na dobehnuti vlakna

    // Dealokace zabrane pameti
    if(basicRulesAutomaton != NULL)
        delete basicRulesAutomaton;
    if(generalRulesAutomaton != NULL)
        delete generalRulesAutomaton;
}

void AnalyseManager::ObserveAfterDamageRule(Automaton::GridType zygote, Automaton::GridType pattern, Automaton::RuleSet basicRules, Automaton::RuleSet generalRules, std::vector<int> cellPossibilities)
{
    this->zygote = zygote;
    this->pattern = pattern;
    this->cellPossibilities = cellPossibilities;

    if(basicRulesAutomaton != NULL)
        delete basicRulesAutomaton;
    if(generalRulesAutomaton != NULL)
        delete generalRulesAutomaton;

    Automaton::GridType translatedZygote = TranslateGridConfiguration(zygote);

    basicRulesAutomaton = new Automaton(translatedZygote, basicRules, cellPossibilities.size(), Config::GetInstance()->neighbourhood);
    generalRulesAutomaton = new Automaton(translatedZygote, generalRules, cellPossibilities.size(), Config::GetInstance()->neighbourhood);

    // Spusteni automatu
    if(!isRunning())
        start();
}

void AnalyseManager::run()
{
    int basicRulesGridHammingDistance, generalRulesGridHammingDistance;

    // Smycka provadejici prechody CA
    while(true)
    {
        // Uspani po provedeni prechodu
        if(executeCycles <= 0)
        {
            mutex.lock();
            condition.wait(&mutex);
            mutex.unlock();
        }
        executeCycles--;

        // Ukonceni pri nastaveni priznaku ukonceni
        mutex.lock();
        if(abortFlag)
        {
            mutex.unlock();
            return;
        }
        mutex.unlock();

        // Provedeni prechodu u obou automatu
        basicRulesAutomaton->AutomatonStep(1, damageProbability);
        generalRulesAutomaton->AutomatonStep(1, damageProbability);

        // Zjisteni Hammingovy vzdalenosti mrizek automatu
        basicRulesGridHammingDistance = Automaton::HammingDistance(basicRulesAutomaton->grid, TranslateGridConfiguration(pattern));
        generalRulesGridHammingDistance = Automaton::HammingDistance(generalRulesAutomaton->grid, TranslateGridConfiguration(pattern));

        // Signal zasilajici aktualni stavy automatu
        bool isLastGrid = (executeCycles == 0) ? true : false;   // Rozhodnuti, zda-li jde o posledni pozadovany prechod (bylo-li pozadovanych prechodu vice)
        emit GridGenerated(TranslateBackGridConfiguration(basicRulesAutomaton->grid), TranslateBackGridConfiguration(generalRulesAutomaton->grid), basicRulesGridHammingDistance, generalRulesGridHammingDistance, isLastGrid);
    }
}

Automaton::GridType AnalyseManager::TranslateGridConfiguration(Automaton::GridType originalGrid)
{
    // Alokace prostoru pro docasnou mrizku
    Automaton::GridType translatedGrid;
    Automaton::CellVector emptyCellVector;
    emptyCellVector.assign(originalGrid.size(), (Automaton::CellType) 0);
    translatedGrid.assign(originalGrid.size(), emptyCellVector);

    for(unsigned int i = 0; i < cellPossibilities.size(); i++)
        for(unsigned int row = 0; row < originalGrid.size(); row++)
            for(unsigned int col = 0; col < originalGrid.size(); col++)
                if(originalGrid[row][col] == cellPossibilities[i])
                    translatedGrid[row][col] = i;

    return translatedGrid;
}

Automaton::GridType AnalyseManager::TranslateBackGridConfiguration(Automaton::GridType originalGrid)
{
    // Alokace prostoru pro docasnou mrizku
    Automaton::GridType translatedGrid;
    Automaton::CellVector emptyCellVector;
    emptyCellVector.assign(originalGrid.size(), (Automaton::CellType) 0);
    translatedGrid.assign(originalGrid.size(), emptyCellVector);

    for(unsigned int i = 0; i < cellPossibilities.size(); i++)
        for(unsigned int row = 0; row < originalGrid.size(); row++)
            for(unsigned int col = 0; col < originalGrid.size(); col++)
                if(originalGrid[row][col] == i)
                    translatedGrid[row][col] = cellPossibilities[i];

    return translatedGrid;
}

void AnalyseManager::SetCycles(Automaton::GridType basicRulesGrid, Automaton::GridType generalRulesGrid, int cyclesCount, double damageProbability)
{
    // Probuzeni vlakna k provedeni dalsiho prechodu automatu
    mutex.lock();
    this->damageProbability = damageProbability;  // Nastaveni poskozeni
    executeCycles += cyclesCount;                 // Nastaveni poctu cyklu
    basicRulesAutomaton->SetGrid(TranslateGridConfiguration(basicRulesGrid));
    generalRulesAutomaton->SetGrid(TranslateGridConfiguration(generalRulesGrid));
    condition.wakeOne();
    mutex.unlock();
}
