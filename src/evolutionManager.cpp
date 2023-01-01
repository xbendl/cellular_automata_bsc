#include "evolutionManager.h"

using namespace std;

EvolutionManager::EvolutionManager(QObject *) : QThread()
{
    // Nastaveni priznaku ukonceni a uspani
    abortFlag = false;
    sleepFlag = false;
    size = 0;
}

void EvolutionManager::FindSolution(Automaton::GridType zygote, Automaton::GridType pattern, std::vector<int> cellPossibilities)
{
    QMutexLocker locker(&mutex);

    // Zpracovani predanych parametru
    this->zygote = zygote;
    this->pattern = pattern;
    this->cellPossibilities = cellPossibilities;

    this->size = pattern.size();
    cellCount = this->size * this->size;
    bestBasicRules.clear();
    bestGeneralRules.clear();

    // Nastaveni priznaku ukonceni a uspani
    this->abortFlag = false;
    this->sleepFlag = false;

    // Nastaveni poctu pravidel na jednu bunku (odvozeno od poctu stavu na jednu bunku)
    rulesCount = 0;
    for(int i = 0; i < (Config::GetInstance()->neighbourhood + 1); i++)
        rulesCount += (cellPossibilities.size() - 1) * ((int) pow((double)cellPossibilities.size(), (double) i));
    rulesCount++;   // Pricteni pravidla pro okoli "same 0"

    // Vynulovani vektoru pro pravidla jednotlivych bunek automatu
    Automaton::Rule emptyRule;
    emptyRule.assign(rulesCount, ((Automaton::CellType) 0));
    rules.assign(cellCount, emptyRule);

    // Vynulovani vektoru pro oznaceni zakladnich pravidel automatu
    std::vector<bool> emptyBoolVector;
    emptyBoolVector.assign(rulesCount, false);
    basicRules.assign(cellCount, emptyBoolVector);

    // Spusteni evoluce
    if(!isRunning())
        start();
}

