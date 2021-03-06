# PARAMETROS de Linea de Commandos

gcainva file.ctl file.inp

## ARCHIVO ctl

nRuns		: numero de corridas con este set de parametros

nEvals		: numero de pasos de las corridas (tiempo maximo)

rndSeed		: semilla para el generador de nros al azar, si es 0 toma el time

gr		: Graficos Y/N

inter		: intervalo en pasos para mostrar grafico y/o 
		  hacer salidad de datos y/o tomar informacion para promedios

init		: numero de pasos a partir del cual inicia la toma de datos o muestra en pantalla

de		: imprime densidad de especies y calcula promedios a partir de nEvals/2

sa		: salva archivo seed al a partir de [inicio] y con la periodicidad 
		  especificada en [inter]

baseName	: Nombre de Archivo para salvar archivos sed, y demas salidas

idrPal		: Paleta de colores idrisi utilizada en salida grafica (NO IMPLEMENTADO)

DimX, DimY	: dimensiones del lattice

noSpecies	: Numero de especies

seedFile	: Archivo sed, configuracion especial de especies edades y habitats

setSeedFile	: Archivo setSeed, especies y edades y nro de individuos para sembrar al azar.

runLineParms	: Toma parametros de las especies en una sola linea, sirve para multiples
		  simulaciones

calcSpectrum	: [Y/N] Calcula el espectro R de la distribucion espacial de los ADULTOS 
		  de todas las especies juntas

calcUnivRipleyK	: [Y/N] Calcula la funcion K de Ripley para los ADULTOS de todas las especies

calcX		: Coordenada X para la posicion de la ventana de utilizada para el calculo del 
                  Espectro y Ripley's K
calcY		: Coordenada Y
calcXLen	: Ancho de la ventana para el calculo
calcYLen	: Profundidad de la ventana para el calculo

pomac		: [Y/N] Este es el modo POMAC donde el output es una sola linea con los parametros y los criterios para evaluacion del modelo.

pomacFile	: Nombre de archivo donde va a parar el output del modelo.

inp/lin: nombre de archivo de parametros de las especies

## ARCHIVO sed

Archivo de condiciones iniciales -> Posiciones espaciales, especie, edad, Habitat.

	La estructura es la siguiente 

	DIMX	DIMY
	SP
		Matriz de numeros de DIMX x DIMY con la posicion de las especies

	AG
		Matriz de numeros de DIMX x DIMY con la edad correspondiente a los individuos de las
		especies especificadas en la seccion SP

	HA
		Matriz de numeros de DIMX x DIMY con la disposicion del los tipos de habitats.
		Solo de lectura no se incluye en archivos salvados
	HAI 
		En la linea siguiente a HAI va el nombre de un archivo IDRISI con la distribucion de 
		habitats

	AJ
		Distribucion de adultos y juveniles, con la siguiente codificacion
		Adultos de la Nth especie  (N-1)*2+1
		Juveniles                  (N-1)*2+2


set: archivo de condiciones iniciales -> siembra al azar individuos de cierta especie y edad
	esta compuesto de lineas con la siguiente estructura
	especie edad cantidad "posicion x maxima" habitat
	

## Archivo par�metros de especies (.inp)

Est� compuesto por l�neas con la siguiente estructura:

	sp MaximunSurvivalDistance AdultAge PJuvenilMortality PAdultMortality MeanDispersalDistance ParmDispersal NRecruits DispersalNorm NeighSurvivalDistance NeighSurvivalSlopeAd NeighSurvivalSlopeJu ShadeTolerance PColonization

