//	Clase Base : CABase
//
//	Clase Asociada : Specie
//
//

#include "caInva.hpp"
#include <string>
#include <math.h>

#include "Spectral.h"
#include "RipleyTriangle.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include "fortify.h"

#ifndef  __GO32__
#define log2(x) log(x)/log(2)
#endif


int CAInva::PrintDensityAge(string const & fname,char *iname, int maxT)
{
	float disadx=0,disjux=0;
//	static bool privez=true;
	bool priPro=false;
	unsigned long tot,totad,totju,tottotju=0,tottot=0;
	int maxAge=100;
	if(Sp[0].PAdultMortality!=0)
	{
		maxAge= 1/Sp[0].PAdultMortality*2;
    	if(maxAge>1000)
    		maxAge = 1000;
	}
    
	simplmat <long> dens(maxAge,NumSpecies);
	simplmat <long> totadx(NumSpecies,1);
	simplmat <long> totjux(NumSpecies,1);
	simplmat <long> maxadx(NumSpecies);
	simplmat <long> maxjux(NumSpecies);
	static simplmat <float> veladx(NumSpecies);
	static simplmat <float> veljux(NumSpecies);
	static simplmat <float> proDenAd(NumSpecies,1);
	static simplmat <float> proDenJu(NumSpecies,1);
	static double proNum=0;
	int e;
    unsigned a;

	fstream fb,fden,fage;
	string name=fname;
	name = name + ".Txt";
	fb.open( name.c_str(), ios::in );
	if(!fb )
		priPro=true;
   fb.close();
   fb.clear();
	fb.open( name.c_str(), ios::out | ios::app);
	if( !fb )
	{
		cerr << "Cannot open file: " << name << endl;
		return 0;
	}
	if( priPro )
	{
		fb << "RunNo\tSpecie\tDenAd\tDenJu\tVelAd\tVelju" << endl;
		priPro = false;
	}

	name = fname;
	name+= ".out";
	
	priPro=false;
	fden.open( name.c_str(), ios::in );
	if(!fden )
		priPro=true;
	fden.close();
	fden.clear();
	fden.open( name.c_str(), ios::out | ios::app );
	if( !fden )
		{
		cerr << "Cannot open file: " << name << endl;
		return 0;
		}

	if( priPro )
	{
		fden << "runNo\tAge\tSpecie";
		fden << "\tTot.sp\tAdult\tRenov\tDist.Ad\tDist.Juv\tMaxAdx\tMaxJux\tVelAdx\tVelJux" << endl;
	}
	

	name = fname;
	name+= ".age";
	
	priPro=false;
	fage.open( name.c_str(), ios::in );
	if(!fage )
		priPro=true;
	fage.close();
	fage.clear();
	fage.open( name.c_str(), ios::out | ios::app );
	if( !fage )
	{
		cerr << "Cannot open file: " << name << endl;
		return 0;
	}

	if( priPro )
	{
		fage << "runNo\tAge\tSpecie";
		for( a=0; a<maxAge; a++)
		{
			fage << "\t" << a;
		}
		fage <<  endl;
	}


	dens.fill( 0 );
	totadx.fill( 0 );
	totjux.fill( 0 );
	maxjux.fill( 0 );
	maxadx.fill( 0 );

	if( NewRun )
	{
		veladx.fill( 0 );
		veljux.fill( 0 );
		proDenAd.fill( 0 );
		proDenJu.fill( 0 );
		NewRun = false;
        proNum = 0;
	}

	int i,j;
	for( i=0; i<DimY; i++)
		for( j=0; j<DimX; j++)
			{
			e=(C(j,i).Elem(N))-1;
			if( e >= 0 )
				{
				a=C(j,i).Age;
				if(a>=maxAge)
					a=maxAge-1;

				dens.elem(a,e)++;

				if( a>=Sp[e].AdultAge )
					{
					if( j >maxadx(e) )
						maxadx(e)= j;
					totadx(e) += j;
					}
				else
					{
					totjux(e) += j;
					if( j >maxjux(e) )
						maxjux(e)= j;
					}
				}
			}


	tottot=0;
	fden << NumLineParms << "\t" << T;
	for(e=0; e<NumSpecies; e++)
		{
		fden << "\t" << Sp[e].spName;
		fage << NumLineParms << "\t" << T << "\t" << Sp[e].spName;
		tot = 0 ;
		totad = 0;
		totju = 0;
		
		for( a=0; a<maxAge; a++)
			{
			fage << "\t" << dens.elem(a,e);

			tot += dens.elem(a,e);
			if( a>=Sp[e].AdultAge )
				totad+= dens.elem(a,e);
			}
		totju = tot - totad;
		tottotju += totju;
		tottot += tot;

		disjux = totju==0 ? 0 : float(totjux(e))/totju;
		disadx = totad==0 ? 0 : float(totadx(e))/totad;

		fden << "\t" << tot << "\t" << totad << "\t" << totju;
		if( T>(maxT/2) )
		{
			proDenAd(e) += totad;
			proDenJu(e) += totju;
			proNum+=1;
		}

	
		if( T > 0)
			{
			if( maxadx(e)+1 ==DimX && veladx(e)==0 )
				{
				veladx(e) = (float)DimX / T;
				}
			if( maxjux(e)+1 ==DimX && veljux(e)==0  )
				{
				veljux(e) = (float)DimX / T;
				}
			}

		fden << "\t" << setprecision(4) << disadx
			 << "\t" << setprecision(4) << disjux
			 << "\t" << maxadx(e) 
			 << "\t" << maxjux(e) 
			 << "\t" << veladx(e)  
			 << "\t" << veljux(e);

		fage << endl;
		
//		if( e+1 < NumSpecies ) fden << endl;
//		if( NumSpecies == 1 ) fden << endl;
		}

	fden << endl;
	
// Imprime valores promedios de densidad y velocidad en archivo aparte
//
	if( T==maxT )
	{
		proNum /= NumSpecies;
		fb << NumLineParms;
		for(e=0; e<NumSpecies; e++)
			 fb << "\t" << Sp[e].spName << "\t" << proDenAd(e)/proNum << "\t" << proDenJu(e)/proNum <<  "\t" << veladx(e) <<  "\t" << veljux(e);
		fb << endl;
	}
			
	return tottot;
}