void EvolutionManager::run()
{
    // Zahajeni stopovani doby behu simulace
	time_t timeStart;       // Cas zapoceti  vypoctu
	timeStart = time(NULL);

    Automaton *automaton = NULL;                            // Vektor pro evolvovane automaty
    initialStep = Config::GetInstance()->initialPhaseStep;  // Urceni kroku, od ktereho je uvazovano o restartu pravidla v pripade jeho nizke uspesnosti
    fitness.assign(cellCount, 0);
    maxFitness.assign(cellCount, 0);
    fitnessSum.assign(cellCount, 0);

    Automaton::GridType translatedZygote = TranslateGridConfiguration(zygote);
    Automaton::GridType translatedPattern = TranslateGridConfiguration(pattern);

    // Inicializace tabulky pravidel pro kazdou bunku automatu
    GenerateRules(false);

    // Vynulovani vektoru pro oznaceni zakladnich pravidel automatu
    std::vector<std::vector<bool> > tempBasicRules;
    std::vector<bool> emptyBoolVector;
    emptyBoolVector.assign(rulesCount, false);
    tempBasicRules.assign(cellCount, emptyBoolVector);

    double actAverageFitness = 0;   // Pomocna pro uchovani aktualni hodnoty fitness
    this->bestBasicRulesFitness = 0;
    this->bestGeneralRulesFitness = 0;

    // Hledani pravidel pro vygenerovani pozadovaneho vzoru ze zygoty
    for(int evolutionStep = 0; evolutionStep < Config::GetInstance()->maxEvolutionStep; evolutionStep++)
    {
        // Uspani pri nastaveni priznaku uspani
        mutex.lock();
        if(sleepFlag)
        {
            condition.wait(&mutex);
            sleepFlag = false;
        }
        mutex.unlock();

        // Ukonceni pri nastaveni priznaku ukonceni
        mutex.lock();
        if(abortFlag)
        {
            mutex.unlock();
            return;
        }
        mutex.unlock();

        // Vygenerovani automatu, jehoz bunky jsou nastaveny podle zygoty
        automaton = new Automaton(translatedZygote, translatedPattern, rules, cellPossibilities.size(), Config::GetInstance()->neighbourhood);

        // Kontrola, zda-li po provedeni urceneho poctu kroku budou mit hodnoty bunek pozadovane stavy hledaneho vzoru
        if(automaton->AutomatonStep(Config::GetInstance()->automatonStep, fitness, &tempBasicRules) == 1)
        {
            actAverageFitness = 1;
            basicRules = tempBasicRules;
            bestBasicRules = rules;
            bestBasicRulesFitness = 1;
            delete automaton;
            //std::cout << "ZAKLADNI PRAVIDLA NALEZENA: " << std::endl;
            emit ActualBasicRuleFitness(1);
            break;
        }

        // Evoluce pravidel
        //automaton->PrintGrid();  // Vypis tabulky fitness a tabulky s automatem

        // Nastavuje pro kazdou bunku po kazdem vyvojovem kroku pocet po sobe jdoucich kroku, kdy mela bunka maximalni fitness
        if(Config::GetInstance()->lockStepCount != -1)
            CheckConfigurations(1);

        // V pripade velmi nizke uspesnosti pravidel jsou pravidla vsech bunek mrizky restartovany
        if((Config::GetInstance()->resetGridPercentageLimit != -1) && (IsRecommendResetGrid(evolutionStep, 1)))
        {
            maxFitness.assign(cellCount, 0);
            fitnessSum.assign(cellCount, 0);
            initialStep = evolutionStep + Config::GetInstance()->initialPhaseStep;

            GenerateRules(false);  // Generovani pravidel
        }

        // V pripade dlouhodobe neuspesnosti pravidla bunky je toto jedno pravidlo resetovano
        if(Config::GetInstance()->resetCellPercentageLimit != -1)
            SolveLongtermUnsuccess(evolutionStep, 1, false);

        int fitterNeighbourCount, position;
        Automaton::RuleSet fitterNeighbour;
        Automaton::Rule selectedRuleSet, selectedRuleSet2, newRuleSet;
        for(int i = 0; i < cellCount; i++)
        {
            fitterNeighbourCount = GetFitterNeighbours(i, fitterNeighbour);  // Zjisteni poctu lepe ohodnocenych sousedu

            if(fitterNeighbourCount == 1)
            {   // Nahrazeni aktualniho pravidla jedinym lepsim pravidlem a provedeni mutace tohoto pravidla
                selectedRuleSet = fitterNeighbour.front();
                MutateRule(selectedRuleSet, Config::GetInstance()->mutationProbability);
                rules[i] = selectedRuleSet;
            }
            else if(fitterNeighbourCount > 1)
            {   // Nahrazeni aktualniho pravidla pravidlem vzniklym krizenim dvou nahodne vybranych, lepe ohodnocenych, pravidel a provedenim mutace takto vznikleho pravidla
                position = rand() % fitterNeighbour.size();    // Prvni vybrane pravidlo
                selectedRuleSet = fitterNeighbour[position];
                fitterNeighbour.erase(fitterNeighbour.begin() + position);

                position = rand() % fitterNeighbour.size();    // Druhe vybrane pravidlo
                selectedRuleSet2 = fitterNeighbour[position];
                fitterNeighbour.erase(fitterNeighbour.begin() + position);

                CrossingRule(selectedRuleSet, selectedRuleSet2, newRuleSet);  // Krizeni vybranych pravidel
                MutateRule(newRuleSet, Config::GetInstance()->mutationProbability);          // Mutace pravidla vznikleho krizenim
                rules[i] = newRuleSet;
                newRuleSet.clear();
            }
            fitterNeighbour.clear();
        }

        // Vypocet prumerneho fitness v populaci v danem evolucnim kroku
        actAverageFitness = 0;
        for(unsigned int i = 0; i < fitness.size(); i++)
            actAverageFitness += fitness[i];
        actAverageFitness = (double) actAverageFitness / cellCount;

        // Vygenerovani signalu, zasilame fitness jednotlivych bunek automatu po provedeni jednoho evolucniho kroku
        emit ActualBasicRuleFitness(actAverageFitness);

        // Kontrola uspesnosti pravidel vygenerovanych v tomto kroku
        if(actAverageFitness > bestBasicRulesFitness)
        {
            bestBasicRules = rules;
            bestBasicRulesFitness = actAverageFitness;
        }

        // Kontrolni vypisy
        //cout << "Evolucni krok: " << evolutionStep << endl;
        //std::cout << "Fitness: " << actAverageFitness << std::endl;

        // Vynulovani fitness jednotlivych bunek
        fitness.assign(cellCount, 0);

        // Aktualizace doby trvani vypoctu
        timeSummary = time(NULL) - timeStart;

        delete automaton;
    }

    // Vygenerovani signalu; zasilame vygenerovana zakladni pravidla dosavadni cas vypoctu
    emit BasicRulesGenerated(bestBasicRules, timeSummary, bestBasicRulesFitness);

    // Hledani pravidel pro opravu poskozeneho vzoru
    int evolutionStep = 0;          // Aktualni vyvojovy krok
    int index = 0;                  // Ukazatel na aktualni automat
    double actFitness = 0;          // Fitness aktualniho automatu s poskozenym hledanym vzorem - pro aktualni evolucni krok
    double bestActFitness = 0;      // Nejlepsi fitness v mnozine automatu s poskozenym hledanym vzorem - pro aktualni evolucni krok
    std::vector<Automaton::GridType> damagedGrids;  // Vektor s poskozenymi mrizkami
    GenerateDamagedGrids(damagedGrids);             // Generovani poskozenych mrizek
    automaton = NULL;
    initialStep = Config::GetInstance()->initialPhaseStep;  // Urceni kroku, od ktereho je uvazovano o restartu pravidla v pripade jeho nizke uspesnosti
    Automaton::CellFitness prevFitness;

    prevFitness.assign(cellCount, 0);
    fitness.assign(cellCount, 0);
    maxFitness.assign(cellCount, 0);
    fitnessSum.assign(cellCount, 0);

    while(evolutionStep < Config::GetInstance()->maxEvolutionStep)
    {
        // Uspani pri nastaveni priznaku uspani
        mutex.lock();
        if(sleepFlag)
        {
            condition.wait(&mutex);
            sleepFlag = false;
        }
        mutex.unlock();

        // Ukonceni pri nastaveni priznaku ukonceni
        mutex.lock();
        if(abortFlag)
        {
            mutex.unlock();
            return;
        }
        mutex.unlock();

        // Vygenerovani automatu, jehoz bunky jsou nastaveny podle zygoty
        automaton = new Automaton(damagedGrids[index%Config::GetInstance()->damagedConfigurationCount], translatedPattern, rules, cellPossibilities.size(), Config::GetInstance()->neighbourhood);

        // Provedeni daneho poctu vyvojovych kroku automatu
        if((actFitness = automaton->AutomatonStep(Config::GetInstance()->automatonStep, fitness)) > bestActFitness)
        {
            bestActFitness = actFitness;
        }

        index++;

        // Evoluce pravidel
        if(index % Config::GetInstance()->damagedConfigurationCount == 0)
        {
            // Vypis tabulky fitness a tabulky s automatem
            //automaton->PrintGrid();

            // Nastaveni priznaku ukonceni v pripade nalezeni pravidel dostatecnych k dosazeni pozadovane konfigurace a setrvani v ni
            if(IsFinalConfigurationAttained())
            {
                actAverageFitness = 1;
                bestGeneralRulesFitness = 1;
                //std::cout << "SEBEOPRAVU ZAJISTUJICI PRAVIDLA NALEZENA" << std::endl;
                emit ActualDamagedConfigurationRuleFitness(1,1);
                break;
            }

            // Nastavuje pro kazdou bunku po kazdem vyvojovem kroku pocet po sobe jdoucich kroku, kdy mela bunka maximalni fitness
            if(Config::GetInstance()->lockStepCount != -1)
                CheckConfigurations(Config::GetInstance()->damagedConfigurationCount);

            // V pripade velmi nizke uspesnosti pravidel jsou pravidla vsech bunek mrizky restartovany
            if((Config::GetInstance()->resetGridPercentageLimit != -1) && IsRecommendResetGrid(evolutionStep, Config::GetInstance()->damagedConfigurationCount))
            {
                fitness.assign(cellCount, 0);
                maxFitness.assign(cellCount, 0);
                fitnessSum.assign(cellCount, 0);
                initialStep = evolutionStep + Config::GetInstance()->initialPhaseStep;

                GenerateRules(true);  // Generovani pravidel
                continue;
            }

            // V pripade dlouhodobe neuspesnosti pravidla bunky je toto jedno pravidlo resetovano
            if(Config::GetInstance()->resetCellPercentageLimit != -1)
                SolveLongtermUnsuccess(evolutionStep, Config::GetInstance()->damagedConfigurationCount, true);

            int fitterNeighbourCount, position;
            Automaton::RuleSet fitterNeighbour;           // Pravidla sousedu s lepsim fitness
            Automaton::Rule selectedRuleSet, selectedRuleSet2, newRuleSet;
            for(int cellNumber = 0; cellNumber < cellCount; cellNumber++)
            {
                fitterNeighbourCount = GetFitterNeighbours(cellNumber, fitterNeighbour);  // Zjisteni lepe ohodnocenych sousedu

                if((evolutionStep > 0) && (fitterNeighbourCount == 0) && (fitness[cellNumber] == prevFitness[cellNumber]) && (fitness[cellNumber] != Config::GetInstance()->damagedConfigurationCount))
                {   // Nemeni-li se v dvou po sobe jdoucich krocich fitness bunky a zaroven tato bunka nedosahuje maximalniho hodnoty fitness, je provedena mutace pravidla teto bunky
                    MutateRule(rules[cellNumber], Config::GetInstance()->mutationProbability, cellNumber);
                }
                else if((evolutionStep > 0) && (fitterNeighbourCount > 0) && (Config::GetInstance()->respectFitnessImprovement) && (fitness[cellNumber] > prevFitness[cellNumber]))
                {   // Pouze mutace soucasneho pravidla (lze sledovat pozitivni vyvoj hodnoty fitness)
                    MutateRule(rules[cellNumber], Config::GetInstance()->mutationProbability, cellNumber);
                }
                else if(fitterNeighbourCount == 1)
                {   // Nahrazeni aktualniho pravidla jedinym lepsim pravidlem a provedeni mutace tohoto pravidla
                    selectedRuleSet = fitterNeighbour.front();
                    MoveRule(selectedRuleSet, rules[cellNumber], cellNumber);
                    MutateRule(rules[cellNumber], Config::GetInstance()->mutationProbability, cellNumber);
                }
                else if(fitterNeighbourCount > 1)
                {   // Nahrazeni aktualniho pravidla pravidlem vzniklym krizenim dvou nahodne vybranych, lepe ohodnocenych, pravidel a provedenim mutace takto vznikleho pravidla
                    position = rand() % fitterNeighbour.size();    // Prvni vybrane pravidlo
                    selectedRuleSet = fitterNeighbour[position];
                    fitterNeighbour.erase(fitterNeighbour.begin() + position);
                    position = rand() % fitterNeighbour.size();    // Druhe vybrane pravidlo
                    selectedRuleSet2 = fitterNeighbour[position];
                    fitterNeighbour.erase(fitterNeighbour.begin() + position);

                    newRuleSet = rules[cellNumber];   // Pred zacatkem krizeni bude pravidlo v nezmenene podobe

                    CrossingRule(selectedRuleSet, selectedRuleSet2, newRuleSet, cellNumber);  // Krizeni vybranych pravidel
                    MutateRule(newRuleSet, Config::GetInstance()->mutationProbability, cellNumber);  // Mutace pravidla vznikleho krizenim
                    rules[cellNumber] = newRuleSet;
                    newRuleSet.clear();
                }
                fitterNeighbour.clear();
            }

            // Vypocet prumerneho fitness v populaci v danem evolucnim kroku
            actAverageFitness = 0;
            for(unsigned int i = 0; i < fitness.size(); i++)
                actAverageFitness += fitness[i];
            actAverageFitness = (double) actAverageFitness / (cellCount * Config::GetInstance()->damagedConfigurationCount);

            // Vygenerovani signalu, zasilame fitness jednotlivych bunek automatu po provedeni jednoho evolucniho kroku
            emit ActualDamagedConfigurationRuleFitness(actAverageFitness, bestActFitness);

            // Kontrola uspesnosti pravidel vygenerovanych v tomto kroku
            if(actAverageFitness > bestGeneralRulesFitness)
            {
                bestGeneralRules = rules;
                bestGeneralRulesFitness = actAverageFitness;
            }
            ///////////////////////////////////////////////////////////////////////////////////////////
            //cout << "Fitness: " << actAverageFitness << endl;
            //cout << "Evolucni krok: " << evolutionStep << endl;
            //cout << "Emit: " << actAverageFitness << endl;
            ///////////////////////////////////////////////////////////////////////////////////////////

            evolutionStep++;
            prevFitness = fitness;

            fitness.assign(cellCount, 0);  // Vynulovani fitness jednotlivych bunek

            bestActFitness = 0;            // Vynulovani nejlepsiho fitness v mnozine poskozenych automatu
        }

        // Aktualizace doby trvani vypoctu
        timeSummary = time(NULL) - timeStart;

        delete automaton;
    }

    // Vygenerovani signalu; zasilame vygenerovana pravidla schopna opravy poskozeneho vzoru a celkovy cas vypoctu
    emit GeneralRulesGenerated(bestGeneralRules, timeSummary, bestGeneralRulesFitness);
}

