/**
* \file config.h
* Spravce konfiguracnich udaju.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <iostream>
#include <stdlib.h>
#include <fstream>

/// Spravce konfiguracnich udaju.
class Config
{
    private:
    /**
    * Konstruktor.
    */
    Config();

    public:
    /*int neighbourhood;                      //< Celkovy pocet prvku ovlivnujici nasledujici stav bunky
    int automatonStep;                      //< Pocet kroku CA behem jednoho vyvojoveho kroku
    int maxEvolutionStep;                   //< Maximalni pocet vyvojovych kroku - pri jeho dosazeni se prepocitaji parametry simulace
    int lockStepCount;                      //< Pocet po sobe jdoucich evolucnich kroku, ve kterych musi byt fitness bunky maximalni mozne, po kterych se dosahne zamknuti pravidel bunky
    int initialPhaseStep;                   //< Pocet kroku po startu simulace, ve kterych nemohou byt pravidla bunek restartovana
    int fitnessMeasuringSequence;           //< Delka sekvence hodnot fitness branych v potaz pri vypoctu uspesnosti pravidla
    int damagedConfigurationCount;          //< Pocet zamerne poskozenych konfiguraci odvozenych od hledane konfigurace, ktere budou pouzity pro trenovani schopnosti sebeopravy
    int evolutionGraphValuesCount;          //< Pocet hodnot na grafu vyvoje fitness pri evoluci pravidel
    int defaultStepInterval;                //< Pocet kroku nabizeny pro preskoceni pri analyze pravidel
    bool extendedRuleCalculation;           //< Udava, zda se pro vypocet noveho pravidla pouziji pravidla sousedicich 4 bunek (false) nebo 8 bunek (true)
    bool respectFitnessImprovement;         //< Udava, ma-li byt respektovano puvodni pravidlo bunky, zvysi-li se po vyvojovem kroku jeji fitness (prestoze stale existuji sousede s vyssim fitness)
    double defaultDamage;                   //< Velikost poskozeni nabizena defaultne pri analyze pravidel
    double maxDamagedPercentage;            //< Maximalni mozne procentualni poskozeni vzoru pri trenovani schopnosti sebeopravy
    double mutationProbability;             //< Pravdepodobnost mutace
    double resetGridPercentageLimit;        //< Procentualni vyjadreni limitu pro pocet bunek, ktere musi byt v cilovem stavu, aby nedoslo k resetu vsech pravidel mrizky
    double resetCellPercentageLimit;        //< Procentualni vyjadreni uspesnosti pravidla bunky, ktere je nutne k tomu, aby nedoslo k resetu pravidla
    double timeIntervalEvolutionRules;      //< Interval pro pridani nove hodnoty do grafu vyvoje fitness pri evoluci pravidel
    double timeIntervalAnalyseRules;        //< Interval mezi prechody automatu behem jeho analyzy
    std::string inputEvolutionFile;         //< Cesta ke vstupniho souboru pri vyuziti programu k evoluci pravidel
    std::string inputAnalyseFile;           //< Cesta ke vstupniho souboru pri vyuziti programu k analyze pravidel
    static Config *configInstance;          //< Reference na instanci objektu*/

    /**
    * Nacte konfiguracni udaje z konfiguracniho souboru.
    * @param configFile Jmeno konfiguracniho souboru.
    * @return Udava, zda-li se nacteni konfiguracnich udaju podarilo.
    */
    //void LoadConfiguration(std::string configFile);

    /**
    * Vrati referenci na instanci objektu (pokud neexistuje, vytvori ji).
    * @return Reference na instanci objektu.
    */
    static Config *GetInstance()
    {
        if (!configInstance)
          configInstance = new Config;

        return configInstance;
    }
};

#endif // CONFIG_H_INCLUDED