// type: AD=adults, JU=Juvenils, AL=All
//
//
int CAInva::CalcSpectrum(string const &type,int xTL, int yTL, int xLen, int yLen,simplmat<double> & polper, long & noIndivid)
{
	simplmat <double> data;
	simplmat <double> dout;
	simplmat <double> rper;		// Rearranged periodogram

	SpectralAnalysis sa;
	Fortify_CheckAllMemory();
	data.resize(yLen,xLen,0.0);
	Fortify_CheckAllMemory();
	polper.fill( 0.0 );
	
	int yBR=yTL+yLen;
	int xBR=xTL+xLen;
	if( xLen<0 || yLen<0 || xTL<0 || yTL<0 || xBR>DimX || yBR>DimY)
	{
		noIndivid=0;
		return 0;
	}

    int i,j,e;
    unsigned a=0;

    noIndivid=0;
	if(type=="AD")
	{
		for( i=yTL; i<yBR; i++)
			for( j=xTL; j<xBR; j++)
			{
				e=(C(j,i).Elem(N))-1;
				if( e >= 0 )
				{
					a=C(j,i).Age;
					if( a>=Sp[e].AdultAge )
					{
//						data(i,j) = 1.0;
						data(i-yTL,j-xTL) = 1.0;
						noIndivid++;
					}
				}
			}
	}
	if(type=="JU")
	{
		for( i=yTL; i<yBR; i++)
			for( j=xTL; j<xBR; j++)
			{
				e=(C(j,i).Elem(N))-1;
				if( e >= 0 )
				{
					a=C(j,i).Age;
					if( a<Sp[e].AdultAge )
					{
						data(i-yTL,j-xTL) = 1.0;
						noIndivid++;
					}
				}
			}
	}
	if(type=="AL")
	{
		for( i=yTL; i<yBR; i++)
			for( j=xTL; j<xBR; j++)
			{
				e=(C(j,i).Elem(N))-1;
				if( e >= 0 )
				{
					data(i-yTL,j-xTL) = 1.0;
					noIndivid++;
				}
			}
	}
	if(noIndivid==0)
		return 0;
		
	int rows=data.getRows();
	int cols=data.getCols();
	if( (rows % 2)!=0 || (cols % 2)!=0)
	{
		cerr << "Error, dimensions must be multiplo of 2" << endl;
		return 0;
	}
	
	double var;
	Fortify_CheckAllMemory();
	if( int(log2(rows))==log2(rows) && int(log2(cols))==log2(cols) )
		var = sa.Spec2D(data,dout); // Calculates the periodogram using FAST FOURIRER
	else
		var = sa.Period2D(data,dout); // Calculates the periodogram

	Fortify_CheckAllMemory();
	sa.Spekout(rows,cols,dout,rper); // Rearranges the periodogram
	Fortify_CheckAllMemory();
	sa.Polar2D(rows,cols,rper,polper); // Calculates the Polar Spectrum

	return 1;
}

