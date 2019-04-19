/*
 *  PSDestro.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_PSDESTRO_HPP
#define TOPGEN_PSDESTRO_HPP

#include <list>
#include <topgen/my_binary.hpp>

namespace topgen {

class CMIN;

class CPSDestro {
public:
	CPSDestro(int terminals, int radix, bool inportbased, CMIN * min);
	virtual ~CPSDestro();

	void initialize();
	void PrintDestinations(bool ** destinations);
	std::list<int> * getDstList(int stage, int node, int port);

	int myLog(int a, int base);

private:
	int N;
	int radix;
	int k;
	int n;
	int kbits;
	bool verbose;
	int m_iNumSwitches;
	int m_iNumIports;
	int m_iNumDigits;
	int *** m_pOports;
	CMIN * m_pMIN;

	unsigned int _computeHighestStagePS(int origen, int destino);
	void _num2Digit(int linkId, int * linkDigits, int base, int nDigits);
	void _digit2Num (int ** linkDigits, int * linkId, int base, int nDigits);
	void _leftShift (int * linkDigits, int numShifts, int nDigits, int * dst);
};

}

#endif /* TOPGEN_PSDESTRO_HPP */
