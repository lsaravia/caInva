//	Clase Base : CABase
//
//	Clase Asociada : Specie
//
//

#include "caInva.hpp"
#include <string>
#include <iomanip>
#include <stdio.h>

#ifdef __DJGPP__
#include <conio.h>
#else
//extern int getch(void);
#define getch() GrKeyRead()
#define kbhit() GrKeyPressed()
#include <grxkeys.h>
#endif




#include "fortify.h"
#include "bgi.hpp"

int SpecieAge::Scan(char * buff)
	{
	int sp=-1;
	istringstream ins(buff);
	Scan(ins,sp);
	return sp;						
	}

istringstream & SpecieAge::Scan(istringstream & ins,int &sp)
	{
    float mAge,aAge,mdDist,nRecr,nsDist;
 	ins >> spName >>
		sp >>
// 		MaximunSurvivalDistance >>
		mAge >>
// 		AdultAge >>
 		aAge >>
		PJuvenilMortality >>
		PAdultMortality >>
//		MeanDispersalDistance >>
		mdDist >>
		ParmDispersal >>
//		NRecruits >>
		nRecr >>
		DispersalNorm >>
//		NeighSurvivalDistance >>
		nsDist >>
		NeighSurvivalSlopeAd >>
		NeighSurvivalSlopeJu >>
		ShadeTolerance >>
		PColonization;
        
	MaximunSurvivalDistance	= mAge;
	AdultAge	= aAge;
	MeanDispersalDistance = mdDist;
	NRecruits 	= nRecr;
	NeighSurvivalDistance = nsDist;
    
	return ins;
	}

ostream & operator << (ostream & o, SpecieAge & sp)
{
	o << sp.spName << "\t" <<
 		sp.MaximunSurvivalDistance << "\t" <<
 		sp.AdultAge << "\t" << 
		sp.PJuvenilMortality << "\t" <<
		sp.PAdultMortality << "\t" <<
		sp.MeanDispersalDistance << "\t" << 
		sp.ParmDispersal << "\t" <<
		sp.NRecruits << "\t" <<
		sp.DispersalNorm << "\t" <<
		sp.NeighSurvivalDistance << "\t" <<
		sp.NeighSurvivalSlopeAd << "\t" <<
		sp.NeighSurvivalSlopeJu << "\t" <<
		sp.ShadeTolerance << "\t" <<
		sp.PColonization << "\t";
	return o;
}

void CAInva::InitGraph()
	{
#ifdef GRAPHICS
   	IGraph(DimX,DimY,"qual256.smp");

//	CABase::InitGraph();
	char buff[20];

	for( int i=0; i<NumSpecies; i++)
		{
			sprintf(buff,"A%d",i);
			GLabel(buff,i*2+1);
			sprintf(buff,"J%d",i);
			GLabel(buff,i*2+2);
		}
#endif
	}

// Shows the spatial pattern in graphics mode
// 
//
void CAInva::PrintGraph()
{
#ifdef GRAPHICS	
	char sa, baseName[9];
	ostringstream name;
	int sp=0;

	for(int i=0; i<DimX; i++)
	{
		for(int j=0;j<DimY;j++)
		{
			//PPix(i,j,C(i,j).Elem(N));
			if( C(i,j).Habitat == NoHabitat )
				PPix(i,j,200 );
			else
           	{
				sp = C(i,j).Elem(N);
				if(sp>0)
					if( C(i,j).Age >= Sp[sp-1].AdultAge )
						PPix(i,j,(sp-1)*2+1);  
					else
						PPix(i,j,(sp-1)*2+2);
				else
					PPix(i,j,0);
			}
		}
	}
	if( T == 0 )
		sa = getch();
	if( kbhit() )
	{
            
		sa=toupper(getch());
		if( sa=='S')
		{
			EndGraph();
			cerr << "Tiempo :" << T << endl;
			cerr << "Salva archivo sed=S, con habitat=H, nada=N ? :";
			sa=toupper(getch());
			if( sa=='S' || sa=='H' )
			{
				cerr << "\nIngrese nombre BASE de archivo : ";
				cin.width(6);
				cin >> baseName;
				name << baseName << T << ".sed" << ends;
				if( sa=='S' )
					SaveSeed( name.str().c_str() );
				else
					SaveSeed( name.str().c_str(), 1 );
			}
			
			InitGraph();
		}
		else
			getch();
	}
	//getch();
#endif
}

