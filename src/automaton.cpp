#include "automaton.h"

using namespace std;

Automaton::Automaton(GridType &zygote, GridType &pattern, RuleSet &rules, int cellPossibilitiesCount, int neighbourhood)
{
    this->grid = zygote;
    this->pattern = pattern;
    this->rules = rules;
    this->size = zygote.size();
    this->cellCount = size * size;
    this->cellPossibilitiesCount = cellPossibilitiesCount;
    this->neighbourhood = neighbourhood;
}

Automaton::Automaton(GridType &zygote, RuleSet &rules, int cellPossibilitiesCount, int neighbourhood)
{
    this->grid = zygote;
    this->rules = rules;
    this->size = zygote.size();
    this->cellCount = size * size;
    this->cellPossibilitiesCount = cellPossibilitiesCount;
    this->neighbourhood = neighbourhood;
}

void Automaton::AutomatonStep(int stepCount)
{
    int cellNumber;       // Cislo aktualni bunky

    // Docasna mrizka pro ulozeni zmen jednotlivych bunek (pro zajisteni paralelismu)
    GridType newGrid;
    CellVector emptyCellVector;
    emptyCellVector.assign(size, 0);
    newGrid.assign(size, emptyCellVector);

    for(int i = 0; i < stepCount; i++)
    {   // Pro kazdy krok
        for(int row = 0; row < this->size; row++)
        {
            for(int col = 0; col < this->size; col++)
            {
                cellNumber = row * size + col;
                SetNewConfiguration(cellNumber, newGrid);    // Ziska vystupni konfiguraci pro aktualni bunky
            }
        }
        grid = newGrid;  // Vymena mrizky
    }
}

void Automaton::AutomatonStep(int stepCount, double damageProbability)
{
    // Provedeni prechodu
    AutomatonStep(stepCount);

    // Vliv "sumu" - nastaveni jine hodnoty pro bunku (s urcitou pravdepodobnosti)
    double randomProbability;
    int newCellValue;

    for(int row = 0; row < size; row++)
    {
        for(int col = 0; col < size; col++)
        {
            randomProbability = ((double)rand() / RAND_MAX);
            if((randomProbability < damageProbability) && (grid[row][col] != 0))
            {
                do  // Vygenerovani jine hodnoty pro bunku
                {
                    newCellValue = rand() % cellPossibilitiesCount;
                }
                while(newCellValue == grid[row][col]);

                grid[row][col] = newCellValue;
            }
        }
    }
}

double Automaton::AutomatonStep(int stepCount, CellFitness &fitness, std::vector<std::vector<bool> > *basicRules)
{
    // Vynulovani vektoru pro oznaceni zakladnich pravidel automatu
    if(basicRules != NULL)
    {
        for(unsigned int i = 0; i < (*basicRules).size(); i++)
            for(unsigned int j = 0; j < (*basicRules)[i].size(); j++)
                (*basicRules)[i][j] = false;
    }

    double actFitness = 0;
    int cellNumber;    // Cislo aktualni bunky
    int rulesNumber;   // Cislo pouziteho pravidla pri prechodu

    // Docasna mrizka pro ulozeni zmen jednotlivych bunek (pro zajisteni paralelismu)
    GridType newGrid;
    CellVector emptyCellVector;
    emptyCellVector.assign(size, (CellType) 0);
    newGrid.assign(size, emptyCellVector);

    for(int i = 0; i < stepCount; i++)
    {   // Pro kazdy krok
        for(int row = 0; row < this->size; row++)
        {
            for(int col = 0; col < this->size; col++)
            {
                cellNumber = row * size + col;
                rulesNumber = SetNewConfiguration(cellNumber, newGrid);    // Ziska vystupni konfiguraci pro aktualni bunky

                if(basicRules != NULL)
                {   // Oznaceni pouziteho pravidla
                    (*basicRules)[cellNumber][rulesNumber] = true;
                }

                if((stepCount > 1) && (i == (stepCount - 1)) && (grid[row][col] == pattern[row][col]) && (newGrid[row][col] == pattern[row][col]))
                {   // V poslednim kroku se nastavi fitness jednotlivych bunek
                    fitness[row*size+col]++;
                    actFitness++;
                }
            }
        }
        grid = newGrid;  // Vymena mrizky
    }

    actFitness = actFitness / cellCount;  // Normalizace do intervalu <0;1>

    return actFitness;
}

