//
// CADis : CA Discreto con bordes continuos
//

#include <ctype.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>

#include <cstring>
#include <cstdlib>

#include "caInva.hpp"
#include "bgi.hpp"
//#include "fortify.h"

#ifdef __DJGPP__
#include <conio.h>
#else
#define getch() GrKeyRead()
#define kbhit() GrKeyPressed()
#include <grxkeys.h>
#endif

struct CaInvaParms
{
	unsigned long nRuns;
	unsigned long nEvals;
	long rndSeed;
	char gr;
	unsigned long inter;
	unsigned long init;
	char de;
	char sa;
	string baseName;
	char idrPal[40];
	int dimX;
	int dimY;
	int noSpecies;
	char seedFile[40];
	char setSeedFile[40];
	char runLineParms;
    char calcSpectrum;
    char calcUnivRipleyK;
    char pomac;
    char pomacFile[40];
    int calcX;
    int calcY;
    int calcXLen;
    int calcYLen;
    char calcType[3];
	int rpkNoIntervals;
	int rpkLenInterval;
//    char spectrumFile[40];
};

int ReadParms(char * pFile, CaInvaParms &p);

int main(int argc, char * argv[])
{
	static bool priPomac=true;
	bool pomacEarlyBreak=false;
    //Fortify_EnterScope();
    CAInva ca;
	CaInvaParms p;

    fstream pof; // Pomac output file stream
    if( argc == 1 )
    {
		ReadParms("pomac.ctl",p);
		ca.Init( p.noSpecies, p.dimX, p.dimY, p.rndSeed);
		if( !ca.ReadParms("pomac.inp"))
        	    return 1;
      	ca.ReadPomacParms("pomac.lin");
    }
    else if( argc == 3 )
	{
		//Fortify_CheckAllMemory();
		ReadParms(argv[1],p);
		ca.Init( p.noSpecies, p.dimX, p.dimY, p.rndSeed);
		if(p.runLineParms=='Y')
		{
			if( !ca.ReadFileLineParms( argv[2] ) )
        	    return 1;
		}
		else
			if( !ca.ReadParms( argv[2] ) )
        	    return 1;
	}
	else
	{
		cerr << "GCaInva controlFile.ctl parmsFile.inp" << endl;
		exit(1);
	}

	if( strstr(p.seedFile,"sed")!=NULL )
		ca.ReadSeed(p.seedFile);	// Lee mapa ASCII de las especies y edades
            
	if( strstr(p.setSeedFile,"set")!=NULL )
		ca.ReadSetSeed(p.setSeedFile);


	bool privez=true;
	double prom=0,dens=0;
	long noIndivid=0;
	
	for(int r=0; r<p.nRuns; r++)
	{
		prom =0;
		if( privez )
			privez = false;
		else
		{
			//Fortify_CheckAllMemory();
			ca.Reset();
			if( strstr(p.seedFile,"sed")!=NULL )
				if( !ca.ReadSeed(p.seedFile) )	// Lee mapa ASCII de las especies y edades
	        	    return 1;
            
			if( strstr(p.setSeedFile,"set")!=NULL )
				ca.ReadSetSeed(p.setSeedFile);
			if(p.pomac == 'Y' )
    			ca.ReadPomacParms("pomac.lin");
            else if(p.runLineParms=='Y')
			{
				if( !ca.ReadFileLineParms( argv[2] ) )
	        	    return 1;
			}
		}


		if( p.gr=='Y' )
			ca.InitGraph();
		int i=0;

        //
        // Calcula e imprime las densidades iniciales
        //
        ca.SetN(0);
		if( p.gr=='Y' )
			ca.PrintGraph();
		else
			cerr << "Evaluation " << i << endl;
		if( p.de=='Y' && (i+1)>=p.init)
			ca.PrintDensityAge( p.baseName, argv[1], p.nEvals);
		ca.SetN(1);

        
        //
        // Loop principal de evaluacion
        //
		for(i=0; i<p.nEvals; i++)
		{
// 			ca.Evaluate();
//			//Fortify_CheckAllMemory();
            ca.EvaluateED();

			if( ((i+1) % p.inter)==0 || i==0 )
			{
				if( p.pomac=='Y' )
                {
					cerr << "Evaluation " << (i+1) << " Dens: " << dens << endl;
                    dens = ca.PrintDensityAge( pof, p.nEvals);
                    if( dens > 800 || ( dens==0 & i>150 ) )
                    {
                    	dens = ca.PrintDensityAge( pof, i+1);
                    	pomacEarlyBreak=true;                    	
                    	break;
                    }
                }
                else
                {
					if( p.gr=='Y' )
						ca.PrintGraph();
					else
						cerr << "Evaluation " << (i+1) << endl;
					if( p.de=='Y' && (i+1)>=p.init)
					{
						//Fortify_CheckAllMemory();
	                    dens = ca.PrintDensityAge( p.baseName, argv[1], p.nEvals);
                    
						//prom += dens;
						//if( dens == 0 )
						//	break;
					}
					if( (p.calcSpectrum=='Y' || p.calcUnivRipleyK=='Y') && (i+1)>=p.init)
					{
	                    simplmat <double> polspe;
		               	int cX=p.calcX;
		                if( p.calcSpectrum=='Y' )
	                    {
		                    for(int cc=0; cc<4; cc++)
		                    {
								ca.CalcWinDensity(p.calcType,cX,p.calcY,p.calcXLen,p.calcYLen,noIndivid);
								if(noIndivid>3)
								{
		                    		ca.CalcSpectrum(p.calcType,cX,p.calcY,p.calcXLen,p.calcYLen,polspe,noIndivid);
		                    		ca.PrintSpectrum(p.baseName+".spe",p.calcType,cX,p.calcY,p.calcXLen,p.calcYLen,polspe,noIndivid);
                                }
		                		cX += 2*p.calcXLen;
                            }
		                }
		               	cX=p.calcX;
						if( p.calcUnivRipleyK=='Y')
						{
		                    for(int cc=0; cc<4; cc++)
		                    {
								ca.CalcWinDensity(p.calcType,cX,p.calcY,p.calcXLen,p.calcYLen,noIndivid);
								if(noIndivid>3)
								{
			            			ca.CalcUnivRipleyK(p.calcType,cX,p.calcY,p.calcXLen,p.calcYLen,polspe,noIndivid,	p.rpkNoIntervals, p.rpkLenInterval);
				                	ca.PrintUnivRipleyK(p.baseName+".rpk",p.calcType,cX,p.calcY,p.calcXLen,p.calcYLen,polspe,noIndivid);
				                	ca.PrintUnivG(p.baseName+".g",p.calcType,cX,p.calcY,p.calcXLen,p.calcYLen,polspe,noIndivid);
								}
		                		cX += 2*p.calcXLen;
		                    }
						}
					}
                }

			}

			if( p.sa=='Y' && (i+1)>=p.init && ((i+1)%p.inter)==0 )
			{
				ostringstream name;
				name << p.baseName << (i+1) << ".sed" << ends;
				ca.SaveSeed( name.str().c_str() );
			}
		
		}

		if( p.pomac=='Y' )
		{
			if(	priPomac )
            {
				pof.open( p.pomacFile, ios::out );
				if( !pof )
				{
					cerr << "Cannot open file. " << p.pomacFile << endl;
					return 0;
				}
				ca.PrintParamsPomac(pof,true);
	            for(int k=0; k<p.noSpecies; k++)
	            {
					pof << "densAD"  << k << "\tdensJU" << k << "\t";
	            }
				if( p.calcUnivRipleyK=='Y' )
                {
                    pof << "\trpk1"  << "\trpk2" << "\trpk3"  << "\trpk4" << "\trpk5";
                }
                pof << endl;
				priPomac = false;
			}
			else
			{
				pof.open( p.pomacFile, ios::app );
				if( !pof )
				{
					cerr << "Cannot open file. " << p.pomacFile << endl;
					return 0;
				}
			}

			ca.PrintParamsPomac(pof);
            if(pomacEarlyBreak)
            {
	            ca.PrintDensityAge(pof,i+1);
//				pof << "\t" << 0.0  << "\t" << 0.0 << "\t" <<
//					   0.0  << "\t" << 0.0;
				if( p.calcUnivRipleyK=='Y' )
                {
                   	pof << "\t-5.0" << "\t-9.0" << "\t-13.0"  << "\t-17.0" << "\t-21.0";
                }
            	pof << endl;
            }
            else
            {
	            ca.PrintDensityAge(pof,p.nEvals);
        	    simplmat <double> polspe;
				if( p.calcUnivRipleyK=='Y' )
				{
//					ca.CalcWinDensity(p.calcType,p.calcX,p.calcY,p.calcXLen,p.calcYLen,noIndivid);
//            		ca.CalcSpectrum(p.calcType,p.calcX,p.calcY,p.calcXLen,p.calcYLen,polspe,noIndivid);
//            		ca.PrintSpectrum(pof,polspe);
                	int cX=p.calcX;
                    for(int cc=0; cc<4; cc++)
                    {
						ca.CalcWinDensity(p.calcType,cX,p.calcY,p.calcXLen,p.calcYLen,noIndivid);
						if(noIndivid>3)
						{
	            			ca.CalcUnivRipleyK(p.calcType,cX,p.calcY,p.calcXLen,p.calcYLen,polspe,noIndivid,	p.rpkNoIntervals, p.rpkLenInterval);
           					pof << "\t";
            				ca.PrintUnivRipleyK(pof,polspe);
            				break;
						}
                		cX += 2*p.calcXLen;
                    }
                    if(noIndivid<4)
                    {
                    	pof << "\t-5.0" << "\t-9.0" << "\t-13.0"  << "\t-17.0" << "\t-21.0";
                    }
				}
            	pof << endl;
            }
        	pomacEarlyBreak=false;
        	pof.close();
		}

#ifdef GRAPHICS
		if( p.gr=='Y' )
        {
            getch();
			EGraph();
		}
#endif
			
	}
	return 0;
	//Fortify_LeaveScope();
}



