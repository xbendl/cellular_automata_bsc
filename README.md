# cellular_automata_bsc
This repository contains a software created for my bachelor thesis "Cellular Automaton in Dynamical Environment". Text of my thesis can be found at:
https://www.fit.vut.cz/study/thesis/4483/.en?year=0&stud=bendl (czech language)

![GUI](https://github.com/xbendl/cellular_automata_bsc/blob/main/fig.png)

Abstract:
==========
The thesis focuses on a new concept of cellular automata control based on instructions. The instruction can be understood as a rule that checks the states of cells in pre-defined areas in the cellular neighbourhood. If a given condition is satisfied, the state of the central cell is changed according to the definition of the instruction. Because it’s possible to perform more instructions in one computational step, their sequence can be understood as a form of a short program. This concept can be extended with simple operations applied to the instruction’s prescription during interpretation of the instructions – an example of such operation can be row shift or column shift. An advantage of the instruction-based approach lies in the search space reduction. In comparison with the table-based approach, it isn’t necessary to search all the possible configurations of the cellular neighbouhood, but only several areas determined by the instructions. While the groups of the inspected cells in the cellular neighbourhood are designed manually on the basis of the analysis of the solved task, their sequence in the chromosome is optimized by genetic algorithm. The capability of the proposed method of cellular automata control is studied on these benchmark tasks - majority, synchronization, self-organization and the design of combinational circuits.

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

