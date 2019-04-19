/*
 *  DIRNode.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_DIR_NODE_HPP
#define TOPGEN_DIR_NODE_HPP

#include <stdlib.h>
#include <stdio.h>
#include <topgen/topology.hpp>
#include <topgen/ia.hpp>

#define TOPGEN_TOP    0     //FIXME: Remove
#define TOPGEN_BOTTOM 1     //FIXME: Remove

namespace topgen {

class CDIRNode
{
public:
	CDIRNode(const int dimensions, const int ports, const int identifier, const nodeType node_type);
	virtual ~CDIRNode();

	void SetupInternalConfiguration();
	int GetSwitchPort(const int port);
	int GetSwitchID(const int port);

	char * GetName();
	int GetID();
	int GetIdentifier();
	int GetPorts();
	int GetDimensions();

	void SetNetPort(const int number);
	int GetNetPort();
	void SetNetPort2(const int number);
	int GetNetPort2();

	void SetNicPort(const int port);
	int GetNicPort();

	void SetPosition(const int dimension, const int position);
	int GetPosition(const int dimension);
        int * GetPositionVector();

	void SetNext(const int dimension, CDIRNode * next);
	CDIRNode * GetNext(const int dimension);

	void SetPrev(const int dimension, CDIRNode * prev);
	CDIRNode * GetPrev(const int dimension);

	int GetNextLink(const int dimension);
	int GetPrevLink(const int dimension);
        

	void Print(FILE * file, const char * type);
        static void ResetCounter();

private:
	int m_iDimensions;
	int m_iPorts;
	int m_iIdentifier;

	char * m_sName;

	int * m_pPosition;
	int m_iNetPort;
	int m_iNetPort2;
	int m_iID;
	CDIRNode ** m_pNeighbours;
	int * m_pNextLinks;
	int * m_pPrevLinks;
	int m_iAssignedPort;
	int m_iNicPort;
	nodeType m_iNodeType;

	int m_iIAs;
	CIA ** m_pIA;

	int * m_pInternalSwitchID;   // [output port] -> Internal Switch ID (i.e. TOP / BOTTOM)
	int * m_pInternalSwitchPort; // [output port] -> Internal Switch Port (i.e. 0,1,2,3) NIC=4 InterCard=5

protected:
	static int g_iCounter;
};

} // namespace topology

#endif /* TOPGEN_DIR_NODE_HPP */
