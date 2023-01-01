/**
* \file automaton.h
* Reprezentace celularniho automatu.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef AUTOMATON_H_INCLUDED
#define AUTOMATON_H_INCLUDED
#include <vector>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <QMetaType>

/// Celularni automat.
class Automaton
{
    public:
    /// Typ pro ulozeni hodnoty bunky automatu.
    typedef unsigned char CellType;

    /// Typ pro ulozeni radku hodnot bunek automatu.
    typedef std::vector<CellType> CellVector;

    /// Typ pro ulozeni mrizky automatu.
    typedef std::vector<CellVector> GridType;

    /// Typ pro ulozeni sady pravidel pro 1 bunku automatu.
    typedef std::vector<CellType> Rule;

    /// Typ pro ulozeni sad pravidel pro kazdou bunku automatu.
    typedef std::vector<Rule> RuleSet;

    /// Typ pro ulozeni fitness kazde bunky automatu.
    typedef std::vector<int> CellFitness;

    /// Typ pro ulozeni informaci o hammingove vzdalenosti dvou automatu v zavislosti na kroku automatu.
    typedef std::vector<double> HammingDistanceStatistic;

    /// Struktura pro ulozeni informaci o pravidle.
    typedef struct
    {
        std::string ruleOutput;  ///< Vystup pravidla.
        int appearanceCount;     ///< Pocet vyskytu vystupu pravidla.
        double lambda;           ///< Pomer kombinaci sousedu, z kterych vzejde neklidovy stav vuci kombinaci sousedu, z kterych vzejde klidovy stav.
    } RuleStatistic;

    GridType grid;                ///< Mrizka automatu.

    /**
    * Konstruktor.
    * @param zygote Zygota.
    * @param pattern Hledany vzor.
    * @param rules Pravidla automatu.
    * @param cellPossibilitiesCount Pocet moznych stavy bunky automatu.
    * @param neighbourhood Pocet prvku ovlivnujici nasledujici stav bunky (pocet sousedu).
    */
    Automaton(GridType &zygote, GridType &pattern, RuleSet &rules, int cellPossibilitiesCount, int neighbourhood);

    /**
    * Konstruktor.
    * @param zygote Zygota.
    * @param rules Pravidla automatu.
    * @param cellPossibilitiesCount Pocet moznych stavy bunky automatu.
    * @param neighbourhood Pocet prvku ovlivnujici nasledujici stav bunky (pocet sousedu).
    */
    Automaton(GridType &zygote, RuleSet &rules, int cellPossibilitiesCount, int neighbourhood);

    /**
    * Provede urceny pocet prechodu celularniho automatu.
    * @param stepCount Pozadovany pocet prechodu.
    */
    void AutomatonStep(int stepCount);

    /**
    * Provede urceny prechodu celularniho automatu a s urcitou pravdepodobnosti pozmeni vystupni hodnoty bunek.
    * @param stepCount Pozadovany pocet prechodu.
    * @param damageProbability Pravdepodobnost zmeny hodnoty bunky.
    */
    void AutomatonStep(int stepCount, double damageProbability);

    /**
    * Provede urceny pocet prechodu celularniho automatu a zjisti, zda-li predchody vedou k dosazeni hledaneho vzoru.
    * @param stepCount Pozadovany pocet prechodu.
    * @param fitness Slouzi pro ulozeni fitness jednotlivych bunek.
    * @param basicRules Slouzi pro nastavei priznaku u pouzitych pravidel nutnych k dosazeni hledaneho vzoru.
    * @return Udava, zda-li behem prechodu byly nalezeny pravidla vedouci k vyvoji pozadovaneho vzoru.
    */
    double AutomatonStep(int stepCount, CellFitness &fitness, std::vector<std::vector<bool> > *basicRules = NULL);

    /**
    * Vytiskne mrizku automatu na standardni vystup.
    */
    void PrintGrid();

    /**
    * Nastavi mrizku automatu.
    * @param grid Mrizka, ktera bude nastavena do automatu.
    */
    void SetGrid(GridType grid);

    /**
    * Zjisti hammingovu vzdalenost dvou automatu (pocet bunek, ve kterych se lisi).
    * @param automaton1 Prvni porovnavany automat.
    * @param automaton2 Druhy porovnavany automat.
    * @return Diference
    */
    static int HammingDistance(GridType grid1, GridType grid2);

    private:
    int size;                     //< Delka strany automatu
    int cellCount;                //< Pocet bunek automatu
    int neighbourhood;            //< Pocet prvku ovlivnujici nasledujici stav bunky (pocet sousedu)
    RuleSet rules;                //< Pravidla automatu
    GridType pattern;             //< Hledany vzor
    int cellPossibilitiesCount;   //< Pocet moznych stavu bunky automatu

    /**
    * Zmeni hodnoty bunek na cisla reprezentujici danou hodnotu.
    * @param oldGrid Puvodni mrizka, ve ktere jsou hodnoty bunek.
    * @return Nova mrizka, ve ktere jsou cisla bunek reprezentujici hodnotu bunek.
    */
    Automaton::GridType TranslateConfiguration(GridType oldGrid);

    /**
    * Provede prechod u jedne bunky - zmeni jeji stav podle jejiho okoli.
    * @param cellNumber Cislo bunky.
    * @param newGrid Pomocna mrizka, do ktere jsou zapisovany nove stavy bunek.
    * @return Cislo pravidla, podle ktereho byl provede prechod.
    */
    int SetNewConfiguration(int cellNumber, GridType &newGrid);

    /**
    * Prevede cislo ze soustavy o zakladu N do desitkove soustavy.
    * @param actConfig Prevadene cislo v soustave o zakladu N.
    * @param length Delka pole s prevadenym cislem.
    * @param systemBase Zaklad soustavy, z ktere prevod provadime.
    * @return Cislo vyjadrena v desitkove soustave.
    */
    int ConvertToDecimalSystem(int actConfig[], int length, int systemBase);

    /**
    * Prevede cislo z desitkove soustavy do soustavy o zakladu N.
    * @param decimalActConfig Prevadene cislo v desitkove soustave.
    * @param actConfig Pole pro ulozeni prevedeneho decimalniho cisla do soustavy o zakladu N.
    * @param length Delka pole s prevadenym cislem.
    * @param systemBase Zaklad soustavy, z ktere prevod provadime.
    */
    void ConvertToBaseNSystem(int decimalActConfig, int actConfig[], int length, int systemBase);
};

Q_DECLARE_METATYPE(Automaton::GridType)

Q_DECLARE_METATYPE(Automaton::CellFitness)

#endif //AUTOMATON_H_INCLUDED
