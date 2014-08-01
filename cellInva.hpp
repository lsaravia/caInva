#ifndef __CellInva_HPP
#define __CellInva_HPP

//#ifdef __GNUC__
//#include "fortify.h"
//#endif

#define uchar unsigned char

class CellInva
	{
	public:

	uchar NumSpecie[2];
	int Age;
	uchar Habitat;
	bool Adult;
	
	CellInva() { NumSpecie[0]=0;  NumSpecie[1]=0; Age=0; Habitat=0; Adult=0;};
	uchar & Elem( int idx ) { return NumSpecie[idx]; };

	const CellInva & Elem( int nsp0, int nsp1, int age=0, int adult=0) {
								NumSpecie[0]=nsp0;NumSpecie[1]=nsp1;
								Age=age;
								Adult=adult;
								return *this; };
	
	CellInva & operator=(const CellInva &src){ NumSpecie[0] = src.NumSpecie[0];
								NumSpecie[1] = src.NumSpecie[1];
								Age= src.Age;
								Habitat = src.Habitat;
								Adult = src.Adult;
								return *this;};

	CellInva(const CellInva &src){ NumSpecie[0] = src.NumSpecie[0];
								NumSpecie[1] = src.NumSpecie[1];
								Age= src.Age;
								Habitat = src.Habitat;
								Adult = src.Adult;
								};

	void Transfer(int nsp0, int nsp1) {NumSpecie[nsp0]=NumSpecie[nsp1];};
	
	};


#endif //__CellInva_HPP