// Imprime el espectro-R
//
// 
int CAInva::PrintSpectrum(ostream & fb,simplmat<double> & polspe)
{
	int l,d=polspe.getRows();

    fb << "\t";
	for(l=0;l<d;l++)
 		fb << setw(9) << fixed << showpoint << setprecision(5) << polspe(l,0) << "\t";
 	return 1;
}

int CAInva::PrintSpectrum(string const & fname,string const &type,int xTL, int yTL, int xLen, int yLen,simplmat<double> & polspe,
							long const & noIndivid)
{
	bool priPro=false;
	fstream fb( fname.c_str(), ios::in );
	if(!fb )
		priPro=true;
	fb.close();
	fb.clear();
	fb.open( fname.c_str(), ios::out | ios::app );
	if( !fb )
	{
		cerr << "Cannot open file. " << fname.c_str() << endl;
		return 0;
	}

	if( priPro )
	{
		fb << "RunNo\tStep\tType\txTL\tyTL\txLen\tyLen\tnoIndiv\tR Spectra" << endl;
	}
	fb << NumLineParms << "\t" << T << "\t" << type << "\t" << xTL  << "\t" << yTL  << "\t"
		<< xLen  << "\t" << yLen  << "\t" << noIndivid << "\t";
	PrintSpectrum(fb,polspe);
	fb << endl;

   return 1;
}


