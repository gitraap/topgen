/*
 *  MyBinary.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */


#ifndef TOPGEN_MY_BINARY_HPP
#define TOPGEN_MY_BINARY_HPP

namespace topgen {

class CMyBinary {
public:
	CMyBinary(int decimal, int k_numbers, int kary);
	virtual ~CMyBinary();

	int MyBinaryToInt();
	int Xor(CMyBinary * otro);
	int getKNumber(int pos);
	void putKnumber(int pos, int value);
	char * toString();

public:
	int m_iKNumbers;
	int * m_pKNumbers;
	int m_iDecimal;
	int m_iKary;
};

}

#endif /* TOPGEN_MY_BINARY_HPP */
