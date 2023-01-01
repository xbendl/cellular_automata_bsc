#include "evolutionRulesTab.h"

using namespace std;

EvolutionRulesTab::EvolutionRulesTab(QWidget *parent) : QWidget(parent)
{
    // Sekce "Mrizka automatu"
    QLabel *zygoteHeaderLabel = new QLabel(tr("Zygota:"), this);
    zygoteHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    zygoteHeaderLabel->move(360, 10);
    zygoteGridWidget = new GridWidget(this, 160, false);
    zygoteGridWidget->setGeometry(355, 40, 160, 160);
    QLabel *patternHeaderLabel = new QLabel(tr("Hledany vzor:"), this);
    patternHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    patternHeaderLabel->move(355, 220);
    patternGridWidget = new GridWidget(this, 160, false);
    patternGridWidget->setGeometry(355, 250, 160, 160);
    evoluteRulesBar = new QProgressBar(this);
    evoluteRulesBar->setGeometry(355, 433, 580, 25);
    evoluteRulesBar->setMinimum(0);
    evoluteRulesBar->setTextVisible(false);
    QLabel *evolutionPlotHeaderLabel = new QLabel(tr("Graf vyvoje fitness:"), this);
    evolutionPlotHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    evolutionPlotHeaderLabel->move(592, 10);
    evolutionPlot = new EvolutionPlot(this, &evolutionManager);
    evolutionPlot->move(535, 40);
    evolutionPlot->resize(400, 370);
    evolutionPlot->show();
    projectStatus = new QLabel(tr("Evoluce zakladnich pravidel"), this);
    projectStatus->setStyleSheet("font-weight: bold");
    projectStatus->setGeometry(540, 438, 300, 18);

    // Sekce "Evoluce pravidel"
    QLabel *evolutionHeaderLabel = new QLabel(tr("Evoluce pravidel:"), this);
    evolutionHeaderLabel->setFont(QFont("Helvetica", 12, QFont::Bold));
    evolutionHeaderLabel->move(10, 10);
    QLabel *taskChooseLaber = new QLabel(tr("Uloha *"), this);
    taskChooseLaber->move(10, 45);
    QLabel *neighbourhoodLabel = new QLabel(tr("Okoli *"), this);
    neighbourhoodLabel->move(10, 75);
    QLabel *automatonStepLabel = new QLabel(tr("Pocet kroku v ramci generace *"), this);
    automatonStepLabel->setGeometry(10, 101, 200, 23);
    QLabel *maxEvolutionStepLabel = new QLabel(tr("Pocet iteraci *"), this);
    maxEvolutionStepLabel->move(10, 135);
    QLabel *mutationProbabilityLabel = new QLabel(tr("Pravdepodobnost mutace *"), this);
    mutationProbabilityLabel->move(10, 165);
    QLabel *damagedConfigurationCountLabel = new QLabel(tr("Sebeoprava: Pocet konfiguraci *"), this);
    damagedConfigurationCountLabel->move(10, 195);
    damagedConfigurationCountLabel->setToolTip("Pocet konfiguraci, na kterych se trenuje schopnost sebeopravy; povinna polozka");
    damagedConfigurationCountLabel->setCursor(Qt::WhatsThisCursor);
    QLabel *maxDamagePercentageLabel = new QLabel(tr("Sebeoprava: Max poskozeni *"), this);
    maxDamagePercentageLabel->move(10, 225);
    maxDamagePercentageLabel->setToolTip("Procentualni vyjadreni maximalniho mozneho poskozeni uvazovaneho pri trenovani pravidel pro sebeopravu; hodnota z intervalu <0;1>");
    maxDamagePercentageLabel->setCursor(Qt::WhatsThisCursor);
    QLabel *respectFitnessImprovementLabel = new QLabel(tr("Respekt ke zvysovani fitness"), this);
    respectFitnessImprovementLabel->move(10, 255);
    respectFitnessImprovementLabel->setToolTip("Zvysuje-li se fitness bunky v dvou po sobe jdoucich krocich, neni pravidlo resetovano, ackoliv se v okoli nachazi bunka (bunky) s vyssim fitness");
    respectFitnessImprovementLabel->setCursor(Qt::WhatsThisCursor);
    QLabel *extendedRuleCalculationLabel = new QLabel(tr("Upraveny vypocet pravidla"), this);
    extendedRuleCalculationLabel->move(10, 285);
    extendedRuleCalculationLabel->setToolTip("Pri vypoctu noveho pravidla pro bunku se uvazuji sousedi z Moorova okoli (9-okoli), prestoze pravidla jsou tvorena sousedy z von Neumannova okolim (5-okoli)");
    extendedRuleCalculationLabel->setCursor(Qt::WhatsThisCursor);
    QLabel *lockStepCountLabel = new QLabel(tr("Zamykani bunek"), this);
    lockStepCountLabel->move(10, 315);
    lockStepCountLabel->setToolTip("Po zadanem poctu kroku, kdy ma bunka maximalni fitness, se jeji pravidlo prestane uvazovat sousedy pro vypocet noveho pravidla");
    lockStepCountLabel->setCursor(Qt::WhatsThisCursor);
    QLabel *resetCellPercentageLimitLabel = new QLabel(tr("Reset bunky"), this);
    resetCellPercentageLimitLabel->move(10, 345);
    resetCellPercentageLimitLabel->setToolTip("Procentualni vyjadreni uspesnosti pravidla bunky, ktere je nutne k tomu, aby nedoslo k resetu pravidla; hodnota z intervalu <0;1>");
    resetCellPercentageLimitLabel->setCursor(Qt::WhatsThisCursor);
    QLabel *resetGridPercentageLimitLabel = new QLabel(tr("Reset mrizky"), this);
    resetGridPercentageLimitLabel->move(10, 375);
    resetGridPercentageLimitLabel->setToolTip("Procentualni vyjadreni limitu pro pocet bunek, ktere musi byt v cilovem stavu, aby nedoslo k resetu vsech pravidel mrizky; hodnota z intervalu <0;1>");
    resetGridPercentageLimitLabel->setCursor(Qt::WhatsThisCursor);
    QLabel *initialPhaseStepLabel = new QLabel(tr("Delka inicializacni faze"), this);
    initialPhaseStepLabel->move(10, 405);
    initialPhaseStepLabel->setToolTip("Pocet vyvojovych kroku, po kterych bude mozne uplatnovat reset bunky / mrizky (jejich nahodne znovu-nastaveni)");
    initialPhaseStepLabel->setCursor(Qt::WhatsThisCursor);
    loadTaskButton = new QPushButton(tr("Vybrat ulohu"), this);
    loadTaskButton->setGeometry(145, 40, 180, 25);
    neighbourhoodComboBox = new QComboBox(this);
    neighbourhoodComboBox->addItem("von Neumannovo (5-okoli)");
    neighbourhoodComboBox->addItem("Moorovo (9-okoli)");
    neighbourhoodComboBox->setGeometry(145, 70, 180, 25);
    automatonStepLine = new QSpinBox(this);
    automatonStepLine->setRange(1, (INT_MAX - 1));
    automatonStepLine->setGeometry(215, 100, 110, 25);
    maxEvolutionStepLine = new QSpinBox(this);
    maxEvolutionStepLine->setRange(1, (INT_MAX - 1));
    maxEvolutionStepLine->setGeometry(215, 130, 110, 25);
    mutationProbabilityLine = new QDoubleSpinBox(this);
    mutationProbabilityLine->setRange(0, 1);
    mutationProbabilityLine->setDecimals(3);
    mutationProbabilityLine->setSingleStep(0.001);
    mutationProbabilityLine->setGeometry(215, 160, 110, 25);
    damagedConfigurationCountLine = new QSpinBox(this);
    damagedConfigurationCountLine->setRange(1, (INT_MAX - 1));
    damagedConfigurationCountLine->setGeometry(215, 190, 110, 25);
    maxDamagePercentageLine = new QDoubleSpinBox(this);
    maxDamagePercentageLine->setRange(0, 1);
    maxDamagePercentageLine->setSingleStep(0.1);
    maxDamagePercentageLine->setGeometry(215, 220, 110, 25);
    respectFitnessImprovementCheckBox = new QCheckBox(this);
    respectFitnessImprovementCheckBox->setGeometry(215, 250, 25, 25);
    extendedRuleCalculationCheckBox = new QCheckBox(this);
    extendedRuleCalculationCheckBox->setGeometry(215, 280, 25, 25);
    lockStepCountCheckBox = new QCheckBox(this);
    lockStepCountCheckBox->setGeometry(215, 310, 25, 25);
    lockStepCountLine = new QSpinBox(this);
    lockStepCountLine->setRange(1, (INT_MAX - 1));
    lockStepCountLine->setGeometry(245, 310, 80, 25);
    lockStepCountLine->setEnabled(false);
    resetCellPercentageLimitCheckBox = new QCheckBox(this);
    resetCellPercentageLimitCheckBox->setGeometry(215, 340, 25, 25);
    resetCellPercentageLimitLine = new QDoubleSpinBox(this);
    resetCellPercentageLimitLine->setRange(0, 1);
    resetCellPercentageLimitLine->setSingleStep(0.1);
    resetCellPercentageLimitLine->setGeometry(245, 340, 80, 25);
    resetCellPercentageLimitLine->setEnabled(false);
    resetGridPercentageLimitCheckBox = new QCheckBox(this);
    resetGridPercentageLimitCheckBox->setGeometry(215, 370, 25, 25);
    resetGridPercentageLimitLine = new QDoubleSpinBox(this);
    resetGridPercentageLimitLine->setRange(0, 1);
    resetGridPercentageLimitLine->setSingleStep(0.1);
    resetGridPercentageLimitLine->setGeometry(245, 370, 80, 25);
    resetGridPercentageLimitLine->setEnabled(false);
    initialPhaseStepCheckBox = new QCheckBox(this);
    initialPhaseStepCheckBox->setGeometry(215, 400, 25, 25);
    initialPhaseStepLine = new QSpinBox(this);
    initialPhaseStepLine->setRange(1, (INT_MAX - 1));
    initialPhaseStepLine->setGeometry(245, 400, 80, 25);
    initialPhaseStepLine->setEnabled(false);
    evoluteRulesButton = new QPushButton(tr("Start"), this);
    evoluteRulesButton->setGeometry(10, 432, 73, 25);
    evoluteRulesButton->setEnabled(false);
    stopEvoluteRulesButton = new QPushButton(tr("Pozastavit"), this);
    stopEvoluteRulesButton->setGeometry(88, 432, 73, 25);
    stopEvoluteRulesButton->setEnabled(false);
    abortEvoluteRulesButton = new QPushButton(tr("Zrusit"), this);
    abortEvoluteRulesButton->setGeometry(166, 432, 74, 25);
    abortEvoluteRulesButton->setEnabled(false);
    saveRulesButton = new QPushButton(tr("Ulozit"), this);
    saveRulesButton->setGeometry(245, 432, 80, 25);
    saveRulesButton->setEnabled(false);

    qRegisterMetaType<Automaton::GridType>("Automaton::GridType");
    qRegisterMetaType<Automaton::RuleSet>("Automaton::RuleSet");

    // Nastaveni slotu a signalu
    connect(loadTaskButton, SIGNAL(clicked()), this, SLOT(SetOpenFileName()));
    connect(evoluteRulesButton, SIGNAL(clicked()), this, SLOT(StartEvoluteRules()));
    connect(stopEvoluteRulesButton, SIGNAL(clicked()), this, SLOT(StopEvoluteRules()));
    connect(abortEvoluteRulesButton, SIGNAL(clicked()), this, SLOT(AbortEvoluteRules()));
    connect(saveRulesButton, SIGNAL(clicked()), this, SLOT(SetSaveRulesFile()));
    connect(lockStepCountCheckBox, SIGNAL(stateChanged(int)), this, SLOT(CheckLockStepCount(int)));
    connect(resetCellPercentageLimitCheckBox, SIGNAL(stateChanged(int)), this, SLOT(CheckResetPercentageLimit(int)));
    connect(resetGridPercentageLimitCheckBox, SIGNAL(stateChanged(int)), this, SLOT(CheckGridPercenatageLimit(int)));
    connect(initialPhaseStepCheckBox, SIGNAL(stateChanged(int)), this, SLOT(CheckInitialPhase(int)));
    connect(&evolutionManager, SIGNAL(ActualBasicRuleFitness(double)), this, SLOT(ReceiveActualBasicRuleFitness(double)));
    connect(&evolutionManager, SIGNAL(ActualDamagedConfigurationRuleFitness(double, double)), this, SLOT(ReceiveActualDamagedConfigurationRuleFitness(double, double)));
    connect(&evolutionManager, SIGNAL(BasicRulesGenerated(Automaton::RuleSet, int, double)), this, SLOT(AcceptBasicRules(Automaton::RuleSet, int, double)));
    connect(&evolutionManager, SIGNAL(GeneralRulesGenerated(Automaton::RuleSet, int, double)), this, SLOT(AcceptGeneralRules(Automaton::RuleSet, int, double)));

    // Vyplneni ovladacich prvku podle nastaveni z konfiguracniho souboru
    SetConfigurationValues();

    // Nacteni hledaneho vzoru do mrizky
    if(!(Config::GetInstance()->inputEvolutionFile).empty())
    {
        if(LoadTask(Config::GetInstance()->inputEvolutionFile))
        {
            evoluteRulesButton->setEnabled(true);
            zygoteGridWidget->SetGrid(zygote);
            patternGridWidget->SetGrid(pattern);
        }
        else
        {
            std::cerr << "CHYBA: " << "Vstupni soubor nelze nalezt nebo neobsahuje pozadovane udaje." << std::endl;
        }
    }

    // Vynulovani promennych pro ulozeni vysledku simulace
    this->timeSummary = 0;
    this->basicRulesFitness = 0;
    this->generalRulesFitness = 0;
}