// type: AD=adults, JU=Juvenils, AL=All
//
//
int CAInva::CalcUnivRipleyK(string const &type,int xTL, int yTL, int xLen, int yLen,simplmat<double> & ripk,
	long const & noIndivid, int const &noIntervals, int const & lenIntervals)
{
	simplmat<float> x(noIndivid+1,1);
	simplmat<float> y(noIndivid+1,1);
	simplmat<float> k(noIntervals+1);
	simplmat<float> g(noIntervals+1);

	int yBR=yTL+yLen;
	int xBR=xTL+xLen;
	if( xLen<0 || yLen<0 || xTL<0 || yTL<0 || xBR>DimX || yBR>DimY)
	{
		cerr << "Window out of bounds " << endl;
		return 0;
	}
    int i,j,e;
    unsigned a=0;
    x.fill(0.0);
    y.fill(0.0);

    long t=1;
	if(type=="AD")
	{
		for( i=yTL; i<yBR; i++)
			for( j=xTL; j<xBR; j++)
			{
				e=(C(j,i).Elem(N))-1;
				if( e >= 0 )
				{
					a=C(j,i).Age;
					if( a>=Sp[e].AdultAge )
					{
						x(t)=j;
						y(t)=i;
						t++;
					}
				}
			}
	}
	if(type=="JU")
	{
			for( i=yTL; i<yBR; i++)
			for( j=xTL; j<xBR; j++)
			{
				e=(C(j,i).Elem(N))-1;
				if( e >= 0 )
				{
					a=C(j,i).Age;
					if( a<Sp[e].AdultAge )
					{
						x(t)=j;
						y(t)=i;
						t++;
					}
				}
			}
	}
	if(type=="AL")
	{
		for( i=yTL; i<yBR; i++)
			for( j=xTL; j<xBR; j++)
			{
				e=(C(j,i).Elem(N))-1;
				if( e >= 0 )
				{
					x(t)=j;
					y(t)=i;
					t++;
				}
			}
	}
	float * dummy;

	float density = static_cast<float>(noIndivid)/(xLen*yLen);

	ripley_tr_rect( noIndivid, x.pointer(), y.pointer(), xTL, xBR,
					yTL, yBR, 0 , dummy, dummy,
					dummy, dummy, dummy, dummy, noIntervals, lenIntervals,
					g.pointer(), k.pointer(), density);
	float rg,rl,rk;
	ripk.resize(noIntervals,3);
	
	for(i=1;i<=noIntervals; i++)
	{
		rg=g(i)/(density*(Pi*i*i*lenIntervals*lenIntervals-Pi*(i-1)*(i-1)*lenIntervals*lenIntervals));
		rk=k(i)/density;
		rl=sqrt(rk/Pi)-i*lenIntervals;
		ripk(i-1,0)=rg;
		ripk(i-1,1)=rk;
		ripk(i-1,2)=rl;
	}
	return 1;
}

int CAInva::PrintUnivRipleyK(ostream & fb,simplmat<double> &ripk)
{
	int l,d=ripk.getRows();
	
	for(l=0;l<d;l++)
 		fb << setw(9) << fixed << showpoint << setprecision(5) << ripk(l,2) << "\t";
// 		fb.form("%8.5f", ripk(l,2)) << "\t";
 		
 	return 1;
}

int CAInva::PrintUnivG(ostream & fb,simplmat<double> &ripk)
{
	int l,d=ripk.getRows();
	
	for(l=0;l<d;l++)
 		fb << setw(9) << fixed << showpoint << setprecision(5) << ripk(l,0) << "\t";
// 		fb.form("%8.5f", ripk(l,0)) << "\t";
 		
 	return 1;
}


int CAInva::PrintUnivRipleyK(string const & fname,string const &type,int xTL, int yTL, int xLen, int yLen,simplmat<double> & ripk,
							long const & noIndivid)
{
	bool priPro=false;
	fstream fb( fname.c_str(), ios::in );
	if(!fb )
		priPro=true;
	fb.close();
	fb.clear();
	fb.open( fname.c_str(), ios::out | ios::app );
	if( !fb )
	{
		cerr << "Cannot open file. " << fname.c_str() << endl;
		return 0;
	}
	if( priPro )
	{
		fb << "RunNo\tStep\tType\txTL\tyTL\txLen\tyLen\tnoIndiv\tRipley's K L()" << endl;
	}

	fb << NumLineParms << "\t" << T << "\t" << type << "\t" << xTL  << "\t" << yTL  << "\t"
		<< xLen  << "\t" << yLen  << "\t" << noIndivid << "\t";
	PrintUnivRipleyK(fb,ripk);
	fb << endl;
   return 1;
}

int CAInva::PrintUnivG(string const & fname,string const &type,int xTL, int yTL, int xLen, int yLen,simplmat<double> & ripk,
							long const & noIndivid)
{
	bool priPro=false;
	fstream fb( fname.c_str(), ios::in );
	if(!fb )
		priPro=true;
	fb.close();
	fb.clear();
	fb.open( fname.c_str(), ios::out | ios::app );
	if( !fb )
	{
		cerr << "Cannot open file. " << fname.c_str() << endl;
		return 0;
	}
	if( priPro )
	{
		fb << "RunNo\tStep\tType\txTL\tyTL\txLen\tyLen\tnoIndiv\tG Density" << endl;
	}

	fb << NumLineParms << "\t" << T << "\t" << type << "\t" << xTL  << "\t" << yTL  << "\t"
		<< xLen  << "\t" << yLen  << "\t" << noIndivid << "\t";
	PrintUnivG(fb,ripk);
	fb << endl;
   return 1;
}


