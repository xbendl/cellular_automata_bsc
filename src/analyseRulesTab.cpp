#include "analyseRulesTab.h"

using namespace std;

AnalyseRulesTab::AnalyseRulesTab(QWidget *parent) : QWidget(parent)
{
    // Sekce "Mrizka automatu"
    palette = new Palette(this, 200, 50);
    palette->move(120, 192);
    QLabel *basicRulesAutomatonHeader = new QLabel(tr("Automat se zakladnimi pravidly:"), this);
    basicRulesAutomatonHeader->setFont(QFont("Helvetica", 12, QFont::Bold));
    basicRulesAutomatonHeader->move(340, 10);
    QLabel *generalRulesAutomatonHeader = new QLabel(tr("Automat s pravidly pro sebeopravu:"), this);
    generalRulesAutomatonHeader->setFont(QFont("Helvetica", 12, QFont::Bold));
    generalRulesAutomatonHeader->move(645, 10);
    basicRulesGridWidget = new GridWidget(this, 290, palette);
    basicRulesGridWidget->setGeometry(340, 40, 290, 290);
    generalRulesGridWidget = new GridWidget(this, 290, palette);
    generalRulesGridWidget->setGeometry(645, 40, 290, 290);
    actualStepLabel = new QLabel("Aktualni krok: 0", this);
    actualStepLabel->setGeometry(340, 365, 250, 30);
    basicRulesGridHammingDistanceLabel = new QLabel("Diference: -", this);
    basicRulesGridHammingDistanceLabel->setGeometry(340, 335, 250, 30);
    generalRulesGridHammingDistanceLabel = new QLabel("Diference: -", this);
    generalRulesGridHammingDistanceLabel->setGeometry(645, 335, 250, 30);

    // Sekce "Analyza pravidel"
    QLabel *analyseRuleHeaderLabel = new QLabel(tr("Analyza pravidel:"), this);
    analyseRuleHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    analyseRuleHeaderLabel->move(10, 10);
    QLabel *rulesLabel = new QLabel(tr("Pravidla"), this);
    rulesLabel->move(10, 45);
    loadRulesButton = new QPushButton(tr("Nacist pravidla"), this);
    loadRulesButton->setGeometry(120, 40, 205, 25);
    resetButton = new QPushButton(tr("Reset ulohy"), this);
    resetButton->setGeometry(120, 70, 205, 25);
    resetButton->setEnabled(false);
    QLabel *automatonStepLabel = new QLabel(tr("Pocet kroku"), this);
    automatonStepLabel->move(10, 105);
    automatonStepLine = new QSpinBox(this);
    automatonStepLine->setRange(1, (INT_MAX - 1));
    automatonStepLine->setGeometry(120, 100, 65, 25);
    automatonStepButton = new QPushButton(tr("Provest kroky"), this);
    automatonStepButton->setGeometry(190, 100, 135, 25);
    automatonStepButton->setEnabled(false);
    QLabel *steppingLabel = new QLabel(tr("Krokovani"), this);
    steppingLabel->move(10, 135);
    steppingLabel->setToolTip("Casovy interval mezi dvema prechody automatu; hodnota v ms");
    steppingLabel->setCursor(Qt::WhatsThisCursor);
    steppingLine = new QSpinBox(this);
    steppingLine->setRange(1, (INT_MAX - 1));
    steppingLine->setGeometry(120, 130, 65, 25);
    steppingButton = new QPushButton(tr("Spustit krokovani"), this);
    steppingButton->setGeometry(190, 130, 135, 25);
    steppingButton->setEnabled(false);
    QLabel *autoDamageLabel = new QLabel(tr("Auto poskozeni"), this);
    autoDamageLabel->setToolTip("Generovani automatickeho poskozeni v kazdem evolucnim kroku; hodnota z intervalu <0;1>");
    autoDamageLabel->setCursor(Qt::WhatsThisCursor);
    autoDamageLabel->move(10, 165);
    autoDamageLine = new QDoubleSpinBox(this);
    autoDamageLine->setRange(0, 1);
    autoDamageLine->setDecimals(3);
    autoDamageLine->setSingleStep(0.001);
    autoDamageLine->setGeometry(120, 160, 65, 25);
    damageCheckBox = new QCheckBox(this);
    damageCheckBox->setGeometry(190, 160, 18, 25);
    QLabel *manualDamageLabel = new QLabel(tr("Manual poskozeni"), this);
    manualDamageLabel->setToolTip("Manualniho nastaveni poskozeni pomoci barev z palety");
    manualDamageLabel->setCursor(Qt::WhatsThisCursor);
    manualDamageLabel->move(10, 195);
    saveResultsButton = new QPushButton(tr("Ulozit"), this);
    saveResultsButton->setGeometry(120, 220, 205, 25);
    saveResultsButton->setEnabled(false);

    // Nastaveni slotu a signalu
    connect(loadRulesButton, SIGNAL(clicked()), this, SLOT(SetOpenRulesFile()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(ResetTask()));
    connect(automatonStepButton, SIGNAL(clicked()), this, SLOT(ExecuteSteps()));
    connect(steppingButton, SIGNAL(clicked()), this, SLOT(ExecuteStepping()));
    connect(saveResultsButton, SIGNAL(clicked()), this, SLOT(SetSaveResultsFile()));
    connect(&analyseManager, SIGNAL(GridGenerated(Automaton::GridType, Automaton::GridType, int, int, bool)), this, SLOT(ShowGridGenerated(Automaton::GridType, Automaton::GridType, int, int, bool)));

    // Vyplneni ovladacich prvku podle nastaveni z konfiguracniho souboru
    SetConfigurationValues();

    // Nacteni hledaneho vzoru do mrizky
    if(!(Config::GetInstance()->inputAnalyseFile).empty())
    {
        string taskFile;
        if(LoadRules(Config::GetInstance()->inputAnalyseFile, taskFile) && LoadTask(taskFile))
        {
            automatonStepButton->setEnabled(true);
            steppingButton->setEnabled(true);
            saveResultsButton->setEnabled(true);
            resetButton->setEnabled(true);
            generalRulesGridWidget->SetGrid(zygote);
            basicRulesGridWidget->SetGrid(zygote);
            actualStepLabel->setText("Aktualni krok: 0");
            basicRulesHammingDistanceStatistic.push_back(Automaton::HammingDistance(zygote, pattern));
            generalRulesHammingDistanceStatistic.push_back(Automaton::HammingDistance(zygote, pattern));
            basicRulesGridHammingDistanceLabel->setText(QString("Diference: %1").arg(basicRulesHammingDistanceStatistic.back()));
            generalRulesGridHammingDistanceLabel->setText(QString("Diference: %1").arg(generalRulesHammingDistanceStatistic.back()));

            // Spusteni sledovani
            analyseManager.ObserveAfterDamageRule(zygote, pattern, basicRules, generalRules, cellPossibilities);
        }
        else
        {
            std::cerr << "CHYBA: " << "Vstupni soubor nelze nalezt nebo neobsahuje pozadovane udaje." << std::endl;
        }
    }

    timerRunning = false;
    actualStep = 0;
    timerId = -1;
}

bool AnalyseRulesTab::LoadRules(std::string inputFile, std::string &taskFile)
{
    // Otevreni souboru
    ifstream inFile;
    inFile.open(inputFile.c_str(), std::ifstream::in);
    if(inFile.fail())
    {   // Vstupni soubor se nepodarilo otevrit
        return false;
    }

    // Ziskani oznaceni souboru
    string line;
    string fileContent = "*** CELULARNI AUTOMAT V DYNAMICKEM PROSTREDI - PRAVIDLA ***";
    getline(inFile, line);
    if(line.substr(0, fileContent.size()) != fileContent)
    {   // Soubor neobsahuje korektni oznaceni
        return false;
    }
    getline(inFile, line);  // Preskoceni prazneho radku

    // Presun v souboru k casti s ulozenymi zakladnimi pravidly
    bool foundRulesSection = false;  // Udava, zda byla ve vstupnim souboru nalezena cast s ulozenymi pravidly
    while(getline(inFile, line))
    {
        if(line.find_first_of('\r') != string::npos)
        {   // Odmazani \r z textu radku souboru
            line = line.substr(0, line.find_first_of('\r'));
        }
        if((line.find_first_of(':') != string::npos) && (line.substr(0, line.find_first_of(':')) == "Vstupni soubor"))
        {   // Ziskani cesty k souboru se specifikaci ulohy (zygota, hledany vzor)
            taskFile = line.substr(line.find_first_of(':') + 1);             // Ziskani casti radku za dvojteckou
            taskFile = taskFile.substr(taskFile.find_first_not_of(" \t"));   // Orezani bilych znaku
        }
        if(line == "[ZAKLADNI PRAVIDLA]")
        {
            foundRulesSection = true;
            break;
        }
    }
    if(!foundRulesSection)
    {   // Vstupni soubor nema cast s ulozenymi pravidly
        return false;
    }

    // Ziskani zakladnich pravidel pro vyvoj ze zygoty do hledaneho vzoru
    basicRules.clear();
    Automaton::Rule actualRule;
    while(getline(inFile, line))
    {
        if(line.find_first_of('\r') != string::npos)          // Odmazani \r z textu radku souboru
            line = line.substr(0, line.find_first_of('\r'));
        if(line.size() == 0)                                  // Ukonceni v pripade, ze je radek prazdny
            break;

        actualRule.clear();
        for(unsigned int i = 0; i < line.size(); i++)         // Precteni jednoho pravidla
            actualRule.push_back(line[i] - '0');

        basicRules.push_back(actualRule);                          // Ulozeni precteneho pravidla
    }

    // Presun v souboru k casti s pravidly umoznujici sebeopravu
    foundRulesSection = false;
    while(getline(inFile, line))
    {
        if(line.find_first_of('\r') != string::npos)
        {   // Odmazani \r z textu radku souboru
            line = line.substr(0, line.find_first_of('\r'));
        }
        if(line == "[PRAVIDLA UMOZNUJICI SEBEOPRAVU]")
        {
            foundRulesSection = true;
            break;
        }
    }
    if(!foundRulesSection)
    {   // Vstupni soubor nema cast s ulozenymi pravidly
        return false;
    }

    // Ziskani pravidel schopnych sebeopravy
    generalRules.clear();
    while(getline(inFile, line))
    {
        if(line.find_first_of('\r') != string::npos)          // Odmazani \r z textu radku souboru
            line = line.substr(0, line.find_first_of('\r'));
        if(line.size() == 0)                                  // Ukonceni v pripade, ze je radek prazdny
            break;

        actualRule.clear();
        for(unsigned int i = 0; i < line.size(); i++)         // Precteni jednoho pravidla
            actualRule.push_back(line[i] - '0');

        generalRules.push_back(actualRule);                   // Ulozeni precteneho pravidla
    }

    return true;
}

bool AnalyseRulesTab::LoadTask(std::string taskFile)
{
    zygote.clear();
    pattern.clear();
    cellPossibilities.clear();

    // Otevreni souboru
    ifstream inFile;
    inFile.open(taskFile.c_str(), std::ifstream::in);
    if(inFile.fail())
    {   // Vstupni soubor se nepodarilo otevrit
        return false;
    }

    // Ziskani oznaceni souboru
    string line;
    string fileContent = "*** CELULARNI AUTOMAT V DYNAMICKEM PROSTREDI - ULOHA ***";
    getline(inFile, line);
    if(line.substr(0, fileContent.size()) != fileContent)
    {   // Soubor neobsahuje korektni oznaceni
        return false;
    }
    getline(inFile, line);  // Preskoceni prazneho radku

    // Ziskani rozmeru mrizky (udaj na prvnim radku souboru)
    int size;
    if(getline(inFile, line))
        size = atoi(line.c_str());

    // Ziskani moznych stavu bunky
    getline(inFile, line);
    int possibility;
    while(line.find_first_of(';') != string::npos)
    {
        possibility = atoi((line.substr(0, line.find_first_of(';'))).c_str());
        cellPossibilities.push_back(possibility);
        line.erase(0, line.find_first_of(';') + 1);
    }
    palette->SetPalette(cellPossibilities);    // Nastaveni barev reprezentujici mozne stavy do palety

    // Ziskani zygoty
    Automaton::CellVector helpVector;  // Pro docasne ulozeni jednoho radku
    int helpInt;
    for(int i = 0; i < size; i++)
    {   // Postupne nacitani radku
        getline(inFile, line);
        helpVector.clear();
        for(int j = 0; j < size; j++)
        {
            helpInt = atoi(line.substr(j, 1).c_str());
            helpVector.push_back(helpInt);
        }
        zygote.push_back(helpVector);
    }

    getline(inFile, line);  // Preskoceni prazdneho radku oddelujiciho zygotu od vzoru

    // Ziskani hledaneho vzoru
    for(int i = 0; i < size; i++)
    {   // Postupne nacitani radku
        getline(inFile, line);
        helpVector.clear();
        for(int j = 0; j < size; j++)
        {
            helpInt = atoi(line.substr(j, 1).c_str());
            helpVector.push_back(helpInt);
        }
        pattern.push_back(helpVector);
    }

    inFile.close();

    return true;
}

bool AnalyseRulesTab::SaveResults(std::string outputFile)
{
    // Zjisteni prumerne Hammingovy vzdalenosti u zakladnich pravidel
    double basicRulesAverageHammingDistance = 0;
    for(unsigned int i = 0; i < basicRulesHammingDistanceStatistic.size(); i++)
        basicRulesAverageHammingDistance += basicRulesHammingDistanceStatistic[i];
    basicRulesAverageHammingDistance /= basicRulesHammingDistanceStatistic.size();

    // Zjisteni prumerne Hammingovy vzdalenosti u pravidel pro sebeopravu
    double generalRulesAverageHammingDistance = 0;
    for(unsigned int i = 0; i < generalRulesHammingDistanceStatistic.size(); i++)
        generalRulesAverageHammingDistance += generalRulesHammingDistanceStatistic[i];
    generalRulesAverageHammingDistance /= generalRulesHammingDistanceStatistic.size();

    std::stringstream stepsCountOut, basicRulesAverageHammingDistanceOut, generalRulesAverageHammingDistanceOut;
    stepsCountOut << basicRulesHammingDistanceStatistic.size();
    basicRulesAverageHammingDistanceOut << basicRulesAverageHammingDistance;
    generalRulesAverageHammingDistanceOut << generalRulesAverageHammingDistance;

    string fileReport;
    fileReport = "*** CELULARNI AUTOMAT V DYNAMICKEM PROSTREDI - VYSLEDKY ANALYZY ***\r\n\r\n";
    fileReport +=  "[SOUBORY]\r\n";
    fileReport += "Vstupni soubor: \t" + Config::GetInstance()->inputAnalyseFile + "\r\n";
    fileReport += "\r\n[VYSLEDKY SIMULACE]\r\n";
    fileReport += "Pocet kroku: \t" + stepsCountOut.str() + "\r\n";
    //fileReport += "Prumerna diference pri pouziti zakladnich pravidel: \t" + basicRulesAverageHammingDistanceOut.str() + "\r\n";
    //fileReport += "Prumerna diference pri pouziti pravidel pro sebeopravu: \t" + generalRulesAverageHammingDistanceOut.str() + "\r\n";

    // Prumerna diference pri pouziti zakladnich pravidel
    fileReport += "\r\n[DIFERENCE / ZAKLADNI PRAVIDLA]\r\n";
    for(unsigned int i = 0; i < basicRulesHammingDistanceStatistic.size(); i++)
    {
        std::stringstream out;
        out << ((int) basicRulesHammingDistanceStatistic[i]);
        fileReport += out.str() + "\r\n";
    }

    // Prumerna diference pri pouziti zakladnich pravidel
    fileReport += "\r\n[DIFERENCE / PRAVIDLA PRO SEBEOPRAVU]\r\n";
    for(unsigned int i = 0; i < generalRulesHammingDistanceStatistic.size(); i++)
    {
        std::stringstream out;
        out << ((int) generalRulesHammingDistanceStatistic[i]);
        fileReport += out.str() + "\r\n";
    }

    std::ofstream outputFileStream(outputFile.c_str(), std::ios::out);
    std::copy(fileReport.begin(), fileReport.end(), std::ostream_iterator<char>(outputFileStream, ""));  // Zapis do souboru

    return true;
}

//////////////////// SLOTY ////////////////////

void AnalyseRulesTab::SetConfigurationValues()
{
    automatonStepLine->setValue(Config::GetInstance()->defaultStepInterval);
    steppingLine->setValue(Config::GetInstance()->timeIntervalAnalyseRules);
    autoDamageLine->setValue(Config::GetInstance()->defaultDamage);
}

void AnalyseRulesTab::SetOpenRulesFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Vyberte soubor s ulozenymi pravidly"), "", tr("Vsechny soubory (*);;Textove soubory (*.txt)"));

    if(!fileName.isEmpty())
    {
        Config::GetInstance()->inputAnalyseFile = fileName.toStdString();
        string taskFile;

        if(LoadRules(Config::GetInstance()->inputAnalyseFile, taskFile) && LoadTask(taskFile))  // Nacteni ulozenych pravidel z precteneho souboru
        {
            automatonStepButton->setEnabled(true);
            steppingButton->setEnabled(true);
            saveResultsButton->setEnabled(true);
            resetButton->setEnabled(true);
            generalRulesGridWidget->SetGrid(zygote);
            actualStepLabel->setText("Aktualni krok: 0");
            basicRulesGridWidget->SetGrid(zygote);
            basicRulesHammingDistanceStatistic.clear();    // Vynulovani statistik k Hamminove vzdalenosti
            generalRulesHammingDistanceStatistic.clear();
            actualStep = 0;
            basicRulesHammingDistanceStatistic.push_back(Automaton::HammingDistance(zygote, pattern));
            generalRulesHammingDistanceStatistic.push_back(Automaton::HammingDistance(zygote, pattern));
            basicRulesGridHammingDistanceLabel->setText(QString("Diference: %1").arg(basicRulesHammingDistanceStatistic.back()));
            generalRulesGridHammingDistanceLabel->setText(QString("Diference: %1").arg(generalRulesHammingDistanceStatistic.back()));

            // Spusteni sledovani
            analyseManager.ObserveAfterDamageRule(zygote, pattern, basicRules, generalRules, cellPossibilities);
        }
        else
        {
            QMessageBox *error = new QMessageBox(QMessageBox::Critical, tr("Chyba"), tr("Vstupni soubor nelze nalezt nebo neobsahuje pozadovane udaje."), QMessageBox::Ok, this, Qt::Widget);
            error->show();
        }
    }
}