bool EvolutionRulesTab::LoadTask(std::string inputFile)
{
    zygote.clear();
    pattern.clear();
    cellPossibilities.clear();

    // Otevreni souboru
    ifstream inFile;
    inFile.open(inputFile.c_str(), std::ifstream::in);
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

bool EvolutionRulesTab::SaveRules(std::string outputFile)
{
    string fileReport;

    if(basicRules.size() != generalRules.size())
    {   // Kontrola, zda-li byly vygenerovany zakladni pravidla i pravidla pro sebeopravu
        return false;
    }

    std::stringstream timeSummaryOut, basicRulesFitnessOut, generalRulesFitnessOut;
    timeSummaryOut << timeSummary;
    basicRulesFitnessOut << basicRulesFitness;
    generalRulesFitnessOut << generalRulesFitness;

    fileReport = "*** CELULARNI AUTOMAT V DYNAMICKEM PROSTREDI - PRAVIDLA ***\r\n\r\n";
    fileReport +=  "[SOUBORY]\r\n";
    fileReport += "Vstupni soubor: \t" + Config::GetInstance()->inputEvolutionFile + "\r\n";
    fileReport += "\r\n[VYSLEDKY SIMULACE]\r\n";
    fileReport += "Doba reseni: \t" + timeSummaryOut.str() + "s\r\n";
    fileReport += "Fitness zakladnich pravidel: \t" + basicRulesFitnessOut.str() + "\r\n";
    fileReport += "Fitness pravidel pro sebeopravu: \t" + generalRulesFitnessOut.str() + "\r\n";

    // Pravidla
    fileReport += "\r\n[ZAKLADNI PRAVIDLA]\r\n";
    for(unsigned int i = 0; i < basicRules.size(); i++)
    {
        for(unsigned int j = 0; j < basicRules[i].size(); j++)
        {
            std::stringstream out;
            out << ((int) basicRules[i][j]);
            fileReport += out.str();
        }
        fileReport += "\r\n";
    }

    fileReport += "\r\n[PRAVIDLA UMOZNUJICI SEBEOPRAVU]\r\n";
    for(unsigned int i = 0; i < generalRules.size(); i++)
    {
        for(unsigned int j = 0; j < generalRules[i].size(); j++)
        {
            std::stringstream out;
            out << ((int) generalRules[i][j]);
            fileReport += out.str();
        }
        fileReport += "\r\n";
    }

    // Vyvoj fitness pravidel
    fileReport += "\r\n[VYVOJ FITNESS ZAKLADNICH PRAVIDEL]\r\n";
    for(unsigned int i = 0; i < basicFitness.size(); i++)
    {
        std::stringstream out;
        out << basicFitness[i];
        fileReport += out.str();
        fileReport += "\r\n";
    }

    // Vyvoj fitness pravidel
    fileReport += "\r\n[VYVOJ FITNESS PRAVIDEL PRO SEBEOPRAVU]\r\n";
    for(unsigned int i = 0; i < averageFitness.size(); i++)
    {
        std::stringstream out;
        out << averageFitness[i];
        fileReport += out.str();
        fileReport += "\r\n";
    }

    std::ofstream outputFileStream(outputFile.c_str(), std::ios::out);
    std::copy(fileReport.begin(), fileReport.end(), std::ostream_iterator<char>(outputFileStream, ""));  // Zapis do souboru

    return true;
}

void EvolutionRulesTab::SetConfigurationValues()
{
    if(Config::GetInstance()->neighbourhood == 4)
        neighbourhoodComboBox->setCurrentIndex(0);
    else
        neighbourhoodComboBox->setCurrentIndex(1);

    automatonStepLine->setValue(Config::GetInstance()->automatonStep);
    maxEvolutionStepLine->setValue(Config::GetInstance()->maxEvolutionStep);
    mutationProbabilityLine->setValue(Config::GetInstance()->mutationProbability);
    damagedConfigurationCountLine->setValue(Config::GetInstance()->damagedConfigurationCount);
    maxDamagePercentageLine->setValue(Config::GetInstance()->maxDamagedPercentage);

    if(Config::GetInstance()->respectFitnessImprovement)
        respectFitnessImprovementCheckBox->setCheckState(Qt::Checked);
    else
        respectFitnessImprovementCheckBox->setCheckState(Qt::Unchecked);

    if(Config::GetInstance()->extendedRuleCalculation)
        extendedRuleCalculationCheckBox->setCheckState(Qt::Checked);
    else
        extendedRuleCalculationCheckBox->setCheckState(Qt::Unchecked);

    if(Config::GetInstance()->lockStepCount != -1)
    {
        lockStepCountCheckBox->setCheckState(Qt::Checked);
        lockStepCountLine->setValue(Config::GetInstance()->lockStepCount);
        lockStepCountLine->setEnabled(true);
    }
    else
    {
        lockStepCountCheckBox->setCheckState(Qt::Unchecked);
        lockStepCountLine->setEnabled(false);
    }

    if(Config::GetInstance()->resetCellPercentageLimit != -1)
    {
        resetCellPercentageLimitCheckBox->setCheckState(Qt::Checked);
        resetCellPercentageLimitLine->setValue(Config::GetInstance()->resetCellPercentageLimit);
        resetCellPercentageLimitLine->setEnabled(true);
    }
    else
    {
        resetCellPercentageLimitCheckBox->setCheckState(Qt::Unchecked);
        resetCellPercentageLimitLine->setEnabled(false);
    }

    if(Config::GetInstance()->resetGridPercentageLimit != -1)
    {
        resetGridPercentageLimitCheckBox->setCheckState(Qt::Checked);
        resetGridPercentageLimitLine->setValue(Config::GetInstance()->resetGridPercentageLimit);
        resetGridPercentageLimitLine->setEnabled(true);
    }
    else
    {
        resetGridPercentageLimitCheckBox->setCheckState(Qt::Unchecked);
        resetGridPercentageLimitLine->setEnabled(false);
    }

    if(Config::GetInstance()->initialPhaseStep != -1)
    {
        initialPhaseStepCheckBox->setCheckState(Qt::Checked);
        initialPhaseStepLine->setValue(Config::GetInstance()->initialPhaseStep);
        initialPhaseStepLine->setEnabled(true);
    }
    else
    {
        initialPhaseStepCheckBox->setCheckState(Qt::Unchecked);
        initialPhaseStepLine->setEnabled(false);
    }
}

void EvolutionRulesTab::SetOpenFileName()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Vyberte ulohu"), "", tr("Vsechny soubory (*);;Textove soubory (*.txt)"));

    if(!fileName.isEmpty())
    {
        if(fileName.startsWith(QApplication::applicationDirPath(), Qt::CaseSensitive))
        {   // Zmena absolutni cesty na relativni
            fileName = fileName.right(fileName.size() - QApplication::applicationDirPath().size() - 1);
        }

        Config::GetInstance()->inputEvolutionFile = fileName.toStdString();
        if(LoadTask(Config::GetInstance()->inputEvolutionFile))
        {
            zygoteGridWidget->SetGrid(zygote);                              // Nastaveni zygoty
            patternGridWidget->SetGrid(pattern);                            // Nastaveni hledaneho vzoru
            evoluteRulesBar->setValue(evoluteRulesBar->minimum());
            evoluteRulesButton->setEnabled(true);
            stopEvoluteRulesButton->setText("Pozastavit");
            update();
        }
        else
        {
            QMessageBox *error = new QMessageBox(QMessageBox::Critical, tr("Chyba"), tr("Vstupni soubor nelze nalezt nebo neobsahuje pozadovane udaje."), QMessageBox::Ok, this, Qt::Widget);
            error->show();
        }
    }
}

