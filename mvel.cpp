// Velocidad a partir de archivo OUT
// caVel arch.inp sp arch.out arch.res
#include "caInva.hpp"

int main(int argc, char * argv[])
	{
	int nEval=0,nRuns=0;
	CAInva ca;

	if( argc > 3 )
		{
		ca.ReadParms( nEval,nRuns,argv[1],0 );
		ca.VelRegress( atoi(argv[2]),argv[3],argv[4],argv[1]);

		}
	else
		cerr << "USO: caregr parms.inp spNum file.out salida" << endl;
	}