void AnalyseRulesTab::SetSaveResultsFile()
{
    std::string outputFile= (QFileDialog::getSaveFileName(this, tr("Vyberte soubor pro ulozeni"), "", tr("Vsechny soubory (*);;Textove soubory (*.txt)"))).toStdString();

    if(!outputFile.empty())
    {
        if(!SaveResults(outputFile))
        {
            QMessageBox *error = new QMessageBox(QMessageBox::Critical, tr("Chyba"), tr("Ulozeni vysledku do vystupniho souboru se nezdarilo."), QMessageBox::Ok, this, Qt::Widget);
            error->show();
        }
    }
}

void AnalyseRulesTab::ResetTask()
{
    actualStep = 0;
    actualStepLabel->setText("Aktualni krok: 0");
    automatonStepButton->setEnabled(true);      // Odemknuti ovladacich prvku
    steppingButton->setEnabled(true);
    saveResultsButton->setEnabled(true);
    resetButton->setEnabled(true);
    generalRulesGridWidget->SetGrid(zygote);         // Nastaveni zygoty do obou mrizek
    basicRulesGridWidget->SetGrid(zygote);
    basicRulesHammingDistanceStatistic.clear();
    generalRulesHammingDistanceStatistic.clear();
    basicRulesHammingDistanceStatistic.push_back(Automaton::HammingDistance(zygote, pattern));
    generalRulesHammingDistanceStatistic.push_back(Automaton::HammingDistance(zygote, pattern));
    basicRulesGridHammingDistanceLabel->setText(QString("Diference: %1").arg(basicRulesHammingDistanceStatistic.back()));
    generalRulesGridHammingDistanceLabel->setText(QString("Diference: %1").arg(generalRulesHammingDistanceStatistic.back()));
}

