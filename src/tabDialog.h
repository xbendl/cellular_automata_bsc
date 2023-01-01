/**
* \file tabDialog.h
* Spravce hlavniho okna.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef TAB_DIALOG_H
#define TAB_DIALOG_H

#include <QtGui>
#include <QDialog>
#include <stdlib.h>
#include "evolutionRulesTab.h"
#include "analyseRulesTab.h"
#include "taskTab.h"
#include "config.h"

/// Spravce hlavniho okna.
class TabDialog : public QDialog
{
    Q_OBJECT

    private:
    QTabWidget *tabWidget;

    public:
    /**
    * Konstruktor.
    */
    TabDialog(QWidget *parent = 0);
};

#endif // TAB_DIALOG_H
