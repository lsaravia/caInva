//	Clase Base : CABase
//
//	Clase Asociada : Specie
//
//
//  VERSION 20000516
//
//
//          Elimine todos los tipos de dispersion excepto el E
//			Con eso hay 2 parametros que no tienen efecto
//				DispersalNorm
//              ParmDispersal
//
//				NeighSurvivalSlopeAd=Tasa de competencia entre adultos y juveniles
//				NeighSurvivalSlopeJu=Tasa de competencia entre juveniles
//				ShadeTolerance = Tasa de aumento de la escala de influencia de los adultos con la edad
//				MaximunSurvivalDistance = Distancia maxima para calculo de mortalidad de Juveniles
//						Debido a los adultos


#include "caInva.hpp"
#include <string>

//#include "fortify.h"

#include "bgi.hpp"
//#include "spectral.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <ctime>
//#include <conio.h>
//#include <ctype.h>


	
void CAInva::Init( unsigned numSp, int dimX, int dimY, int rr )
{
	CABase::Init(numSp, dimX, dimY,rr);

	C.resize(dimX,dimY);

	ActualSp = 0;
	NumLineParms = 0; // Numero identificador de line para archivo de parametros de linea
	NewRun = true;

	Sp	= new SpecieAge[NumSpecies];
	TimeType = 'D';
	NoHabitat = 255;

	
}


void CAInva::EvaluateED() // Discreto, y comenzando de una esquina al azar
{
    int x,y;
	A = T % 2;
	N = (T+1) % 2;
	switch( Rand(3) )
	{
		case 0:
			for(x=0; x<DimX; x++)
				for(y=0; y<DimY; y++)
					EvalCell(x,y);
			break;

		case 1:
			for(x=DimX-1; x>=0; x--)
				for(y=0; y<DimY; y++)
					EvalCell(x,y);
			break;

		case 2:
			for(x=0; x<DimX; x++)
				for(y=DimY-1; y>=0; y--)
					EvalCell(x,y);
			break;

		case 3:
			for(x=DimX-1; x>=0; x--)
				for(y=DimY-1; y>=0; y--)
					EvalCell(x,y);
	}

	T++;
	EmptyPrevious();

}

void CAInva::Evaluate()
	{
	int x,y,i,max;

	if( TimeType == 'D' )
		{
		A = T % 2;
		N = (T+1) % 2;
		}
	else
		N = 0;


	switch(EvalType)
	{
		case 'E':
			switch( Rand(4) )
			{
				case 0:
					for(x=0; x<DimX; x++)
						for(y=0; y<DimY; y++)
							EvalCell(x,y);
					break;

				case 2:
					for(x=DimX-1; x>=0; x--)
						for(y=0; y<DimY; y++)
							EvalCell(x,y);
					break;

				case 3:
					for(x=0; x<DimX; x++)
						for(y=DimY-1; y>=0; y--)
							EvalCell(x,y);
					break;

				case 1:
					for(x=DimX-1; x>=0; x--)
						for(y=DimY-1; y>=0; y--)
							EvalCell(x,y);
			}
			break;
		case 'A':
			for(x=0; x<DimX; x++)
				for(y=0; y<DimY; y++)
					EvalCell(x,y);
			break;

		case 'B':
			EvalCellB1(x,y);
			break;

		case 'D':
			max = DimX*DimY;
			for(i=0; i<max; i++)
			{
				x = Rand(DimX);
				y = Rand(DimY);
				EvalCell(x,y);
			}
			break;
	}
	T++;


	if( TimeType == 'D' )
		EmptyPrevious();
	}

void CAInva::EvalCellB1(int &x,int &y)
	{
	int max = DimX*DimY;
	CellEvent * pos = new CellEvent[max];
	int p=0;

	for(x=0; x<DimX; x++)
		for(y=0; y<DimY; y++)
			{
			pos[p].Elem(x,y);
			p++;
			}
	JumbleXY(pos, max);
	for(p=0;p<max;p++)
	   {
	   x = pos[p].Elem(0);
	   y = pos[p].Elem(1);
	   EvalCell(x,y);
	   }
	delete pos;
	}


