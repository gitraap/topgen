/*
 *  PSDestro.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <stdio.h>
#include <limits.h>
#include <math.h>
#include "topgen/psdestro.hpp"
#include "topgen/min.hpp"
#include "topgen/exception.hpp"

namespace topgen {

CPSDestro::CPSDestro(int terminals, int radix, bool inportbased, CMIN * min)
{
	m_pMIN = min;
	N = terminals;
	this->radix = radix;
	k = radix/2;
	n = myLog(N,k);
	kbits = myLog(k,2);

	m_iNumSwitches = (N/k)*n;
	m_iNumDigits = myLog(N,2);

	if(inportbased)
		m_iNumIports = k;
	else
		m_iNumIports = 1;

	m_pOports = new int ** [m_iNumSwitches];
	for(int i = 0; i < m_iNumSwitches; i++){
		m_pOports[i] = new int * [N];
		for(int j = 0; j < N; j++){
			m_pOports[i][j] = new int [m_iNumIports];
			for(int k = 0; k < m_iNumIports; k++)
				m_pOports[i][j][k] = INT_MAX;
		}
	}
}

CPSDestro::~CPSDestro()
{
	for(int i = 0; i < m_iNumSwitches; i++){
		for(int j = 0; j < N; j++)
			delete [] m_pOports[i][j];
		delete [] m_pOports[i];
	}
	delete [] m_pOports;
}

void CPSDestro::initialize(){
	/* This method fills the m_pOports structures based on the routing algorithm */

	int contadorSrc = INT_MAX;
	int contadorDst = INT_MAX;
	int highestStage = INT_MAX;
	int hops = 0;
	int stage = 0;
	int inLink = INT_MAX;
	int currentSw = INT_MAX;
	int outLink = INT_MAX;
	int linkIdNext;
	int * linkDigits = NULL, * linkDigitsNext = NULL;
	int nextSw, nextLink;

	for(contadorSrc = 0; contadorSrc < N; contadorSrc++){
		for(contadorDst = 0; contadorDst < N; contadorDst++){
			if(contadorSrc!=contadorDst){
				// Calculate the turn stage for the src, dst pair
				highestStage = _computeHighestStagePS(contadorSrc,contadorDst);
				hops = (2 * highestStage) + 1;

				stage = 0;
				inLink = (contadorSrc % k);
                                currentSw = contadorSrc / k;

				while(hops > 0){
					if(m_iNumIports > 1){
						// PSDestro for MFTs
						if((stage < highestStage) && (inLink < k)){
							// upwards
							outLink = ((contadorDst + inLink) % k) + k;
							++stage;
						}
						else{
							// downwards
							_num2Digit (contadorDst, linkDigits, 2, m_iNumDigits);
							_leftShift (linkDigits, stage * kbits, m_iNumDigits, linkDigitsNext);
							_digit2Num (&linkDigitsNext, &linkIdNext, 2, m_iNumDigits);

							outLink = linkIdNext % k;
							--stage;
						}

						if(m_pOports[currentSw][contadorDst][inLink] != INT_MAX
								&& m_pOports[currentSw][contadorDst][inLink] != outLink){
							printf("[Conmutador:%d, entrada:%d, destino:%d]. Outlink múltiple: actual=%d, nuevo=%d\n", currentSw, inLink, contadorDst, m_pOports[currentSw][contadorDst][inLink], outLink);
							throw new CException(" ERROR: Perfect-shuffle - Multiple output ports in the same table entry");
						}
						m_pOports[currentSw][contadorDst][inLink] = outLink;

					}
					else{
						// PSDestro for LFTs
						if((stage < highestStage) && (inLink < k)){
							/* Upward */
							outLink = ((contadorDst/(int)pow(k, ((n - stage) % n))) % k) + k; // Self-routing
							++stage;
						}
						else{
							/* Downward */
							outLink = (contadorDst/(int)pow(k, ((n - stage) % n))) % k; // Self-routing
							--stage;
						}

						if(m_pOports[currentSw][contadorDst][0] != INT_MAX
								&& m_pOports[currentSw][contadorDst][0] != outLink){
							printf("[Conmutador:%d, entrada:%d, destino:%d]. Multiple oport. current entry:%d, new entry:%d\n", currentSw, inLink, contadorDst, m_pOports[currentSw][contadorDst][0], outLink);
							//throw new CException(" ERROR: Perfect-shuffle - Multiple output ports in the same table entry");
							printf(" ERROR: Perfect-shuffle - Multiple output ports in the same table entry");
							exit(0);
						}
						m_pOports[currentSw][contadorDst][0] = outLink;
					}

                        // TODO: Routing pairs

            		if(m_pMIN->IsElementToElement(currentSw,outLink,(unsigned int *)&nextSw,(unsigned int *)&nextLink)){
            			inLink = nextLink;
            			currentSw = nextSw;
            		}
            		else{
            			if(!m_pMIN->IsElementToIA(currentSw,outLink,(unsigned int *)&nextSw,(unsigned int *)&nextLink)){
            				printf("Src:%d, Dst:%d, IA:%u\n",contadorSrc,contadorDst,nextSw);
            				printf("ERROR: Routing fails (IsElementToIA).");
            				exit(0);
            			}

            			if(nextSw != contadorDst){
            				printf("Src:%d, Dst:%d, currentSw:%d, currentLk:%d, IA:%u\n",contadorSrc,contadorDst,currentSw,outLink,nextSw);
            				printf("ERROR: Routing fails (IA incorrect).");
            				exit(0);

            			}
            		}
				    --hops;

				}
			}
		}
	}

	printf("PSDestro table:\n");
	for(int i = 0; i < m_iNumSwitches; i++){
		printf("Sw:%d -> [",i);
		for(int j = 0; j < N; j++){
			if(m_pOports[i][j][0] != INT_MAX)
				printf("Dst:%d - OP:%d, ",j,m_pOports[i][j][0]);
		}
		printf("]\n");
	}
}

