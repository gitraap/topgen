/*
 *  MINNode.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_NODE_HPP
#define TOPGEN_NODE_HPP

#include <ostream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <topgen/topology.hpp>
#include <topgen/exception.hpp>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <topgen/scp_manager.hpp>

#define TOPGEN_LEFT  0      //FIXME: Remove
#define TOPGEN_RIGHT 1      //FIXME: Remove

namespace topgen {

class CMINNode;

struct nodeStruct
{
	int port;
	unsigned long decimal;
	boost::dynamic_bitset<> binary;
	CMINNode * remote_node;
	struct nodeStruct * remote_port;
	int net_port;
};

struct internalPortStruct
{
	int switch_id;
	int switch_port;
};

typedef struct nodeStruct minNodeStructType;

class CMINNode
{
public:
	CMINNode(int _stage, int _switch, int identifier, int total_ports, int side_ports, int bits, bool isLast, int internal_ports, CSCPManager * scp);
	virtual ~CMINNode();

	void SetNodeType(nodeType type);
	void SetNetPort(int port, int net_port);

	int GetID();
	int GetPorts();
	int GetIdentifier();
	char * GetName();
	int GetSwitches();
	int GetInternalPorts();

	void Init(int port, unsigned long decimal);
	void InitTable(int switches, int switch_ports, matchingType matching);
	minNodeStructType * GetPortByPort(int port);
	minNodeStructType * GetLeftPortByDecimal(unsigned long decimal);
	minNodeStructType * GetRightPortByDecimal(unsigned long decimal);

	int SearchInterval(int at_port, bool first);

	int GetSwitchID(int port);
	int GetSwitchPort(int port);

	void Print(FILE * file, const char * type);
        static void ResetCounter();

private:
	int m_iStage;
	int m_iSwitch;
	int m_iIdentifier;
	int m_iPorts;
	int m_iSidePorts;
	int m_iBits;
	int m_bIsLastStage;
	int m_iInternalPorts;
	CSCPManager * m_pSCPManager;

	char * m_sName;
	int m_iID;
	nodeType m_iNodeType;

	minNodeStructType ** m_pPorts;

	// Embedded Switches
	int m_iSwitches;
	struct internalPortStruct ** m_pPortTable; // Table: [L/R] x [side_port] -> switch_id x switch_port

protected:
	static int g_iCounter;
};

} // namespace topology

#endif /* TOPGEN_NODE_HPP */