int ReadParms(char * pFile, CaInvaParms &p)
{
	char buff[100];
	ifstream parms(pFile);
    if( !parms )
    	{
    	cerr << "Can't open parms file" << endl;
        exit(1);
        }
	
	while( !parms.eof() )
	{
		parms >> buff;
		
		if(strcasecmp(buff,"nRuns")==0) // numero de Corridas
    	{
    		parms >> p.nRuns;
    	}
    	else if(strcasecmp(buff,"nEvals")==0) // numero de evaluaciones (max time)
    	{
    		parms >> p.nEvals;
    	}
    	else if(strcasecmp(buff,"rndSeed")==0) // random number seed
    	{
    		parms >> p.rndSeed;
    	}
    	else if(strcasecmp(buff,"gr")==0) // Graficos? N/Y
    	{
    		parms >> p.gr;
    		p.gr = toupper( p.gr );
    	}
	    else if(strcasecmp(buff,"Inter")==0) // Intervalo de Graficacion/Muestreo
	    {
    		parms >> p.inter;
	    }
	    else if(strcasecmp(buff,"Init")==0) // tiempo de Inicio de muestreo
	    {
    		parms >> p.init;
	    }
	    else if(strcasecmp(buff,"de")==0) // Calcula densidad por especie S/N
	    {
    		parms >> p.de;
			p.de = toupper( p.de );
	    }
	    else if(strcasecmp(buff,"sa")==0) // Salva seed en cada muestreo
	    {
    		parms >> p.sa;
			p.sa = toupper( p.sa );
	    }
	    else if(strcasecmp(buff,"baseName")==0) // Salva seed en cada muestreo
	    {
    		parms >> p.baseName;
	    }
	    else if(strcasecmp(buff,"idrPal")==0) // utiliza una palette de Idrisi
	    {
    		parms >> p.idrPal;
	    }
	    else if(strcasecmp(buff,"dimX")==0) // Dimension X
	    {
    		parms >> p.dimX;
	    }
	    else if(strcasecmp(buff,"dimY")==0) // Dimension Y
	    {
    		parms >> p.dimY;
	    }
	    else if(strcasecmp(buff,"noSpecies")==0) // Nro de especies
	    {
    		parms >> p.noSpecies;
	    }
	    else if(strcasecmp(buff,"seedFile")==0) // Archivo de sembrado
	    {
    		parms >> p.seedFile;
	    }
	    else if(strcasecmp(buff,"setSeedFile")==0) // Archivo de sembrado al azar
	    {
    		parms >> p.setSeedFile;
	    }
	    else if(strcasecmp(buff,"runLineParms")==0) // Corre con parametros de la linea de comando
	    {
    		parms >> p.runLineParms;
    		p.runLineParms = toupper( p.runLineParms );
	    }
	    else if(strcasecmp(buff,"calcSpectrum")==0) // Calcula el espectro
	    {
    		parms >> p.calcSpectrum;
    		p.calcSpectrum = toupper( p.calcSpectrum );
	    }
	    else if(strcasecmp(buff,"calcUnivRipleyK")==0) // Calcula RipleyK univariado
	    {
    		parms >> p.calcUnivRipleyK;
    		p.calcUnivRipleyK = toupper( p.calcUnivRipleyK );
	    }
	    else if(strcasecmp(buff,"pomac")==0) // Corre para el metodo pomac
	    {
    		parms >> p.pomac;
    		p.pomac = toupper( p.pomac );
	    }
	    else if(strcasecmp(buff,"pomacFile")==0) // archivo para pomac
	    {
    		parms >> p.pomacFile;
	    }
	    else if(strcasecmp(buff,"calcX")==0) // Coordenada X de la ventana para calculo de SPE
	    {
    		parms >> p.calcX;
	    }
	    else if(strcasecmp(buff,"calcY")==0) // Coordenada Y de la ventana para calculo de SPE
	    {
    		parms >> p.calcY;
	    }
	    else if(strcasecmp(buff,"calcXLen")==0) // Ancho de la ventana para calculo de SPE
	    {
    		parms >> p.calcXLen;
	    }
	    else if(strcasecmp(buff,"calcYLen")==0) // Profundidad de la ventana para calculo de SPE
	    {
    		parms >> p.calcYLen;
	    }
	    else if(strcasecmp(buff,"calcType")==0) // Typo de individuos AD o JU para calculo de SPE
	    {
    		parms >> p.calcType;
	    }
	    else if(strcasecmp(buff,"rpkNoIntervals")==0) // Nro de Intervalos para calculo de RipleyK
	    {
    		parms >> p.rpkNoIntervals;
	    }
	    else if(strcasecmp(buff,"rpkLenInterval")==0) // Long del Intervalo para calculo de RipleyK
	    {
    		parms >> p.rpkLenInterval;
	    }
	    else if(buff!=NULL)
		{
			cerr << "Error in input file, unrecognized token" << endl;
			exit(1);
		}
			
	}
	return 0;
}


