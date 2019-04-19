/*
 *  DFlyNode.h
port *
 *  Created by Juan Villar and Jesus Escudero-Sahuquillo on 01/11/2015.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef DFLYNODE_H_
#define DFLYNODE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <topgen/topology.hpp>
#include <topgen/exception.hpp>
#include <math.h>
#include <stdint.h>

namespace topgen {

class CDFlyNode;

struct dflynodeStruct
{
  int port;
  //unsigned long decimal;
  int decimal;
  CDFlyNode * remote_node;
  struct dflynodeStruct * remote_port;
  int net_port;
};

typedef struct dflynodeStruct dflyNodeStruct_t;

class CDFlyGroup;

class CDFlyNode
{
public:
        CDFlyNode(int identifier, int a, int h, int p);
	virtual ~CDFlyNode();

	int GetID();
	int GetPorts();
	int GetIdentifier();
	CDFlyGroup * GetGroup();
	char * GetName();

	void SetNetPort(int port, int net_port);
        int GetNetPort(int port);
	void SetGroup(CDFlyGroup * group);
	void Init();

	dflyNodeStruct_t * GetPortByPort(int port);

	void Print(FILE * file, const char * type);
	void Dump();
        static void ResetCounter();
        static const size_t MAX_STRING_LENGTH = 16;

private:
	int m_iIdentifier;
	int m_iPorts;
	int m_iGlobalLinks;
	int m_iLocalLinks;
	int m_iNetPortLinks;

	char * m_sName;
	int m_iID;

	dflyNodeStruct_t ** m_pNodePortsArray;
	CDFlyGroup * m_pGroup;

protected:
	static int g_iCounter;
};

} // namespace topology

#endif /* DFLYNODE_H_ */
