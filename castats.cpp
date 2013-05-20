//
//
//
//#pragma implementation

#include "caInva.hpp"
#include "fortify.h"

void CAInva::WindowStats( int xTopLeft, int yTopLeft, int xBottomRight, int yBottomRight,
						DensVar & r,int specie)
	{
	int x,y,e,tot,a;

	for( y=yBottomRight; y<yTopLeft; y++)
		for( x=xBottomRight; x<xTopLeft; x++)
			{
			e=(C(x,y).Elem(N));
			if( e == specie )
				{
				e--;        // specie debe ser > 0

				a=C(x,y).Age;


				if( a>=Sp[e].AdultAge )
					{
					r.AdultCant++;
					}
				else
					{
					r.JuvenilCant++;
					}
				}
			}
	tot = (xTopLeft-xBottomRight) * (yTopLeft-yBottomRight);

	r.AdultDensity   = r.AdultCant/tot;
	r.JuvenilDensity = r.JuvenilCant/tot;
	r.TotalCant      = r.JuvenilCant+r.AdultCant;
	r.TotalDensity   = r.TotalCant / tot;

	r.AdultVar = (r.AdultDensity-1)*(r.AdultDensity-1) * r.AdultCant + (r.AdultDensity) * (r.AdultDensity) * (tot - r.AdultCant);
	r.JuvenilVar = (r.JuvenilDensity-1)*(r.JuvenilDensity-1) * r.JuvenilCant + (r.JuvenilDensity) * (r.JuvenilDensity) * (tot - r.JuvenilCant);
	r.TotalVar   = (r.TotalDensity-1)*(r.TotalDensity-1) * r.TotalCant + (r.TotalDensity) * (r.TotalDensity) * (tot - r.TotalCant);
	}
