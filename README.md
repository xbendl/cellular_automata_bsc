# cellular_automata_bsc
Master thesis: Intruction-controlled cellular automata.

This thesis focuses on the new concept of cellular automata controlling based on instructions. Instruction can be understood as a rule, whose checks states of cells in pre-defined areas
in neighbourhood of central cell. If condition is satisfied, state of central cell is changed according by definition of the instruction. Because it’s possible performs more instructions
in one computational step, their sequence can be understood as form of short program. Advantage of instruction usage inheres in search space reducing. In comparison with truth table, there isn’t necessary search all set of possible configurations of cells in neighbouhood, but only several determined areas. Althought search space is reduced, selection of suitable instructions would be ensure same computational power as traditional cellular automata with Moore’s neighbourhood. While groups of inspected cells are manually designed, their sequence in chromosome is optimalized by genetic algorithm.