void CAInva::RandomSetSeed(int sp,unsigned age, int no, int minX,int habitat)
{
	int rx,ry;
	int i;
	for( i=0; i<no; i++)
	{
		while( true )
		{
			if( minX>0 )
				rx=Rand(minX-1);
			else
				rx=Rand(DimX-1);
			ry=Rand(DimY-1);

			if( C(rx,ry).Habitat != NoHabitat )
				if( C(rx,ry).Habitat == habitat )
					if( C(rx,ry).Elem(A) == 0 )
					{
						C(rx,ry).Elem(A) = sp+1;
						C(rx,ry).Age = age;
						if( age >= Sp[sp].AdultAge )
							C(rx,ry).Adult=true;
						break;
					}
		}
	}
}

void CAInva::ReadSetSeed(char * file)
{
	FILE *in;
	char buff[255];

	int sp=0,age=0,cant=0,minx=0,hab=0;


	if ((in = fopen(file, "rt")) == NULL)
	{
		fprintf(stderr, "Cannot open set file.\n");
		exit(1);
	}

	fgets(buff,80,in);
	do
	{

		if( sscanf(buff,"%i %i %i %i %i",&sp,&age,&cant,&minx,&hab) != 5 || sp>=NumSpecies )
        {
			fprintf(stderr, "Error in set file.\n");
			break;
		}
		RandomSetSeed(sp,age,cant,minx,hab);
	}
	while( fgets(buff,80,in)!=NULL );
	fclose(in);

}

int CAInva::ReadSeed( char * fname)
{
	FILE *in;
	char buff[255];
	int dx,dy;
	int spe,tipo;

	if ((in = fopen(fname, "rt")) == NULL)
	{
		fprintf(stderr, "Cannot open Seed file.\n");
		return 1;
	}
	fgets(buff,255,in);
	sscanf(buff,"%d %d",&dx,&dy);
	if( dx!=DimX || dy!=DimY )
	{
		fprintf(stderr, "Wrong dimension for Seed file.\n");
		exit(1);
	}

	while( fgets(buff,4,in)!= NULL )
	{
		if( strncmp(buff,"SP",2)==0 )
			tipo = 0;
		else if(strncmp(buff,"AG",2)==0 )
			tipo = 1;
		else if(strncmp(buff,"HAI",3)==0 )
		{
			fgets(buff,10,in);
			fgets(buff,20,in);
            char * nl=strchr(buff,'\n');
            *nl =  0;
			ReadIdrisi(buff,"HA");
            continue;
		}
		else if(strncmp(buff,"HA",2)==0 )
			tipo = 2;
		else
			continue;

		for(dy=0;dy<DimY; dy++)
			for(dx=0;dx<DimX; dx++)
			{
				int ret = fscanf(in,"%i",&spe);
				if( ret == 0 || ret ==EOF )
				{
					cerr << "Seed File invalid.\n";
					exit(1);
				}

				switch (tipo) {
					case 0:
						C(dx,dy).Elem(A) = spe;
						break;
					case 1:
						C(dx,dy).Age = spe;
						break;
					case 2:
						C(dx,dy).Habitat = spe;
						break;
					default:
						cerr << "Seed File invalid.\n";
						exit(1);
				}

			}
	}

	for(dy=0;dy<DimY; dy++)
		for(dx=0;dx<DimX; dx++)
			{
				int sp = C(dx,dy).Elem(A);
				if(sp>0)
					if( C(dx,dy).Age >= Sp[sp-1].AdultAge )
						C(dx,dy).Adult=true;
						
			}
	fclose(in);
	return 1;

}

