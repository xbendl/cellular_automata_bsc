#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

Config::Config()
{
    neighbourhood = 4;
    automatonStep = 0;
    maxEvolutionStep = 0;
    lockStepCount = 0;
    mutationProbability = 0;
    damagedConfigurationCount = 0;
    maxDamagedPercentage = 0;
    resetGridPercentageLimit = 0;
    resetCellPercentageLimit = 0;
    fitnessMeasuringSequence = 0;
    initialPhaseStep = 0;
    defaultDamage = 0;
    defaultStepInterval = 1;
    evolutionGraphValuesCount = 100;
    timeIntervalEvolutionRules = 1000;
    timeIntervalAnalyseRules = 1000;

    extendedRuleCalculation = false;
    respectFitnessImprovement = false;
}

Config::~Config()
{
    if(configInstance != 0)
        delete configInstance;
}

bool Config::LoadConfiguration(string configFile)
{
    // Otevreni souboru
    ifstream inFile;
    inFile.open(configFile.c_str(), std::ifstream::in);
    if(inFile.fail())
    {   // Konfiguracni soubor se nepodarilo otevrit
        return false;
    }

    // Ziskani promennych z konfiguracniho souboru
    string line;
    string variableName;

    while(getline(inFile, line))
    {
        if(line.size() > 0 && isalpha(line[0]) && line.find('='))
        {
            variableName = line.substr(0, line.find('='));
            variableName.erase(variableName.find_last_not_of(' ') + 1);
            variableName.erase(0, variableName.find_first_not_of(' '));

            if(variableName == "Neighbourhood")
                neighbourhood = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "AutomatonStep")
                automatonStep = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "MutationProbability")
                mutationProbability = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "MaxEvolutionStep")
                maxEvolutionStep = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "DamagedConfigurationsCount")
                damagedConfigurationCount = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "MaxDamagedPercentage")
                maxDamagedPercentage = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "ExtendedRuleCalculation")
                extendedRuleCalculation = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "RespectFitnessImprovement")
                respectFitnessImprovement = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "LockStepCount")
                lockStepCount = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "InitialPhaseStep")
                initialPhaseStep = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "ResetGridPercentageLimit")
                resetGridPercentageLimit = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "ResetCellPercentageLimit")
                resetCellPercentageLimit = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "FitnessMeasuringSequence")
                fitnessMeasuringSequence = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "EvolutionGraphValuesCount")
                evolutionGraphValuesCount = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "TimeIntervalEvolutionRules")
                timeIntervalEvolutionRules = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "TimeIntervalAnalyseRules")
                timeIntervalAnalyseRules = atof(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "DefaultStepInterval")
                defaultStepInterval = atoi(line.substr(line.find('=') + 1).c_str());
            else if(variableName == "DefaultDamage")
                defaultDamage = atof(line.substr(line.find('=') + 1).c_str());
        }
    }
    return true;
}

Config *Config::configInstance = 0;
