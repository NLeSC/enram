#!/bin/bash

# execute this script in a terminal by:
# . set-convert-environment.sh
# (don't omit the dot at the beginning)

enramroot='/home/daisycutter/enram/'

# define where the different ENRAM directories are:
export ENRAM_DATA=$enramroot'testdata/'
export ENRAM_RAW_DATA=$enramroot'testdata/raw/'
export ENRAM_CONVERTED_DATA=$enramroot'testdata/converted/odim/'
export ENRAM_TMP='/tmp/'