void EvolutionRulesTab::SetSaveRulesFile()
{
    std::string outputFile= (QFileDialog::getSaveFileName(this, tr("Vyberte soubor pro ulozeni"), "", tr("Vsechny soubory (*);;Textove soubory (*.txt)"))).toStdString();

    if(!outputFile.empty())
    {
        if(!SaveRules(outputFile))
        {
            QMessageBox *error = new QMessageBox(QMessageBox::Critical, tr("Chyba"), tr("Ulozeni pravidel do vystupniho souboru se nezdarilo.\nMozne priciny: Evoluce nebyla dokoncena."), QMessageBox::Ok, this, Qt::Widget);
            error->show();
        }
    }
}

void EvolutionRulesTab::StartEvoluteRules()
{
    evolutionPlot->ResetTimer();    // Aktivace casovace pro beh grafu vyvoje fitness
    evolutionPlot->StartTimer();

    basicFitness.clear();
    averageFitness.clear();

    // Kontrola vyplneni povinnych polozek
    if((automatonStepLine->text()).isEmpty() || (mutationProbabilityLine->text()).isEmpty() || (maxEvolutionStepLine->text()).isEmpty() || (damagedConfigurationCountLine->text()).isEmpty() || (maxDamagePercentageLine->text()).isEmpty())
    {
        QMessageBox *error = new QMessageBox(QMessageBox::Warning, tr("Upozorneni"), tr("Nejsou vyplnene vsechny povinne polozky (polozky oznacene hvezdickou)."), QMessageBox::Ok, this, Qt::Widget);
        error->show();
        return;
    }

    // Odemknuti / zamknuti ovladacich prvku
    loadTaskButton->setEnabled(false);
    evoluteRulesButton->setEnabled(false);
    stopEvoluteRulesButton->setEnabled(true);
    abortEvoluteRulesButton->setEnabled(true);
    saveRulesButton->setEnabled(false);
    projectStatus->setText("Evoluce zakladnich pravidel");

    // Aktualizace konfiguracnich udaju podle zmen uzivatele primo ve formulari
    Config::GetInstance()->neighbourhood = neighbourhoodComboBox->currentText().toInt();
    if(neighbourhoodComboBox->currentIndex() == 0)
        Config::GetInstance()->neighbourhood = 4;
    else
        Config::GetInstance()->neighbourhood = 8;
    Config::GetInstance()->automatonStep = automatonStepLine->text().toInt();
    Config::GetInstance()->maxEvolutionStep = maxEvolutionStepLine->text().toInt();
    Config::GetInstance()->mutationProbability = mutationProbabilityLine->text().toDouble();
    if(respectFitnessImprovementCheckBox->isChecked())
        Config::GetInstance()->respectFitnessImprovement = true;
    else
        Config::GetInstance()->respectFitnessImprovement = false;
    if(extendedRuleCalculationCheckBox->isChecked())
        Config::GetInstance()->extendedRuleCalculation = true;
    else
        Config::GetInstance()->extendedRuleCalculation = false;
    if(lockStepCountCheckBox->isChecked())
        Config::GetInstance()->lockStepCount = lockStepCountLine->text().toInt();
    else
        Config::GetInstance()->lockStepCount = -1;
    if(resetCellPercentageLimitCheckBox->isChecked())
        Config::GetInstance()->resetCellPercentageLimit = resetCellPercentageLimitLine->text().toDouble();
    else
        Config::GetInstance()->resetCellPercentageLimit = -1;
    if(resetGridPercentageLimitCheckBox->isChecked())
        Config::GetInstance()->resetGridPercentageLimit = resetGridPercentageLimitLine->text().toDouble();
    else
        Config::GetInstance()->resetGridPercentageLimit = -1;
    if(initialPhaseStepCheckBox->isChecked())
        Config::GetInstance()->initialPhaseStep = initialPhaseStepLine->text().toInt();
    else
        Config::GetInstance()->initialPhaseStep = -1;

    // Nastaveni progressBaru
    evoluteRulesBar->setValue(evoluteRulesBar->minimum());
    evoluteRulesBar->setMaximum(Config::GetInstance()->maxEvolutionStep);

    // Vynulovani pripadnych vysledku z minulych behu
    this->basicRulesFitness = 0;
    this->generalRulesFitness = 0;
    this->timeSummary = 0;

    evolutionManager.FindSolution(zygote, pattern, cellPossibilities);
}