void AnalyseRulesTab::ShowGridGenerated(Automaton::GridType basicRulesAutomatonGrid, Automaton::GridType generalRulesAutomatonGrid, int basicRulesGridHammingDistance, int generalRulesGridHammingDistance, bool lastGrid)
{
    basicRulesGridWidget->SetGrid(basicRulesAutomatonGrid);
    generalRulesGridWidget->SetGrid(generalRulesAutomatonGrid);

    // Zmena ovladacich prvku v zavislosti na tom, zda byl vykonan pouze jeden bezny prechod, nebo posledni prechod
    if((lastGrid == true) && !timerRunning)
    {
        loadRulesButton->setEnabled(true);
        automatonStepButton->setEnabled(true);
        loadRulesButton->setEnabled(true);
        steppingButton->setEnabled(true);
        saveResultsButton->setEnabled(true);
        resetButton->setEnabled(true);
    }

    basicRulesHammingDistanceStatistic.push_back(basicRulesGridHammingDistance);     // Ulozeni Hammingovy vzdalenosti
    generalRulesHammingDistanceStatistic.push_back(generalRulesGridHammingDistance); // Ulozeni Hammingovy vzdalenosti
    basicRulesGridHammingDistanceLabel->setText(QString("Diference: %1").arg(basicRulesGridHammingDistance));
    generalRulesGridHammingDistanceLabel->setText(QString("Diference: %1").arg(generalRulesGridHammingDistance));

    // Zobrazeni cisla aktualniho kroku
    actualStep++;
    actualStepLabel->setText(QString("Aktualni krok: %1").arg(actualStep));
    update();

    // Kontrola, zda-li jiz oba automaty dosahli pozadovaneho vzoru
    if((basicRulesHammingDistanceStatistic.back() == 0) && (generalRulesHammingDistanceStatistic.back() == 0))
        if(timerId > 0)
            ExecuteStepping();
}