int CAInva::ReadIdrisi( char * fname, char * dataType )
{
	int pos=0;
	string dname,iname;
	iname = fname;

	if( (pos=iname.find(".img")) > 0 )
		fname[pos]=0;
	else
		iname += ".img";
	
	dname = fname;
	dname += ".doc";
	
	char buff[256],type[10], *ptr;
	ifstream in;
	int dx=0,dy=0;

	in.open(dname.c_str());
	if( !in )
	{
		cerr << "Cannot open doc file." << endl;
		return 1;
	}

	while( !in.eof() )
	{
		in.getline(buff,255);
		
		ptr =strstr(buff,"columns");
		if( ptr!=NULL )
			dx = atoi(ptr+13);

		ptr=strstr(buff,"rows");
		if( ptr!=NULL )
			dy = atoi(ptr+13);

		ptr=strstr(buff,"data type");
		if( ptr!=NULL )
		{
			strncpy(type,ptr+14,10);
			type[9]='\0';
   		}
        ptr=strstr(buff,"file type");
		if( ptr!=NULL )
		{
	        ptr=strstr(buff,"ascii");
            if( ptr!=NULL )
            {
            	cerr << "ASCII File type not supported" << endl;
                return 1;
            }
		}

	}

	in.close();

	if( dx!=DimX || dy!=DimY )
	{
		cerr << "Wrong dimension for idrisi file!" << endl;
		exit(1);
	}

	int spe=0;
	// unsigned
    char echa=0;
//	float *eflo= data.pointer();
//	float eflo=0;
    int eint=0;
	string dt = dataType;
	
	in.open(iname.c_str(), ios::binary | ios::in );

	if( !in )
	{
		cerr << "Cannot open img file.\n";
		return 1;
	}

	if( strstr( type,"byte")!=NULL)
	{
		for(dy=0;dy<DimY; dy++)
			for(dx=0;dx<DimX; dx++)
			{
				in.read(&echa,1);
				spe = echa;
				if( dt== "SP" )
					C(dx,dy).Elem(A) = spe;
				if( dt== "HA" )
					C(dx,dy).Habitat = spe;
				if( dt== "AG" )
					C(dx,dy).Age = spe;
				
			}
	}
	else if( strstr( type,"integer")!=NULL )
	{
		for(dy=0;dy<DimY; dy++)
			for(dx=0;dx<DimX; dx++)
			{
				in.read((char*)&eint,2);
				spe = eint;
				if( dt== "SP" )
					C(dx,dy).Elem(A) = spe;
				if( dt== "HA" )
					C(dx,dy).Habitat = spe;
				if( dt== "AG" )
					C(dx,dy).Age = spe;
			}
	}

	return 0;
}

int CAInva::SaveSeed(const char * fname,
			int xTopLeft, int yTopLeft, int xBottomRight, int yBottomRight, int option)
{
	int i,j;
	ofstream sav( fname );
	if(!sav)
	{
		cerr << "Cannot open Seed file.\n";
		return 0;
	}

	sav << DimX << "\t" << DimY << endl;
	
	// Save species
	//
	sav << "SP " << "Window " << xTopLeft << "\t" << yTopLeft << "\t"
		<< xBottomRight << "\t" << yBottomRight << endl;
	if( (xTopLeft < 0 || xTopLeft > DimX ) ||
		( yTopLeft < 0 || yTopLeft > DimX ) ||
		( xBottomRight < 0 || xBottomRight > DimX ) ||
		( yBottomRight < 0 || yBottomRight > DimX ) ||
		( xTopLeft>xBottomRight || yTopLeft>yBottomRight ) )
		return(0);											// Error de Limites
	
	for(i=yTopLeft; i<yBottomRight; i++)
	{
		for(j=xTopLeft; j<xBottomRight; j++)
			sav<< setw(2) << (int)C(j,i).Elem(N);
		sav << endl;
	}
	sav << endl;

	// Save age strutcture
	//
	sav << "AG" << endl;
	for(i=yTopLeft; i<yBottomRight; i++)
	{
		for(j=xTopLeft; j<xBottomRight; j++)
			sav<< setw(4) << (int)C(j,i).Age;
		sav << endl;
	}
	sav << endl;

	// Save Habitat strutcture
	//
	if( option == 1 )
	{
		sav << "HA" << endl;
		for(i=yTopLeft; i<yBottomRight; i++)
		{
			for(j=xTopLeft; j<xBottomRight; j++)
				sav<< setw(4) << (int)C(j,i).Habitat;
			sav << endl;
		}
		sav << endl;
	}
	
	// Salva posicion de Adultos y Juveniles
	// Adultos de la 1ra especie definida en el archivo de parametros 1
	// Juveniles                                                      2
	// Adultos de la Nth especie  (n-1)*2+1
	// Juveniles                  (n-1)*2+2
	//
	sav << "AJ" << endl;
	for(i=yTopLeft; i<yBottomRight; i++)
	{
		for(j=xTopLeft; j<xBottomRight; j++)
		{
			int sp = C(j,i).Elem(N);
			if(sp>0)
				if( C(j,i).Adult )
					sav << setw(4) << (sp-1)*2+1;
				else
					sav << setw(4) << (sp-1)*2+2;
			else
				sav << setw(4) << "0";
		}
		sav << endl;
	}
	sav << endl;

	return 1;
}
	
