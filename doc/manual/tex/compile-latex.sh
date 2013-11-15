#!/bin/bash


str='enram-manual'


latex -output-directory=./../out -interaction=nonstopmode $str.tex
cd ./../out
bibtex $str.aux
makeindex -o $str.ind $str.idx
cd ./../tex
latex -output-directory=./../out -interaction=nonstopmode $str.tex
latex -output-directory=./../out -interaction=nonstopmode $str.tex
dvips -o ./../out/$str.ps ./../out/$str.dvi
ps2pdf ./../out/$str.ps ./../out/$str.pdf

rm ./../out/$str.aux
rm ./../out/$str.bbl
rm ./../out/$str.blg
rm ./../out/$str.dvi
rm ./../out/$str.idx
rm ./../out/$str.ilg
rm ./../out/$str.ind
rm ./../out/$str.log
rm ./../out/$str.out
rm ./../out/$str.ps
rm ./../out/$str.toc