void CAInva::JumbleXY(CellEvent * pos, int max)
	{
	int p,p1,p2,m;
	CellEvent tmp;
	m = DimX*DimY*3;
	for(p=0; p<m; p++)
		{
		p1 = Rand(max);
		p2 = Rand(max);
		while(p1==p2)
			{
			p1 = Rand(max);
			p2 = Rand(max);
			}

		tmp=pos[p1];
		pos[p1] = pos[p2];
		pos[p2] = tmp;
		}
	}


inline void CAInva::EvalCell(int x,int y)
{
	if( C(x,y).Habitat != NoHabitat )
		if( Survival(x,y) )
			Growth(x,y);
}


void CAInva::Growth(int x,int y)
{
	int dx=0,dy=0;
	unsigned ne;
	char dn;
	float rnd,dis,dd,ang,pd;

	ActualSp = Elem(x,y,A);
	
	if(ActualSp>0 )
	{
		int actSp = ActualSp-1;
		ActualAge = C(x,y).Age;
		int isAdult=C(x,y).Adult;

		if( isAdult )
		{
			dd = Sp[actSp].MeanDispersalDistance; 	// Distancia Media en el caso Exponencial
													// 			Maxima en Euclidiana o uniforme
//			pd = Sp[actSp].ParmDispersal; // Parametro n de la Gamma
			ne = Sp[actSp].NRecruits;
			dn = Sp[actSp].DispersalNorm;

			for(int i=0; i<ne; i++)
			{
				do
				{
					switch( dn )
					{
						// Euclidian Norm (Norma 2)
						// dd = Distancia MAXIMA
						case 'E':
							dx = Rand(dd*2) - dd;
							dis = sqrt( dd * dd - dx * dx );
							dy = Rand(dis*2) - dis ;
							break;
					
						// Exponencial    
						case 'X':
							dis= -log(Rand())/dd;
							rnd = Rand();
							ang = rnd * Pi2;
							dx = cos( ang ) * dis ;
							dy = sin( ang ) * dis ;
							break;
					}

					
				}while( dx==0 && dy==0 );
				
				Colonize(x+dx,y+dy);
			}
		}
	}
}



void CAInva::Colonize(int x,int y)
{

#ifdef PERIODIC_BOUNDARY
//	Periodic boundary conditions
	x = (x + DimX) % DimX;
	y = (y + DimY) % DimY;
	if( x<0 )
		x = (x + DimX) % DimX;
	if( y<0 )
		y = (y + DimY) % DimY;

#else
// Disipattive boundary conditions
//
	if( (x>=DimX || y>=DimY || x<0 || y<0) )
		return;
#endif 
//
//		Collision rules:
//		 	if the "actual cell" or the "next cell" is occupied the propagule is lost
//	
	if( C(x,y).Habitat != NoHabitat )
	{
	if( Elem(x,y,A)==0 )
		if( Elem(x,y,N)==0 )
			Elem(x,y,N)=ActualSp;
	}
}


