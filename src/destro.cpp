/*
 *  Destro.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <stdio.h>
#include "topgen/destro.hpp"

namespace topgen {

CDestro::CDestro(int terminals, int radix)
{
	N = terminals;
	this->radix = radix;
	k = radix/2;
	n = myLog(N,k);
	kbits = myLog(k,2);
}

CDestro::~CDestro()
{

}

void CDestro::PrintDestinations(bool ** destinations)
{
	for (int p=0; p<radix; p++) {
		printf("[%3d]-> ", p);
		bool first = true;
		for (int n=0; n<N; n++) {
			if (destinations[p][n]) {
				if (first)
					printf("%3d", n);
				else
					printf(",%3d", n);
				first = false;
			}
		}
		printf("\n");
	}

}

std::list<int> * CDestro::Nbtl(int stage, int node, int port)
{
	// Nbtl
	CMyBinary * bin_o = new CMyBinary(node, n, k);
	std::list<int> * tmp = new std::list<int>();

	if (port<k)
	{
		for (int terminal=0; terminal<N; terminal++)
		{
			CMyBinary * h = new CMyBinary(terminal, n, k);
			bool condition1 = false;
			bool condition2 = true;

			if (h->getKNumber(stage) == port)
				condition1 = true;

			for (int i=stage+1; i<n; i++)
				if (h->getKNumber(i) != bin_o->getKNumber(i-1))
					condition2 = false;

			if (condition1 && condition2) {
				int dec_h = h->MyBinaryToInt();
				tmp->push_back(dec_h);
			}

			delete h;
		}
	}

	delete bin_o;
	return tmp;
}

std::list<int> * CDestro::Nftl(int stage, int node, int port)
{
	// Nftl
	CMyBinary * bin_o = new CMyBinary(node, n, k);
	std::list<int> * tmp = new std::list<int>();

	if (radix/2 <= port && port<radix)
	{
		for (int terminal=0; terminal<N; terminal++)
		{
			CMyBinary * h = new CMyBinary(terminal, n, k);
			bool condition1 = false;

			for (int i=stage+1; i<n; i++)
				if (h->getKNumber(i) != bin_o->getKNumber(i-1))
					condition1 = true;

			if (condition1)
			{
				int dec_h = h->MyBinaryToInt();
				tmp->push_back(dec_h);
			}

			delete h;
		}
	}

	delete bin_o;
	return tmp;
}

std::list<int> * CDestro::NbRl(int stage, int node, int port)
{
	// NbRl
	CMyBinary * bin_o = new CMyBinary(node, n, k);
	std::list<int> * tmp = new std::list<int>();

	CMyBinary * h = new CMyBinary(node, n, k);

	for (int i=stage+1; i<n; i++) {
		h->putKnumber(i, bin_o->getKNumber(i-1));
	}

	h->putKnumber(stage, port);

	for (int i=0; i<stage; i++) {
		h->putKnumber(i, bin_o->getKNumber(i));
	}

	int dec_h = h->MyBinaryToInt();
	tmp->push_back(dec_h);

	delete h;
	delete bin_o;
	return tmp;
}

std::list<int> * CDestro::NfRl(int stage, int node, int port)
{
	// NfRl
	CMyBinary * bin_o = new CMyBinary(node, n, k);
	std::list<int> * tmp = new std::list<int>();

	if (stage<n-1)
	{
		for (int terminal=0; terminal<N; terminal++)
		{
			CMyBinary * h = new CMyBinary(terminal, n, k);
			bool condition1 = false;
			bool condition2 = false;
			bool condition3 = true;

			for (int i=stage+1; i<n; i++)
				if (h->getKNumber(i) != bin_o->getKNumber(i-1))
					condition1 = true;

			if (h->getKNumber(stage) == port-k)
				condition2 = true;

			for (int i=0; i<stage; i++)
				if (h->getKNumber(i) != bin_o->getKNumber(i))
					condition1 = false;

			if (condition1 && condition2 && condition3) {
				int dec_h = h->MyBinaryToInt();
				tmp->push_back(dec_h);
			}

			delete h;
		}
	}

	delete bin_o;
	return tmp;
}

int CDestro::myLog(int a, int base)
{
	int times = 0;
	double acumulator = 1.0;

	while (acumulator < a)
	{
		acumulator *= base;
		times++;
	}

	return times;
}

}