void EvolutionRulesTab::StopEvoluteRules()
{
    if(stopEvoluteRulesButton->text() == "Pozastavit")   // Stridani textu na buttonu Pozastavit / Obnovit
       stopEvoluteRulesButton->setText("Obnovit");
    else
       stopEvoluteRulesButton->setText("Pozastavit");

    evolutionManager.StopEvoluteRules();    // Pokyn k pozastaveni evoluce pravidel

    if(evolutionPlot->IsTimerRunning())     // Zastaveni grafu zobrazujiciho vyvoj fitness
        evolutionPlot->KillTimer(true);
    else
        evolutionPlot->StartTimer();
}

void EvolutionRulesTab::AbortEvoluteRules()
{
    // Odemknuti ovladacich prvku
    loadTaskButton->setEnabled(true);
    evoluteRulesButton->setEnabled(true);
    stopEvoluteRulesButton->setEnabled(false);
    abortEvoluteRulesButton->setEnabled(false);
    saveRulesButton->setEnabled(true);

    evolutionPlot->KillTimer(true);        // Vypnuti casovace
    evolutionManager.AbortEvoluteRules();  // Pokyn k pozastaveni evoluce pravidel

    // Zjisteni aktualnich vysledku
    evolutionManager.GetActualResults(basicRules, generalRules, basicRulesFitness, generalRulesFitness, timeSummary);
}