bool EvolutionManager::IsFinalConfigurationAttained()
{
    for(int i = 0; i < cellCount; i++)
    {
        if(fitness[i] != Config::GetInstance()->damagedConfigurationCount)
        {   // Nektery z automatu populace ma bunku v necilovem stavu
            return false;
        }
    }

    return true;
}

void EvolutionManager::GenerateDamagedGrids(std::vector<Automaton::GridType> &damagedGrids)
{
    Automaton::GridType translatedPattern = TranslateGridConfiguration(pattern);
    Automaton::CellVector rowValues;   // Pomocna pro ukladani hodnot na radku matice reprezentujici poskozeny vzor
    Automaton::GridType damagedGrid;   // Pomocna pro ukladani matice reprezentujici poskozeny vzor
    Automaton::CellType newCellValue;  // Pomocna pro generovani nahodne hodnoty bunky pro simulaci poskozeni
    double randomTreshold;             // Procentualni poskozeni mrizky

    for(int gridIndex = 0; gridIndex < Config::GetInstance()->damagedConfigurationCount; gridIndex++)  // Vektor mrizek
    {
        damagedGrid.clear();
        randomTreshold  = (double)rand() / RAND_MAX;
        //randomTreshold *= randomTreshold;
        randomTreshold *= Config::GetInstance()->maxDamagedPercentage;

        for(int row = 0; row < size; row++)      // Jedna mrizka
        {
            rowValues.clear();
            for(int col = 0; col < size; col++)  // Jeden radek mrizky
            {
                if((translatedPattern[row][col] != 0) && (((double)rand() / RAND_MAX) < randomTreshold))  // Zamena hodnoty vzoru
                {
                    do
                    {
                        newCellValue = rand() % cellPossibilities.size();  // Na misto dane bunky bude vlozena nahodna hodnota
                    }
                    while(newCellValue == translatedPattern[row][col]);  // Nahodna hodnota musi byt jina nez puvodni hodnota v miste bunky

                    rowValues.push_back(newCellValue);
                }
                else
                {
                    rowValues.push_back(translatedPattern[row][col]);
                }
            }
            damagedGrid.push_back(rowValues);
        }
        damagedGrids.push_back(damagedGrid);
    }
}

