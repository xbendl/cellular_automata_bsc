# cellular_automata_bsc
This repository contains a software created for my bachelor thesis "Cellular Automaton in Dynamical Environment". Further description in the
[thesis fulltext (CZ)](https://www.fit.vut.cz/study/thesis/4483/.en?year=0&stud=bendl)

![GUI](https://github.com/xbendl/cellular_automata_bsc/blob/main/fig.png)

Abstract:
==========
This bachelor thesis focuses on the method of evolution of cellular automaton capable of self-repair after being damaged by external environment. The described method is based on cellular programming algorithm and uses principles of biological development. Experiments leading to verification of regenerative ability for cellular automaton evolved by this approach are presented in this work.

Requirements:
============
1) OS Linux
2) Qt library v.4

Installation:
============
1) Run ./install.sh with root permission (because it needs to install Qwt)

How to run it:
=============
Synopsis: ./automaton [-u usage] [-i inputFile]
-u Optional parameters specifying the program use
   1 = CA rule evolution, 2 = CA rule analysis
-i Optional parameter specifying the path to the input file

User interface:
===============
Software contains three main sections (tabs) for work with cellular automata (CA):
- Create jobs: Specify "zygote" figure and desired "final" figure
- Evolution of CA rules: Evolution of rules capable of development of specified final figure from zygote figure and self-correction after random destruction of evolved figure
- Test CA rules