void CAInva::VelRegress(int specie, char * iname, char * oname, char * pname)
	{
	static int priPro=0;
	fstream fo,fi;
	ostringstream name;
	name << oname << "vel.Txt"  << ends;
	fo.open( name.str().c_str(), ios::in );
	if(!fo )
		priPro=1;
	fo.close();
	fo.clear();
	fo.open( name.str().c_str(), ios::out | ios::app );
	if( !fo )
		{
		cerr << "Cannot open out file. " << name.str() << endl;
		return;
		}
	if( priPro )
		{
		fo << "File\tSp\tVel\tr2\tStErr" << endl;
		priPro = 0;
		}

	fi.open( iname, ios::in );
	if( !fi )
		{
		cerr << "Cannot open in file. " << name.str() << endl;
		return;
		}

	double sumx=0,sumy=0,sumxsq=0,sumysq=0,sumxy=0,max=0;
	double a,b,sdb,r,xbar,ybar;

	a=b=r=sdb=xbar=ybar=0;

	int sp,age,maxAdx,maxJux,privez=0;
	long adult,renov,totSp;
	float distAd,distJuv,velAdx,velJux;

	char buff[256];

	while(!fi.eof() )
		{
		fi.getline(buff,255);
		if( fi.eof() )
			break;

		if( strstr(buff,"Sp")==NULL )
			{
			sscanf(buff,"%d\t%d\t%ld\t%ld\t%ld\t%f\t%f\t%d\t%d\t%f\t%f",
				&sp,&age,&totSp,&adult,&renov,&distAd,&distJuv,
				&maxAdx,&maxJux,&velAdx,&velJux	);

			if( sp==specie )
				{
				if( !privez)
					{
					sumy += maxAdx;
					sumx += age;
					sumxy += long(maxAdx) * age;
					sumysq += long(maxAdx) * maxAdx;
					sumxsq += long(age)*age;
					max=age+1;
					}
				if(maxAdx+1==DimX)
					privez=1;
				}
			}

		else
			{
			if( sumy==0) continue;

			xbar = sumx/max;
			ybar = sumy/max;

			b = (sumxy - max * xbar * ybar)/(sumxsq - max * xbar * xbar );

			a = ybar - ( b * xbar );

			r = (sumxy - max * xbar * ybar)/sqrt((sumxsq - max * xbar * xbar )
					* (sumysq - max * ybar * ybar));
			if( (r*r) < 1 )
				sdb = sqrt( (1 - r*r)*(sumysq - max *ybar * ybar)/
					((max-4)*(sumxsq - max*xbar*xbar)) );
			else
				sdb = 0;

			fo << pname << "\t" << specie << "\t" <<b << "\t" << r*r << "\t" << sdb << endl;

			sumx=0;
			sumy=0;
			sumxy=0;
			sumxsq=0;
			sumysq=0;
			a=b=r=sdb=xbar=ybar=0;
			privez=0;
			}
		}

	xbar = sumx/max;
	ybar = sumy/max;

	b = (sumxy - max * xbar * ybar)/(sumxsq - max * xbar * xbar );

	a = ybar - ( b * xbar );

	r = (sumxy - max * xbar * ybar)/sqrt((sumxsq - max * xbar * xbar )
			* (sumysq - max * ybar * ybar));
	if( (r*r) < 1 )
		sdb = sqrt( (1 - r*r)*(sumysq - max *ybar * ybar)/
			((max-4)*(sumxsq - max*xbar*xbar)) );
	else
		sdb = 0;

	fo << pname << "\t" << specie << "\t" <<b << "\t" << r*r << "\t" << sdb << endl;

	fi.close();
	fo.close();
	}