int CAInva::Survival(int x, int y)
{
	int kn=0,dx,dy,nj=0,na=0;
	ActualSp = Elem(x,y,A);
	ActualAge = C(x,y).Age;
	bool isAdult = C(x,y).Adult;
        
	double Pm=0;
    
	if(ActualSp>0)
	{
		int actSp = ActualSp-1;
		int superficie=0;
        int ymax=-1;
        int xdif=0;
        int xx=0,yy=0;


		if( isAdult )
		{
//			if( ActualAge > Sp[actSp].MaximunAge)
//			{
//				C(x,y).Elem(0,0,0,0);
//				return 0;
//			}

			kn = Sp[actSp].NeighSurvivalDistance+ Sp[actSp].ShadeTolerance*(ActualAge-Sp[actSp].AdultAge);
			if( kn>Sp[actSp].MaximunSurvivalDistance)
				kn = Sp[actSp].MaximunSurvivalDistance;

            na=1;
            superficie = M_PI * kn * kn;
			if(kn!=0)
			{
				for(dx=x-kn; dx <= x+kn; dx++ )
				{
					xdif=dx-x;
//					ymax = static_cast<int>(sqrt(kn*kn - xdif*xdif));
					ymax = sqrt(static_cast<double>(kn*kn - xdif*xdif));
					for(dy=y-ymax; dy<=y+ymax; dy++)
					{
                    
#ifdef PERIODIC_BOUNDARY
						xx = (dx + DimX) % DimX;
						yy = (dy + DimY) % DimY;
                       
#else	// Dissipative boundary condition
						xx = dx;
						yy = dy;

						if( !(xx>=DimX || yy>=DimY || xx<0 || yy<0) )
#endif						
						{
							if( C(xx,yy).Habitat != NoHabitat )
                            {
                            	//superficie++;
    							if( xx!=x || yy!=y )
    							{
    								int spc = Elem(xx,yy,A);
    								if( spc>0)
    								{
    									if( !C(xx,yy).Adult )
    									{
											Pm = Sp[spc-1].NeighSurvivalSlopeAd*na;
											if( Rand() < Pm )
												C(xx,yy).Elem(0,0,0,0);
    									}
    								}
    							}
							}
						}
					}
				}
			}
			Pm= Sp[actSp].PAdultMortality;
		}
		else
		{
            // Calcula la cantidad de adultos y juveniles que hay en
            // el entorno definido por NeighSurvivalDistance 
            //
			Pm = Sp[actSp].PJuvenilMortality;			
			kn = Sp[actSp].NeighSurvivalDistance;
			if(kn!=0)
			{
				for(dx=x-kn; dx <= x+kn; dx++ )
				{
					xdif=dx-x;
					ymax = static_cast<int>(sqrt(static_cast<double>(kn*kn - xdif*xdif)));
					for(dy=y-ymax; dy<=y+ymax; dy++)
					{
                    
#ifdef PERIODIC_BOUNDARY
						xx = (dx + DimX) % DimX;
						yy = (dy + DimY) % DimY;
                       
#else	// Dissipative boundary condition
						xx = dx;
						yy = dy;

						if( !(xx>=DimX || yy>=DimY || xx<0 || yy<0) )
#endif						
						{
							if( C(xx,yy).Habitat != NoHabitat )
                            {
                            	superficie++;
    							if( xx!=x || yy!=y )
    							{
    								int spc = Elem(xx,yy,A);
    								if( spc>0)
    								{
    									if( C(xx,yy).Adult )
    										na++;
	   									else
    									{
//    										int ag = C(xx,yy).Age;
//
//		Suma los juveniles mayores o iguales en edad
//    										
//    										if( ag >= ActualAge )
//    											nj++;
//		Suma Todos los juveniles 
//    										
   											nj++;
    									}
    								}
    							}
							}
						}
					}
				}
				Pm = Sp[actSp].PJuvenilMortality + Sp[actSp].NeighSurvivalSlopeJu*nj;
			}
		}
		
		if( Rand() < Pm )
		{
			C(x,y).Elem(0,0,0,0);
			return 0; 	//no sobrevivio o no hay nadie en ese lugar
		}
		else
		{
			C(x,y).Transfer(N,A);
			C(x,y).Age++;
			if( C(x,y).Age >= Sp[actSp].AdultAge )
				C(x,y).Adult= true;
			//
			return 1;					// Sobrevivio
		}
	}
	else		// External Colonization
	{
		Pm=Rand();
		for( na=0; na<NumSpecies; na++)
		{
			if( Pm < Sp[na].PColonization )
			{
				ActualSp = na+1;
				Colonize(x,y);
				break;
			}
		}
	}
		
	return 0;
}


void CAInva::Reset()
{
//	rndSeed= time(NULL);
//	r250_init(rndSeed);
	C.fill( ActualCell.Elem(0,0,0) );
	A = 0;
	N = 1;
	T = 0;
	ActualSp = 0;
	NewRun=true;
}