unsigned int CPSDestro::_computeHighestStagePS(int origen, int destino){
	int stage, bits, bitPos;
	int highestStage;
	int *linkDigitsSrc;
	int *linkDigitsDst;

	linkDigitsSrc = new int[m_iNumDigits];
	linkDigitsDst = new int[m_iNumDigits];

	highestStage = 0;

	_num2Digit(origen,linkDigitsSrc,2,m_iNumDigits);
	_num2Digit(destino,linkDigitsDst,2,m_iNumDigits);

	for(stage = 1; stage < n; stage++){
		for(bits = 0; bits < kbits; bits++){
			bitPos = m_iNumDigits - ((stage -1) * kbits) - bits - 1;
			if(((stage * kbits) + bits) < m_iNumDigits){
				if(linkDigitsSrc[bitPos] != linkDigitsDst[bitPos]){
					highestStage = stage;
				}
			}
		}
	}

	delete [] linkDigitsSrc;
	delete [] linkDigitsDst;

	return highestStage;
}

void CPSDestro::_num2Digit(int linkId, int * linkDigits, int base, int nDigits){

	for(int i= 0; i < nDigits; i++){
		*(linkDigits+i) = linkId % base;
		linkId /= base;
	}
}

void CPSDestro::_digit2Num (int ** linkDigits, int * linkId, int base, int nDigits){
	int i;
    int factor;

    *linkId = 0;
    factor = 1;
    for(i = 0; i < nDigits;i++){
    	*linkId = *linkId + (factor * ((*linkDigits)[i]));
    	factor = factor * base;
    }
}

void CPSDestro::_leftShift (int * linkDigits, int numShifts, int nDigits, int * dst){

	for(int i = nDigits - 1; i >= numShifts; i--){
		*(dst+i) = *(linkDigits+(i - numShifts));
	}

	if(numShifts != 0){
		for(int i = numShifts-1; i>=0; i--)
			*(dst + i) = *(linkDigits + (nDigits - numShifts + i));
	}
}

void CPSDestro::PrintDestinations(bool ** destinations)
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

std::list<int> * CPSDestro::getDstList(int stage, int node, int port)
{
	std::list<int> * tmp = new std::list<int>();
	unsigned int swid = (stage * (m_iNumSwitches/n)) + node; // TODO Ask id to the topology library

	for (int terminal = 0; terminal < N; terminal++){
		if(m_pOports[swid][terminal][0] != INT_MAX){
			if(m_pOports[swid][terminal][0] == port){
				tmp->push_back(terminal);
			}
		}
	}

	return tmp;
}

int CPSDestro::myLog(int a, int base)
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