int CAInva::SaveSeed(const char * fname, int option)
{
	int i,j;
	ofstream sav( fname );
	if(!sav)
	{
		cerr << "Cannot open Seed file.\n";
		return 1;
	}

	sav << DimX << "\t" << DimY << endl;
	
	// Save species
	//
	sav << "SP" << endl;
	for(i=0; i<DimY; i++)
	{
		for(j=0;j<DimX;j++)
			sav<< setw(2) << (int)C(j,i).Elem(N);
		sav << endl;
	}
	sav << endl;

	// Save age strutcture
	//
	sav << "AG" << endl;
	for(i=0; i<DimY; i++)
	{
		for(j=0;j<DimX;j++)
			sav<< setw(4) << (int)C(j,i).Age;
		sav << endl;
	}
	sav << endl;

	// Save Habitat strutcture
	//
	if( option == 1 )
	{
		sav << "HA" << endl;
		for(i=0; i<DimY; i++)
		{
			for(j=0;j<DimX;j++)
				sav<< setw(4) << (int)C(j,i).Habitat;
			sav << endl;
		}
		sav << endl;
	}
	
	// Salva posicion de Adultos y Juveniles
	// Adultos de la 1ra especie definida en el archivo de parametros 1
	// Juveniles                                                      2
	// Adultos de la Nth especie  (n-1)*2+1
	// Juveniles                  (n-1)*2+2
	//
	sav << "AJ" << endl;
	for(i=0; i<DimY; i++)
	{
		for(j=0;j<DimX;j++)
		{
			int sp = C(j,i).Elem(N);
			if(sp>0)
				if( C(j,i).Adult )
					sav << setw(4) << (sp-1)*2+1;
				else
					sav << setw(4) << (sp-1)*2+2;
			else
				sav << setw(4) << "0";
		}
		sav << endl;
	}
	sav << endl;

	return 0;
}


int CAInva::PrintParams(ostream & fb)
{
	int l;

	for(l=0;l<NumSpecies;l++)
		fb << Sp[l];
 	return 1;
}


int CAInva::PrintParamsPomac(ostream & fb,bool titles)
{
	int l;

	for(l=0;l<NumSpecies;l++)
	{
		if(titles)
		{
			fb << "MaximunSurvivalDistance"  << "\t";
			fb << "AdultAge"  << "\t";
			fb << "PJuvenilMortality"  << "\t";
			fb << "PAdultMortality"  << "\t";
			fb << "MeanDispersalDistance"  << "\t";
//			fb << "ParmDispersal"  << "\t";
			fb << "NRecruits"  << "\t";
//			fb << "DispersalNorm"  << "\t";
			fb << "NeighSurvivalDistance"  << "\t";
			fb << "NeighSurvivalSlopeAd"  << "\t";
			fb << "NeighSurvivalSlopeJu"  << "\t";
			fb << "ShadeTolerance"  << "\t";
			fb << "PColonization"  << "\t";
		}
		else
		{
			if(Sp[l].AdultAge<0 || Sp[l].PJuvenilMortality<0 || Sp[l].PAdultMortality<0 ||
				Sp[l].MeanDispersalDistance<0 || Sp[l].NRecruits<0 || Sp[l].NeighSurvivalDistance<0 ||
				Sp[l].NeighSurvivalSlopeAd<0)
			{
				cerr << "Invalid parameter writing pomac file!!!!\b\b\b\b\b\b\b" << endl;
				exit(1);
			}

			fb << Sp[l].MaximunSurvivalDistance << "\t";
			fb << Sp[l].AdultAge << "\t";
			fb << Sp[l].PJuvenilMortality << "\t";
			fb << Sp[l].PAdultMortality << "\t";
			fb << Sp[l].MeanDispersalDistance << "\t";
//			fb << Sp[l].ParmDispersal << "\t";
			fb << Sp[l].NRecruits << "\t";
//			fb << Sp[l].DispersalNorm << "\t";
			fb << Sp[l].NeighSurvivalDistance << "\t";
			fb << Sp[l].NeighSurvivalSlopeAd << "\t";
			fb << Sp[l].NeighSurvivalSlopeJu << "\t";
			fb << Sp[l].ShadeTolerance << "\t";
			fb << Sp[l].PColonization << "\t";
			
		}
	}

 	return 1;
}