void EvolutionRulesTab::CheckLockStepCount(int newState)
{
    if(newState)
        lockStepCountLine->setEnabled(true);
    else
        lockStepCountLine->setEnabled(false);
}

void EvolutionRulesTab::CheckResetPercentageLimit(int newState)
{
    if(newState)
        resetCellPercentageLimitLine->setEnabled(true);
    else
        resetCellPercentageLimitLine->setEnabled(false);
}

void EvolutionRulesTab::CheckGridPercenatageLimit(int newState)
{
    if(newState)
        resetGridPercentageLimitLine->setEnabled(true);
    else
        resetGridPercentageLimitLine->setEnabled(false);
}

void EvolutionRulesTab::CheckInitialPhase(int newState)
{
    if(newState)
        initialPhaseStepLine->setEnabled(true);
    else
        initialPhaseStepLine->setEnabled(false);
}

void EvolutionRulesTab::ReceiveActualBasicRuleFitness(double actAverageFitness)
{
    evoluteRulesBar->setValue(evoluteRulesBar->value() + 1);
    basicFitness.push_back(actAverageFitness);
}

void EvolutionRulesTab::ReceiveActualDamagedConfigurationRuleFitness(double actAverageFitness, double )
{
    evoluteRulesBar->setValue(evoluteRulesBar->value() + 1);
    averageFitness.push_back(actAverageFitness);
}

