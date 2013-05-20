@ECHO OFF
ECHO 
ECHO Extrae una ventana y deja solamente los adultos de la secion AJ (impares)
ECHO
ECHO 1: Imput File
ECHO 2: Output File
ECHO 3: X top left
ECHO 4: Y top left
ECHO 5: X length  
ECHO 6: Y length

\Cpp\SpatialAnalysis\sedWin %1 kk.sed AJ 0 %3 %4 %5 %6
\Cpp\SpatialAnalysis\sedJuvAd kk.sed %2 A