void AnalyseRulesTab::ExecuteSteps()
{
    // Zamknuti prislusnych ovladacich prvku
    loadRulesButton->setEnabled(false);
    automatonStepButton->setEnabled(false);
    loadRulesButton->setEnabled(false);
    steppingButton->setEnabled(false);
    saveResultsButton->setEnabled(false);
    resetButton->setEnabled(false);
    this->setFocus();

    // Kontrola vyplneni povinnych polozek
    if((automatonStepLine->text()).isEmpty())
    {
        QMessageBox *warning = new QMessageBox(QMessageBox::Warning, tr("Upozorneni"), tr("Prosim vyplnte pocet kroku."), QMessageBox::Ok, this, Qt::Widget);
        warning->show();
        return;
    }

    // Zjisteni miry pozadovaneho poskozeni
    double damageProbability = 0;
    if(damageCheckBox->isChecked())
        damageProbability = autoDamageLine->value();

    analyseManager.SetCycles(basicRulesGridWidget->GetGrid(), generalRulesGridWidget->GetGrid(), (automatonStepLine->text()).toInt(), damageProbability);
}

void AnalyseRulesTab::ExecuteStepping()
{
    if(timerRunning)
    {   // Vypnuti krokovani
        steppingButton->setText("Spustit krokovani");
        timerRunning = false;
        killTimer(timerId);
        timerId = -1;

        // Odemknuti ovladacich prvku
        loadRulesButton->setEnabled(true);
        automatonStepButton->setEnabled(true);
        loadRulesButton->setEnabled(true);
        saveResultsButton->setEnabled(true);
        resetButton->setEnabled(true);
    }
    else
    {   // Zapnuti krokovani
        steppingButton->setText("Zastavit krokovani");
        timerRunning = true;

        // Zamknuti ovladacich prvku
        loadRulesButton->setEnabled(false);
        automatonStepButton->setEnabled(false);
        loadRulesButton->setEnabled(false);
        saveResultsButton->setEnabled(false);
        resetButton->setEnabled(false);

        timerId = startTimer((steppingLine->text()).toInt());
    }

    update();
}

void AnalyseRulesTab::timerEvent(QTimerEvent *)
{
    double damageProbability = 0;
    if(damageCheckBox->isChecked())
        damageProbability = autoDamageLine->value();

    analyseManager.SetCycles(basicRulesGridWidget->GetGrid(), generalRulesGridWidget->GetGrid(), 1, damageProbability);
}