int CAInva::ReadPomacParms(string const & fpom)
{
	string buff;
	static bool privez=true;
	if(privez)
	{
		InLineFile.open(fpom.c_str());
		if( !InLineFile )
		{
			cerr << "Cannot open Parms file" << endl;
			return 0;
		}
		privez=false;
		getline(InLineFile,buff);
	}

    if( InLineFile.eof() )
        return 0;
	getline(InLineFile,buff);
	istringstream ins(buff.c_str());
	int l;                    
	for(l=0;l<NumSpecies;l++)
	{
        char kk[20];

		ins >> Sp[l].MaximunSurvivalDistance;
		ins >> Sp[l].AdultAge;
		ins >> kk;
		ins >> Sp[l].PJuvenilMortality;
		ins >> Sp[l].PAdultMortality;
		ins >> Sp[l].MeanDispersalDistance;
		ins >> Sp[l].NRecruits;
		ins >> kk;
		ins >> Sp[l].NeighSurvivalDistance;
		ins >> kk;
		ins >> Sp[l].NeighSurvivalSlopeAd;
		ins >> Sp[l].NeighSurvivalSlopeJu;
		ins >> Sp[l].ShadeTolerance;
		ins >> Sp[l].PColonization;		

		if(Sp[l].MaximunSurvivalDistance<0 || Sp[l].AdultAge<0 || Sp[l].PJuvenilMortality<0 || Sp[l].PAdultMortality<0 ||
			Sp[l].MeanDispersalDistance<0 || Sp[l].NRecruits<0 || Sp[l].NeighSurvivalDistance<0 ||
			Sp[l].NeighSurvivalSlopeAd<0 || Sp[l].ShadeTolerance<0 || Sp[l].PColonization<0 )
		{
			cerr << "Invalid parameter reading pomac file!!!!\b\b\b\b\b\b\b" << endl;
			exit(1);
		}

	}
	InitParms();
    return 1;
}

int CAInva::ReadLineParms(string & lineParms)
{
	int spno=0;
	istringstream ins(lineParms.c_str());
	ins >> NumLineParms;
	
	for(int i=0;i<NumSpecies;i++)
	{
		Sp[i].Scan(ins,spno);
		if( spno>=NumSpecies || spno!=i )
		{
			cerr << "Error reading Line parameters" << endl;
			return 0;
		}
	}
	return 1;
}

int CAInva::ReadFileLineParms( char * file)
{
	string buff;
	static bool privez=true;
	if(privez)
	{
		InLineFile.open(file);
		if( !InLineFile )
		{
			cerr << "Cannot open Parms file" << endl;
			return 0;
		}
		privez=false;
	}

	getline(InLineFile,buff);
    if( InLineFile.eof() )
        return 0;
                    
	if(!ReadLineParms(buff))
	{
        cerr << "Error reading Line parms" << endl;
        return 0;
	}
	InitParms();
	return 1;
}

int CAInva::ReadParms( char * file)
{
	ifstream in;
	char buff[255];

	int sp=0;

	in.open(file);
	if( !in )
	{
		cerr << "Cannot open Parms file.\n";
		return 0;
	}

	for(int i=0;i<NumSpecies;i++)
	{
		in.getline(buff,254);
		sp = Sp[i].Scan(buff);
		if( sp>=NumSpecies || sp!=i )
		{
			cerr << "Error reading Parameter file:" << file << endl;
			return 0;
		}
	}
	InitParms();
	return 1;
}


int CAInva::InitParms()
{
	int na;
	for( na=1; na<NumSpecies; na++)
		Sp[na].PColonization += Sp[na-1].PColonization;
		
	if( Sp[na-1].PColonization > 1 )
	{
		for( na=0; na<NumSpecies; na++)
			Sp[na].PColonization /= Sp[NumSpecies-1].PColonization;
	}
	return 1;
}


