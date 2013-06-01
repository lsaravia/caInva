#ifndef __CAINVA_HPP
#define __CAINVA_HPP
#include <string>
#include <sstream>
#include <fstream>

#include "cabase.hpp"
#include "smattpl.hpp"
#include "cellInva.hpp"
#include "cell.hpp"
#include "ran.h"

using namespace std;

struct DensVar
	{
	long AdultCant;
	long JuvenilCant;
	long TotalCant;
	float AdultDensity;
	float JuvenilDensity;
	float TotalDensity;
	float AdultVar;
	float JuvenilVar;
	float TotalVar;
	};


class SpecieAge
	{
	public:

	SpecieAge() { Init(); };
	void Init(){ MaximunSurvivalDistance=0; 
						AdultAge=0;
						PJuvenilMortality=0;
						PAdultMortality=0;
						MeanDispersalDistance=0;
						NRecruits=0;
						DispersalNorm=0;
						ParmDispersal=0;
						NeighSurvivalDistance=0;
						NeighSurvivalSlopeAd=0;
						NeighSurvivalSlopeJu=0;
						ShadeTolerance=0;
						PColonization=0;
		};

	int Scan(char * buff);
	istringstream & Scan(istringstream & ins,int &sp);
	
	unsigned AdultAge,
			 NRecruits,
			 NeighSurvivalDistance;
			 
	
	float PJuvenilMortality,
		PAdultMortality,
		MeanDispersalDistance,
		ParmDispersal,
		NeighSurvivalSlopeAd,
		NeighSurvivalSlopeJu,
		MaximunSurvivalDistance,
		PColonization,
		ShadeTolerance;

	char DispersalNorm;
	string spName;	

	friend ostream & operator << (ostream & o, SpecieAge & sp);
	};


//
//
class CellEvent : public Cell
	{
	public:
		char event;
	CellEvent() : Cell() { event =0;};
	CellEvent & operator=(const CellEvent &src){ NumSpecie[0] = src.NumSpecie[0];  NumSpecie[1] = src.NumSpecie[1]; event=src.event; return *this;};
	CellEvent(const CellEvent &src){ NumSpecie[0] = src.NumSpecie[0];  NumSpecie[1] = src.NumSpecie[1]; event=src.event;};
	};


//
//
class CAInva : public CABase
	{
	protected:
		SpecieAge * Sp;
		simplmat <CellInva> C;
		CellInva ActualCell ;
		int ActualSp;
		int ActualAge;
		char EvalType;
		char TimeType;
		int EventType;
		ifstream InLineFile;	// Archivo para parametros de linea 
		int NumLineParms;	// Numero identificador de line para archivo de parametros de linea
		bool NewRun;        // True cuando empieza una nueva corrida, lo resetea PrintDensityAge
		int NoHabitat;

        Ranf1 ran;

	void JumbleXY(CellEvent * pos, int max);
	void EvalCellB1(int &x,int &y);
		
	public:

	CAInva( unsigned numSp, int dimX, int dimY, int rr=0 ) : CABase(numSp,dimX,dimY) { Init(numSp,dimX,dimY,rr); };
	CAInva() {};
    ~CAInva() { delete []Sp;
    			InLineFile.close();
    			};
	void Init(unsigned numSp, int dimX, int dimY, int rr=0 );

	uchar & Elem(int x, int y, unsigned O) { return C(x,y).Elem(O); };

	void Growth(int x,int y);

	int  Survival(int x,int y);
	int  ReadParms(char * file);
	int  ReadLineParms(string & lineParms);
	int  ReadFileLineParms( char * file);
	void Reset();

	void EvalCell(int x,int y);
	void EmptyPrevious(){ for(int x=0;x<DimX;x++)
									for(int y=0;y<DimY;y++)
										C(x,y).Elem(A)=0;
									};
	void Colonize(int x,int y);

	void Evaluate();
	void EvaluateED(); // Discreto, y comenzando de una esquina al azar

	void SetN( unsigned n ){ N= n;};
	void PrintMap(unsigned O);

	void PrintGraph();
	void InitGraph();
	int ReadSeed( char * fname);
	int ReadIdrisi( char * fname, char * dataType );
	int SaveSeed(const char * fname, int option=0);
	int SaveSeed(const char * fname,
			int xTopLeft, int yTopLeft, int xBottomRight, int yBottomRight, int option=0);

	void ReadSetSeed( char * fname);
	void RandomSetSeed(int sp,unsigned age, int no, int minX,int habitat=0);
	int Rand(int num);
	double Rand();

	int  PrintDensityAge(string const & fname,char *iname, int maxT);
	int  PrintDensityAge(ostream & ost, int maxT);
	
	int  PrintSpectrum(string const & fname,string const &type,int xTL, int yTL, int xLen, int yLen,simplmat<double> & polspe,
							long const & noIndivid);

	int  PrintSpectrum(ostream & fb,simplmat<double> & polspe);
	
	int  CalcSpectrum(string const &type,int xTL, int yTL, int xLen, int yLen,simplmat<double> & polper, long & noIndivid);

	int  CalcUnivRipleyK(string const &type,int xTL, int yTL, int xLen, int yLen,simplmat<double> & ripk,
							long const & noIndivid, int const &noIntervals, int const & lenIntervals);
	int  PrintUnivRipleyK(string const & fname,string const &type,int xTL, int yTL, int xLen, int yLen,simplmat<double> & ripk,
							long const & noIndivid);
	int  PrintUnivRipleyK(ostream & fb,simplmat<double> &ripk);

	int PrintUnivG(string const & fname,string const &type,int xTL, int yTL, int xLen, int yLen,simplmat<double> & ripk,
							long const & noIndivid);
	int PrintUnivG(ostream & fb,simplmat<double> &ripk);
	
	int  PrintParams(ostream & fb);
	int  PrintParamsPomac(ostream & fb,bool titles=false);
	int  ReadPomacParms(string const & fpom);

	int  CalcWinDensity(string const &type,int xTL, int yTL, int xLen, int yLen, long & noIndivid);

//	void WindowStats( int xTopLeft, int yTopLeft, int xBottomRight, int yBottomRight,
//						DensVar & r,int specie);
	void VelRegress(int specie, char * iname, char * oname, char * pname);
	int InitParms();
	};

inline double CAInva::Rand() { 
	//return ranf();
	return ran.doub(); 
	};

inline int CAInva::Rand(int num) {
		return (ran.int64() % (num+1)); // between 0 and num inclusive 
		//return ignuin(0,num);
        };


#endif