int Automaton::HammingDistance(GridType grid1, GridType grid2)
{
    // Kontrola stejne velikosti automatu
    if(grid1.size() != grid2.size())
        return -1;

    // Porovnavani jednotlivych bunek
    int hammingDistance = 0;
    for(unsigned int row = 0; row < grid1.size(); row++)
        for(unsigned int col = 0; col < grid1[0].size(); col++)
            if(grid1[row][col] != grid2[row][col])
                hammingDistance++;

    return hammingDistance;
}

int Automaton::SetNewConfiguration(int cellNumber, GridType &newGrid)
{
    int actConfig[neighbourhood+1];  // Pro ulozeni konfigurace aktualni bunky
    int row = (int)(cellNumber / size);   // Nastaveni cisla radku odpovidajiciho cislu bunky
    int col = cellNumber % size;          // Nastaveni cisla sloupce odpovidajiciho cislu bunky

    // Ziskani vstupni konfigurace aktualni bunky
    actConfig[0] = grid[row][col];                 // Centralni bunka
    actConfig[1] = grid[(row+size-1)%size][col];   // Horni soused
    actConfig[2] = grid[row][(col+size-1)%size];   // Levy soused
    actConfig[3] = grid[row][(col+1)%size];        // Pravy soused
    actConfig[4] = grid[(row+1)%size][col];        // Dolni soused
    if(neighbourhood == 8)
    {   // Pri pouziti Moorova okoli (8 sousedu + centralni bunka) musime vzit v potaz i sousedy na diagonalach
        actConfig[5] = grid[(row+size-1)%size][(col+size-1)%size];  // Levy horni soused
        actConfig[6] = grid[(row+size-1)%size][(col+1)%size];       // Pravy horni soused
        actConfig[7] = grid[(row + 1) % size][(col+size-1)%size];   // Levy dolni soused
        actConfig[8] = grid[(row + 1) % size][(col+1)%size];        // Pravy dolni soused
    }

    // Prevod konfigurace do desitkove soustavy
    int decimalActConfig = ConvertToDecimalSystem(actConfig, neighbourhood+1, cellPossibilitiesCount);

    // Nastaveni stavu centralni bunky podle odpovidajiciho pravidla
    newGrid[row][col] = rules[cellNumber][decimalActConfig];

    return decimalActConfig;
}

void Automaton::PrintGrid()
{
    for(int i = 0; i < this->size; i++)
        cout << "__";
    cout << "__" << endl;

    for(int row = 0; row < this->size; row++)
    {
        cout << "|";
        for(int col = 0; col < this->size; col++)
        {
            cout << (int)grid[row][col] << " ";
        }
        cout << "|";
        cout << endl;
    }

    for(int i = 0; i < this->size; i++)
        cout << "__";
    cout << "__" << endl;
}

int Automaton::ConvertToDecimalSystem(int actConfig[], int length, int systemBase)
{
    int result = 0;

    for(int i = 0; i < length; i++)
        result += actConfig[length-i-1] * pow((double)systemBase, (double)i);

    return result;
}

void Automaton::ConvertToBaseNSystem(int decimalActConfig, int actConfig[], int length, int systemBase)
{
    for(int i = 0; i < length; i++)
    {
        actConfig[length-1-i] = decimalActConfig % systemBase;
        decimalActConfig = (int) (decimalActConfig / systemBase);
    }
}

void Automaton::SetGrid(GridType grid)
{
    this->grid = grid;
}
