# cellular_automata_msc
This repository contains a software created for my bachelor thesis "Cellular Automaton in Dynamical Environment". Text of my thesis can be found at:
https://www.fit.vut.cz/study/thesis/4483/.en?year=0&stud=bendl (czech language)

![example of interface]([http://url/to/img.png](https://github.com/xbendl/cellular_automata_bsc/blob/main/fig.png))

======================================================
Bakalarska prace: Celularni automat v dynamickem prostredi
Platforma: OS Linux
======================================================

Pozadavky:
==========
1) OS Linux
2) Knihovna Qt v.4

Instalace:
==========
1) Spusteni skriptu ./install.sh s pravy roota (bude se instalovat knihovna Qwt)

Spusteni programu:
==================
Synopsis: ./automaton [-u usage] [-i inputFile]
-u Nepovinny parametr urcujici vyuziti programu
   1 = evoluce pravidel, 2 = analyza pravidel
-i Nepovinny parametr urcujici cestu ke vstupnimu souboru
   se zadanim ulohy pri "-u 1", s pravidly pri "-u 2"

Struktura programu:
===================
Program je rozdelen pomoci zalozek na tri casti:
- Tvorba uloh: Slouzi k vytvoreni ulohy: zygoty a hledaneho vzoru.
- Evoluce pravidel: Slouzi k evoluci zakladnich pravidel pro vyvoj ze zygoty
  do hledaneho vzoru a pro vyvoj pravidel schopnych sebe-opravy.
- Analyza pravidel: Slouzi k testovani vyvinutych pravidel.

Napoveda v programu:
====================
Vetsina ovladacich prvku ma napovedu. K jejimu zobrazeni dojde po prejeti mysi
nad oznacenim daneho prvku (napr. "Reset bunky" v zalozce "Evoluce pravidel").
Podrobnejsi navod je uveden jako "Dodatek B" v textu bakalarske prace

Nastaveni parametru:
====================
Cesta k souboru s parametry: ./configuration.txt
- Soubor s vychozimi parametry evoluce a analyzy pravidel
  vcetne jejich popisu.
- Vsechny parametry lze pozdeji menit ve spustene aplikaci pomoci
  interaktivnich ovladacich prvku.

POZNAMKY:
=========
K realizaci grafu v aplikaci zobrazujiciho prubeh evoluce byla vyuzita
knihovna Qwt (http://qwt.sf.net).