void EvolutionRulesTab::AcceptBasicRules(Automaton::RuleSet rules, int timeSummary, double fitness)
{
    // Aktivace cesovace pro beh grafu vyvoje fitness
    evolutionPlot->ResetTimer();
    evolutionPlot->StartTimer();

    this->basicRules = rules;
    this->basicRulesFitness = fitness;
    this->timeSummary = timeSummary;
    evoluteRulesBar->setValue(evoluteRulesBar->maximum());
    projectStatus->setText("Evoluce pravidel pro sebeopravu");
    evoluteRulesBar->setValue(evoluteRulesBar->minimum());

    update();
}

void EvolutionRulesTab::AcceptGeneralRules(Automaton::RuleSet rules, int timeSummary, double fitness)
{
    this->generalRules = rules;
    this->generalRulesFitness = fitness;
    this->timeSummary = timeSummary;
    evolutionPlot->KillTimer(false);  // Vypnuti casovace

    // Odemknuti ovladacich prvku
    loadTaskButton->setEnabled(true);
    evoluteRulesButton->setEnabled(true);
    stopEvoluteRulesButton->setEnabled(false);
    abortEvoluteRulesButton->setEnabled(false);
    saveRulesButton->setEnabled(true);
    evoluteRulesBar->setValue(evoluteRulesBar->maximum());

    update();
}

void EvolutionRulesTab::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setPen(QPen(Qt::gray, 1));
    painter.drawLine(340, 40, 340, 455);
    painter.drawLine(524, 40, 524, (250 + zygoteGridWidget->height()));
}