Automaton::GridType EvolutionManager::TranslateGridConfiguration(Automaton::GridType originalGrid)
{
    // Alokace prostoru pro docasnou mrizku
    Automaton::GridType translatedGrid;
    Automaton::CellVector emptyCellVector;
    emptyCellVector.assign(size, (Automaton::CellType) 0);
    translatedGrid.assign(size, emptyCellVector);

    for(unsigned int i = 0; i < cellPossibilities.size(); i++)
        for(unsigned int row = 0; row < originalGrid.size(); row++)
            for(unsigned int col = 0; col < originalGrid.size(); col++)
                if(originalGrid[row][col] == cellPossibilities[i])
                    translatedGrid[row][col] = i;

    return translatedGrid;
}

void EvolutionManager::GenerateRules(bool basicRulesControl)
{
    Automaton::Rule helpRule;       // Pomocna pro aktualni sestavovane pravidlo
    double random;
    double helpValue;
    int suma;
    vector<int> randomNumbers;      // Pocet bunek, ktere idealne pripadaji na dany stav mrizky
    vector<double> randomDivision;  // Rozlozeni hustoty pravdepodobnosti vyskytu stavu (urceneho indexem)
    int divider = floor(cellCount / cellPossibilities.size() * 2);

    for(int cellNumber = 0; cellNumber < cellCount; cellNumber++)
    {   // Pro kazdou bunku
        do
        {   // Cyklus do-while jako ochrana proti nahodnemu nagenerovani samych nul (coz by melo za nasledek divide by zero)
            randomNumbers.clear();
            randomDivision.clear();
            suma = 0;
            helpValue = 0;

            for(unsigned int i = 0; i < cellPossibilities.size(); i++)
            {   // Kazdemu stavu je prirazen nahodne zvoleny pocet bunek mrizky (idealne je takto prirazeny pocet bunek pres vsechny stavy roven poctu bunek mrizky)
                random = rand() % divider;
                suma += random;
                randomNumbers.push_back(random);
            }
        } while(suma == 0);

        for(unsigned int i = 0; i < cellPossibilities.size(); i++)
        {   // Vytvoreni rozlozeni hustoty pravdepodobnosti
            helpValue += ((double)randomNumbers[i] / suma);
            randomDivision.push_back(helpValue);
        }

        helpRule.push_back(0);      // Z nezive bunky v kombinaci s nezivym okolim nemuze vzniknout ziva bunka
        for(int rulesNumber = 1; rulesNumber < rulesCount; rulesNumber++)
        {   // Vygenerovani jedne sady pravidel pro urcitou bunku
            random = ((double) rand() / RAND_MAX);

            if(basicRulesControl && basicRules[cellNumber][rulesNumber])
            {   // Dane pravidlo bunky patri mezi zakladni pravidla, proto nebude zmeneno
                helpRule.push_back(rules[cellNumber][rulesNumber]);
            }
            else
            {   // Dane pravidlo bunky nepatri mezi zakladni pravidla, proto bude zmeneno
                for(unsigned int i = 0; i < randomDivision.size(); i++)
                {
                    if(random <= randomDivision[i])
                    {   // Overujeme na hustote rozlozeni pravdepodobnosti
                        helpRule.push_back(i);  // Podle indexu pozname stav
                        break;
                    }
                }
            }
        }

        rules[cellNumber] = helpRule;
        helpRule.clear();
    }
}

