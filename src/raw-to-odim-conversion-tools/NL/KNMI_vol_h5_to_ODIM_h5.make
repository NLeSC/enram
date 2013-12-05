#! /bin/bash
#
# Makefile
#

#Setting names of program

SOURCE="KNMI_vol_h5_to_ODIM_h5.c"
BINARY="$HOME/bin/KNMI_vol_h5_to_ODIM_h5"

#Setting compiler and options

COMPILE="gcc -Wall -O"

#Setting include and library directories

IPATH="-I$HOME/include"
LPATH="-L$HOME/lib"

#Setting libraries

LIBRARIES="-lhdf5_hl -lhdf5 -lz -lm"

#Compiling of source

$COMPILE $SOURCE -o $BINARY $IPATH $LPATH $LIBRARIES