int CAInva::PrintDensityAge(ostream & ost, int maxT)
{
	unsigned long tot,totad,totju,tottotad=0,tottot=0;

	int maxAge=100;
	if(Sp[0].PAdultMortality!=0)
	{
		maxAge= 1/Sp[0].PAdultMortality*2;
    	if(maxAge>1000)
    		maxAge = 1000;
	}

	simplmat <long> dens(maxAge,NumSpecies);
	static simplmat <float> proDenAd(NumSpecies,1);
	static simplmat <float> proDenJu(NumSpecies,1);
	static double proNum=0;
    static bool noSum=false;

	int e;
   	unsigned a;

	dens.fill( 0 );

	if( NewRun )
	{
    	noSum = false;
		proDenAd.fill( 0 );
		proDenJu.fill( 0 );
		NewRun = false;
        proNum = 0;
	}
	if( !noSum )
    {
		int i,j;
		for( i=0; i<DimY; i++)
			for( j=0; j<DimX; j++)
			{
				e=(C(j,i).Elem(N))-1;
				if( e >= 0 )
				{
					a=C(j,i).Age;
					if(a>maxAge)
						a=maxAge-1;

					dens.elem(a,e)++;

				}
			}


		tottot=0;
		tottotad=0;
        
		for(e=0; e<NumSpecies; e++)
		{
			tot = 0 ;
			totad = 0;
			totju = 0;
		
			for( a=0; a<maxAge; a++)
				{
				tot += dens.elem(a,e);
				if( a>=Sp[e].AdultAge )
					totad+= dens.elem(a,e);
				}
			totju = tot - totad;
			tottotad += totad;
			tottot += tot;
            
			if( T>(maxT/2) )
			{
				proDenAd(e) += totad;
				proDenJu(e) += totju;
				proNum+=1;
			}
	    }
	}

// Imprime valores promedios de densidad y velocidad en archivo aparte
//
	if( T==maxT )
	{
		if(!noSum)
        {
        	noSum= true;
			proNum /= NumSpecies;
        }
        else
        {
		for(e=0; e<NumSpecies; e++)
			ost << "\t" << proDenAd(e)/proNum << "\t" << proDenJu(e)/proNum;
        }
	}
			
	return tottotad;
}


// type: AD=adults, JU=Juvenils, AL=All
//
//
int CAInva::CalcWinDensity(string const &type,int xTL, int yTL, int xLen, int yLen, long & noIndivid)
{
	Fortify_CheckAllMemory();
	
	int yBR=yTL+yLen;
	int xBR=xTL+xLen;
	if( xLen<0 || yLen<0 || xTL<0 || yTL<0 || xBR>DimX || yBR>DimY)
	{
		noIndivid=0;
		return 0;
	}

    int i,j,e;
    unsigned a=0;

    noIndivid=0;
	if(type=="AD")
	{
		for( i=yTL; i<yBR; i++)
			for( j=xTL; j<xBR; j++)
			{
				e=(C(j,i).Elem(N))-1;
				if( e >= 0 )
				{
					a=C(j,i).Age;
					if( a>=Sp[e].AdultAge )
					{
						noIndivid++;
					}
				}
			}
	}
	if(type=="JU")
	{
		for( i=yTL; i<yBR; i++)
			for( j=xTL; j<xBR; j++)
			{
				e=(C(j,i).Elem(N))-1;
				if( e >= 0 )
				{
					a=C(j,i).Age;
					if( a<Sp[e].AdultAge )
					{
						noIndivid++;
					}
				}
			}
	}
	if(type=="AL")
	{
		for( i=yTL; i<yBR; i++)
			for( j=xTL; j<xBR; j++)
			{
				e=(C(j,i).Elem(N))-1;
				if( e >= 0 )
				{
					noIndivid++;
				}
			}
	}
	return 1;
}