void EvolutionManager::GenerateRule(int cellNumber, bool basicRulesControl)
{
    Automaton::Rule helpRule;       // Pomocna pro aktualni sestavovane pravidlo
    double random;
    double helpValue = 0;
    int suma = 0;
    vector<int> randomNumbers;      // Pocet bunek, ktere idealne pripadaji na dany stav mrizky
    vector<double> randomDivision;  // Rozlozeni hustoty pravdepodobnosti vyskytu stavu (urceneho indexem)
    int divider = floor(cellCount / cellPossibilities.size() * 2);

    for(unsigned int i = 0; i < cellPossibilities.size(); i++)
    {   // Kazdemu stavu je prirazen nahodne zvoleny pocet bunek mrizky (idealne je takto prirazeny pocet bunek pres vsechny stavy roven poctu bunek mrizky)
        random = rand() % divider;
        suma += random;
        randomNumbers.push_back(random);
    }
    for(unsigned int i = 0; i < cellPossibilities.size(); i++)
    {   // Vytvoreni rozlozeni hustoty pravdepodobnosti
        helpValue += ((double)randomNumbers[i] / suma);
        randomDivision.push_back(helpValue);
    }

    helpRule.push_back(0);      // Z nezive bunky v kombinaci s nezivym okolim nemuze vzniknout ziva bunka
    for(int rulesNumber = 1; rulesNumber < rulesCount; rulesNumber++)
    {   // Vygenerovani jedne sady pravidel pro urcitou bunku
        random = ((double) rand() / RAND_MAX);

        if(basicRulesControl && basicRules[cellNumber][rulesNumber])
        {   // Dane pravidlo bunky patri mezi zakladni pravidla, proto nebude zmeneno
            helpRule.push_back(rules[cellNumber][rulesNumber]);
        }
        else
        {   // Dane pravidlo bunky nepatri mezi zakladni pravidla, proto bude zmeneno
            for(unsigned int i = 0; i < randomDivision.size(); i++)
            {
                if(random < randomDivision[i])
                {   // Overujeme na hustote rozlozeni pravdepodobnosti
                    helpRule.push_back(i);  // Podle indexu pozname stav
                    break;
                }
            }
        }
    }
    rules[cellNumber] = helpRule;
}

