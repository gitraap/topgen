/*
 *  MiBinary.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "topgen/topology.hpp"
#include "topgen/my_binary.hpp"

namespace topgen {

CMyBinary::CMyBinary(int decimal, int k_numbers, int kary)
{
	m_iDecimal = decimal;
	m_iKNumbers = k_numbers;
	m_pKNumbers = new int[m_iKNumbers];
	m_iKary = kary;

    int aux = decimal;
    for(int i = 0; i<m_iKNumbers; i++)
    {
        if(aux > 0)
        {
            m_pKNumbers[i] = aux % m_iKary;
            aux = aux/m_iKary;
        }
	    else
	    	m_pKNumbers[i] = 0;
    }

}

CMyBinary::~CMyBinary()
{
	delete [] m_pKNumbers;
}

int CMyBinary::MyBinaryToInt()
{
	int dec = 0;
	for(int i=0; i<m_iKNumbers; i++)
	{
		dec += m_pKNumbers[i] * pow(m_iKary, i);
	}
	m_iDecimal = dec;

	return m_iDecimal;
}

int CMyBinary::Xor(CMyBinary * otro)
{
	int pos = m_iKNumbers - 1;
	while (m_pKNumbers[pos] == otro->m_pKNumbers[pos] && pos > 0) {
		pos--;
	}

	return pos;
}

int CMyBinary::getKNumber(int pos)
{
	return m_pKNumbers[pos];
}

void CMyBinary::putKnumber(int pos, int value)
{
	m_pKNumbers[pos] = value;
}

char *CMyBinary::toString()
{
	char *sr = new char[TOPGEN_MAX_STRING_LENGTH];
	char *tmp = new char[TOPGEN_MAX_STRING_LENGTH];
	sr[0]='\0';

	for (int i=m_iKNumbers-1; i>=0; i--) {
		if (i != (m_iKNumbers - 1))
			snprintf(tmp, TOPGEN_MAX_STRING_LENGTH, ",%d", m_pKNumbers[i]);
		else
			snprintf(tmp, TOPGEN_MAX_STRING_LENGTH, "%d", m_pKNumbers[i]);

                strncat(sr, tmp, TOPGEN_MAX_STRING_LENGTH - strlen(sr));
	}
        delete[] tmp;
	return sr;
}

}
