#include "tabDialog.h"

TabDialog::TabDialog(QWidget *parent) : QDialog(parent)
{
    // Ziskani zadanych parametru z prikazoveho radku
    QStringList arguments = QCoreApplication::arguments();          // Pocet parametru prikazoveho radku
    bool usageFlag = false;
    int usage;
    std::string inputFile, outputFile;

    for(int i = 1; i < arguments.count(); i++)
    {   // Provedeni akci na zaklade precteni jednotlivych parametru
        if((arguments[i] == "-u") && (i+1 != arguments.count()))
        {   // Precteni vyuziti programu (1 = generovani pravidel, 2 = analyza pravidel)
            usage = arguments[++i].toInt();
            usageFlag = true;
            continue;
        }
        else if((arguments[i] == "-i") && (i+1 != arguments.count()))
        {   // Precteni cesty ke vstupniho souboru
            inputFile = arguments[++i].toStdString();
            continue;
        }
    }

    // Prirazeni parametru inputFile podle vyznamu bud k Evoluci pravidel, nebo k Analyze pravidel
    if((usage == 1) && (!inputFile.empty()))
        Config::GetInstance()->inputEvolutionFile = inputFile;
    else if((usage == 2) && (!inputFile.empty()))
        Config::GetInstance()->inputAnalyseFile = inputFile;

    // Nastaveni ovladacich prvku podle zadaneho konfiguracniho souboru
    if(!Config::GetInstance()->LoadConfiguration("configuration.txt"))
    {
        std::cerr << "CHYBA: Konfiguracni soubor nelze nalezt nebo neobsahuje pozadovane udaje." << std::endl;
    }

    // Globalni nastaveni aplikace
    setFixedSize(952, 495);
    setFont(QFont("Helvetica", 9, QFont::Normal));
    setWindowTitle(tr("Celularni automat v dynamickem prostredi"));

    // Pridani zalozek
    tabWidget = new QTabWidget(this);
    tabWidget->addTab(new EvolutionRulesTab(this), tr("Evoluce pravidel"));
    tabWidget->addTab(new AnalyseRulesTab(this), tr("Analyza pravidel"));
    tabWidget->addTab(new TaskTab(this), tr("Tvorba uloh"));
    tabWidget->setFixedSize(952, 495);

    // Prepnuti na sousedni zalozku v pripade, ze parametr prikazove radky udava vyuziti programu jako analyzatoru pravidel
    if(usage == 2)
        tabWidget->setCurrentIndex(1);
}