int EvolutionManager::GetFitterNeighbours(int cellNumber, Automaton::RuleSet &fitterNeighbour)
{
    int actualCellFitness = fitness[cellNumber]; // Fitness aktualni bunky
    int row = (int)(cellNumber / size);          // Nastaveni cisla radku odpovidajiciho cislu bunky
    int col = cellNumber % size;                 // Nastaveni cisla sloupce odpovidajiciho cislu bunky
    vector<int> position;

    position.push_back((((row + size - 1) % size) * size) + col);                       // Horni soused
    position.push_back((row * size) + ((col + size - 1) % size));                       // Levy soused
    position.push_back((row * size) + ((col + 1) % size));                              // Pravy soused
    position.push_back((((row + 1) % size) * size) + col);                              // Dolni soused
    if(Config::GetInstance()->extendedRuleCalculation || Config::GetInstance()->neighbourhood == 8)
    {   // Nove pravidlo se bude vypocitavat z 9 okoli jen pokud je to povoleno v konfiguracnim souboru ci je-li nastavene 8-okoli
        position.push_back((((row + size - 1) % size) * size) + ((col+size-1)%size));   // Levy horni soused
        position.push_back((((row + size - 1) % size) * size) + ((col+1)%size));        // Pravy horni soused
        position.push_back((((row + 1) % size) * size) + ((col+size-1)%size));          // Levy dolni soused
        position.push_back((((row + 1) % size) * size) + ((col+1)%size));               // Pravy dolni soused
    }
    SortByFitness(position);   // Srovnani pozic ukazatelu do fitness podle fitness na danych pozicich

    for(unsigned int i = 0; i < position.size(); i++)
    {   // Pro kazdeho souseda
        if((fitness[position[i]] > actualCellFitness) && ((Config::GetInstance()->lockStepCount == -1) || (maxFitness[position[i]] < Config::GetInstance()->lockStepCount)))
        {   // Ma-li vybrany soused vetsi fitness nez aktualni bunka a dany soused nema zamknute pravidla, ulozime jeho pravidlovou tabulku souseda do prislusneho vektoru
            fitterNeighbour.push_back(rules[position[i]]);
        }
    }
    return fitterNeighbour.size();
}

