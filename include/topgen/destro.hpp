/*
 *  Destro.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */


#ifndef TOPGEN_DESTRO_HPP
#define TOPGEN_DESTRO_HPP

#include <list>
#include <topgen/my_binary.hpp>

namespace topgen {

class CDestro {
public:
	CDestro(int terminals, int radix);
	virtual ~CDestro();

	void PrintDestinations(bool ** destinations);
	std::list<int> * Nbtl(int stage, int node, int port);
	std::list<int> * Nftl(int stage, int node, int port);
	std::list<int> * NbRl(int stage, int node, int port);
	std::list<int> * NfRl(int stage, int node, int port);

	int myLog(int a, int base);

private:
	int N;
	int radix;
	int k;
	int n;
	int kbits;
	bool verbose;
};

}

#endif /* TOPGEN_DESTRO_HPP */
