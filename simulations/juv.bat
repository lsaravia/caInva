REM Parametro 1 = nombre de archivo
REM           2 = dimension para ripley K

del %1.trk
del %1.g
del %1.spe
del %1.rpk
del %1.out
del %1.txt
del %1.age
del %11.sed
gcaSimul %1.ctl %1.inp
rem call fft %11.sed %11.spe SP 0 R 0.05 0 500
\Cpp\SpatialAnalysis\sed2xy.exe %11.sed %1.dat JU 1 1
riptri %1.dat %1.plt %2 1 500 1 0.025 %1.trk