void EvolutionManager::SortByFitness(vector<int> &position)
{
    vector<int> helpVector;
    int maxFitness, selectedPos;

    while(!position.empty())
    {
        maxFitness = 0;
        selectedPos = 0;

        for(unsigned int i = 0; i < position.size(); i++)
        {
            if(fitness[position[i]] > maxFitness)
            {
                maxFitness = fitness[position[i]];
                selectedPos = i;
            }
        }
        helpVector.push_back(position[selectedPos]);
        position.erase(position.begin() + selectedPos);
    }

    position = helpVector;
}

void EvolutionManager::MutateRule(Automaton::Rule &selectedRuleSet, double probability)
{
    int random;
    for(int i = 1; i < rulesCount; i++)    // Zaciname od pravidla c.1, protoze z nezive bunky v kombinaci s nezivym okolim nemuze vzniknout ziva bunka
    {
        if(((double)rand() / RAND_MAX) < probability)
        {
            do
            {
                random = rand() % cellPossibilities.size();
            }
            while(random == selectedRuleSet[i]);

            selectedRuleSet[i] = random;
        }
    }
}

void EvolutionManager::MutateRule(Automaton::Rule &selectedRuleSet, double probability, int cellNumber)
{
    int random;
    for(int rulesNumber = 1; rulesNumber < rulesCount; rulesNumber++)    // Zaciname od pravidla c.1, protoze z nezive bunky v kombinaci s nezivym okolim nemuze vzniknout ziva bunka
    {
        if((((double)rand() / RAND_MAX) < probability) && !basicRules[cellNumber][rulesNumber])
        {
            do
            {
                random = rand() % cellPossibilities.size();
            }
            while(random == selectedRuleSet[rulesNumber]);

            selectedRuleSet[rulesNumber] = random;
        }
    }
}

void EvolutionManager::MoveRule(Automaton::Rule &sourceRule, Automaton::Rule &destRule, int destCellNumber)
{
    for(unsigned int rulesNumber = 0; rulesNumber < sourceRule.size(); rulesNumber++)
        if(!basicRules[destCellNumber][rulesNumber])
            destRule[rulesNumber] = sourceRule[rulesNumber];
}

void EvolutionManager::CrossingRule(Automaton::Rule &selectedRuleSet, Automaton::Rule &selectedRuleSet2, Automaton::Rule &newRuleSet)
{
    int crossPoint = rand() % rulesCount;           // Vyber bodu krizeni
    for(int i = 0; i < crossPoint; i++)             // Prvni cast vysledneho pravidla
        newRuleSet.push_back(selectedRuleSet[i]);
    for(int i = crossPoint; i < rulesCount; i++)    // Druha cast vysledneho pravidla
        newRuleSet.push_back(selectedRuleSet2[i]);
}

