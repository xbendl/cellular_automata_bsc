#!/bin/bash

# Rozbaleni archivu
unzip src.zip

# Kompilace knihovny Qwt
qmake ./src/qwt/qwt.pro -o Makefile
make -C src/qwt

# Nastaveni promenne prostredi $QWT
PART1=`pwd`
PART2="/qwt"
export QWT=$PART1$PART2

# Zkopirovani dynamickych knihoven do /usr/lib
cp ./src/qwt/lib/* /usr/lib/

# Kompilace programu
cd src
qmake
cd ..
make -C src
 s
# Presun souboru a slozek
mv src/configuration.txt ./
mv src/example_tasks/ ./
mv src/example_rules/ ./

# Sebedestrukce
#rm src.tar.gz
#rm install.sh