### El significado de los parametros es:

	sp: numero de especie menos 1 

	spName:
		Nombre de la especie 

	MaximunSurvivalDistance: 
		Distancia m�xima, a la que un adulto puede influir en la mortalidad de los juveniles

	AdultAge: 
		Edad a partir de la cual el individuo empieza a reproducirse. Habria que sumarle el 
		lag debido a que en el modelo los adultos producen individuos de 140 cm de altura, 
		o sea sumar el tiempo que tardan en llegar a 140.

	PJuvenilMortality: 
		Probabilidad de muerte de 1 individuo juvenil (densoindependiente).

	PAdultMortality: 
		Probabilidad de muerte de 1 individuo Adulto (densoindependiente).

	MeanDispersalDistance: 
		Distancia de dispersion media (LA DISTANCIA MAXIMA para dispersion E!!!!!!!)

	ParmDispersal: 
		NO USADO porque la dispersion esta fija en E!!!!!!!!!!!!!!!!!!!!!!!!!
		En dipersion gamma (G) es el parametro "n" (shape) de la distribucion
		En dispersion gaussiana es la desviacion estandard de la distribucion
		

	NRecruits:

		Cantidad de nuevos reclutas producidos por individuo adulto por evento reproductivo

	DispersalNorm: 

		Solamente estan implementados E & X 

		Distribucion de los reclutas a partir de los individuos adultos.
		
		E: Norma Euclidiana, se dispersan a una distancia MAXIMA = MeanDispersalDistance, 
		   calculando la distancia euclidiana.
		   
		N: Von Newman (Norma 1), rombos equivalente a los cuatro vecinos mas cercanos.
		
		M: Moore (Norma Infinito), cuadrado de lado MeanDispersalDistance*2+1 equivalente a
		   los 8 vecinos mas cercanos
		   
		X: Distribucion exponencial, el parametro corresponde a la media
		
		G: Distribucion Gamma 
		
		U: Distribucion Normal
		
	NeighSurvivalDistance:
		
		Distancia que utiliza para evaluar el entorno en el cual influye la densidad para el 
		calculo de la mortalidad densodependiente, o la presencia de adultos segun el parametro, ShadeTolerance.

	NeighSurvivalSlopeAd
		
		Mortalidad Densodependiente producida por inhibicion de adultos en adultos.

		Dist = NeighSurvivalDistance + ShadeTolerance * (ActualAge-AdultAge)
		Si Dist>MaximunSurvivalDistance -> Dist=MaximunSurvivalDistance

		En la version de CaInvaFullMortality
		Pm = NeighSurvivalSlopeAd*na
		na: propocion de adultos en el entorno determinado por Dist. 
		    
		En la version CaInva
		Pm = PAdultMortality + NeighSurvivalSlopeAd*na
		
			
	PJuvenilMortality
	
		Mortalidad de Juveniles densoindependiente


	NeighSurvivalSlopeJu

		Mortalidad Densodependiente producida por influencia de juveniles y adultos. 
		Deberia sumar la edad para hacerlo equivalente al area basal segun @Comita2009:
		
		Pm = PJuvenilMortality+ NeighSurvivalSlopeJu*nj
		
		nj: Proporcion de juveniles+adultos en el entorno determinado por NeighSurvivalDistance


	ShadeTolerance (NO IMPLEMENTADO EN ULTIMA VERSION)
		Aumento del entorno de influencia de los adultos sobre los juveniles con la edad.
		kn = NeighSurvivalDistance + ShadeTolerance*(ActualAge-AdultAge)


//	
// NO SE USA QUEDO FIJO EN ED
//
EvalType: 	A, La evaluacion comienza siempre desde el mismo vertice
			B, Los eventos se evaluan al azar sin reposicion
			E, La evaluacion comienza desde un vertice elegido al azar en cada paso
			
TimeType: 	C, Continuo los eventos tienen un efecto instantaneo, la evaluacion es gradual
			D, Todos los eventos se evaluan al mismo tiempo 

EvenType: Cualquiera sea el valor de este parametro, 
			Primero se evalua la mortalidad y si muere no se reproduce

En realidad hay que usar solo la opcion D de timeType, para la opcion C hay que redefinir 
como se evaluan los eventos en el modelo. 
	