void EvolutionManager::CrossingRule(Automaton::Rule &selectedRuleSet, Automaton::Rule &selectedRuleSet2, Automaton::Rule &newRuleSet, int cellNumber)
{
    int crossPoint = rand() % rulesCount;           // Vyber bodu krizeni

    for(int rulesNumber = 0; rulesNumber < crossPoint; rulesNumber++)  // Prvni cast vysledneho pravidla
        if(!basicRules[cellNumber][rulesNumber])
            newRuleSet[rulesNumber] = selectedRuleSet[rulesNumber];

    for(int rulesNumber = crossPoint; rulesNumber < rulesCount; rulesNumber++)  // Prvni cast vysledneho pravidla
        if(!basicRules[cellNumber][rulesNumber])
            newRuleSet[rulesNumber] = selectedRuleSet2[rulesNumber];
}

void EvolutionManager::StopEvoluteRules()
{
    if(sleepFlag)
    {   // Obnoveni prehravani
        mutex.lock();
        sleepFlag = false;
        condition.wakeOne();
        mutex.unlock();
    }
    else
    {   // Pozastaveni prehravani
        mutex.lock();
        sleepFlag = true;
        mutex.unlock();
    }
}

void EvolutionManager::AbortEvoluteRules()
{
    mutex.lock();
    abortFlag = true;      // Nastaveni priznaku pro ukonceni
    condition.wakeOne();   // Probuzeni spiciho vlakna
    mutex.unlock();

    wait();                // Pockani na dobehnuti vlakna
}

EvolutionManager::~EvolutionManager()
{
    mutex.lock();
    abortFlag = true;      // Nastaveni priznaku pro ukonceni
    condition.wakeOne();   // Probuzeni spiciho vlakna
    mutex.unlock();

    wait();                // Pockani na dobehnuti vlakna
}

void EvolutionManager::CheckConfigurations(int configurationCount)
{
    for(int i = 0; i < cellCount; i++)
    {
        if(fitness[i] != configurationCount)
        {   // Nektery z automatu populace ma bunku v necilovem stavu
            maxFitness[i] = 0;
        }
        else
        {
            maxFitness[i]++;
        }
    }
}

void EvolutionManager::SolveLongtermUnsuccess(int evolutionStep, int configurationCount, bool basicRulesControl)
{
    // Vygenerovani noveho pravidla na misto dlouhodobe neuspesne bunky
    if(evolutionStep > initialStep)
    {   // Reset bunky prichazi v uvahu az po uvodni fazi
        for(int i = 0; i < cellCount; i++)
        {
            fitnessSum[i] += fitness[i];
            if(((evolutionStep % Config::GetInstance()->fitnessMeasuringSequence) == 0) && (evolutionStep != 0))
            {   // Pokud je dosazeno pozadovaneho poctu cyklu
                if(fitnessSum[i] < floor(Config::GetInstance()->resetCellPercentageLimit * Config::GetInstance()->fitnessMeasuringSequence * configurationCount))
                {   // Pokud bylo pravidlo neuspesne, vygeneruje se na jeho misto nove pravidlo
                    GenerateRule(i, basicRulesControl);
                }
                fitnessSum[i] = 0;
            }
        }
    }
}

bool EvolutionManager::IsRecommendResetGrid(int evolutionStep, int configurationCount)
{
    int correctCellValue = 0;

    for(int i = 0; i < cellCount; i++)
    {   // Soucet fitness vsech bunek mrizky
        correctCellValue += fitness[i];
    }

    if((((double)correctCellValue / (cellCount * configurationCount)) < Config::GetInstance()->resetGridPercentageLimit) && (evolutionStep > initialStep))
    {   // Reset pravidel na cele mrizce
        return true;
    }

    return false;
}

void EvolutionManager::GetActualResults(Automaton::RuleSet &basicRules, Automaton::RuleSet &generalRules, double &basicRulesFitness, double &generalRulesFitness, time_t &timeSummary)
{
    basicRules = this->bestBasicRules;
    generalRules = this->bestGeneralRules;
    basicRulesFitness = this->bestBasicRulesFitness;
    generalRulesFitness = this->bestGeneralRulesFitness;
    timeSummary = this->timeSummary;
}
