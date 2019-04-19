/*
 *  MINNode.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <ostream>
#include <iostream>
#include "topgen/min_node.hpp"

namespace topgen {

CMINNode::CMINNode(int _stage, int _switch, int identifier, int total_ports, int side_ports, int bits, bool is_last, int internal_ports, CSCPManager * scp)
{
	m_iStage = _stage;
	m_iSwitch = _switch;
	m_iIdentifier = identifier;
	m_iPorts = total_ports;
	m_iSidePorts = side_ports;
	m_iBits = bits;
	m_bIsLastStage = is_last;
	m_iInternalPorts = internal_ports;
	m_pSCPManager = scp;

	m_iSwitches = 1;
	m_sName = new char[TOPGEN_MAX_STRING_LENGTH];
	sprintf(m_sName, "s%d", m_iIdentifier);
	m_iID=g_iCounter++;
	m_iNodeType = switchSTD;

	m_pPorts = new minNodeStructType*[m_iPorts];

	m_pPortTable = new struct internalPortStruct*[m_iPorts];
	for(int p=0; p<m_iPorts; p++)
	{
		m_pPortTable[p] = new struct internalPortStruct;
		m_pPortTable[p]->switch_id = -1;
		m_pPortTable[p]->switch_port = -1;
	}
}

CMINNode::~CMINNode()
{
	delete [] m_sName;

	for(int p=0; p<m_iPorts; p++) {
		delete m_pPorts[p];
		delete m_pPortTable[p];
	}
	delete [] m_pPorts;
	delete [] m_pPortTable;
}

int CMINNode::g_iCounter=0; /* Initialize static counter */

void CMINNode::SetNodeType(nodeType type)
{
	m_iNodeType = type;
}

void CMINNode::SetNetPort(int port, int net_port)
{
	m_pPorts[port]->net_port = net_port;
}

int CMINNode::GetID()
{
	return m_iID;
}

int CMINNode::GetPorts()
{
	return m_iPorts;
}

int CMINNode::GetIdentifier()
{
	return m_iIdentifier;
}

char * CMINNode::GetName()
{
	return m_sName;
}

int CMINNode::GetSwitches()
{
	return m_iSwitches;
}


int CMINNode::GetInternalPorts()
{
	return m_iInternalPorts;
}

void CMINNode::Init(int port, unsigned long decimal)
{
	m_pPorts[port] = new minNodeStructType;
	m_pPorts[port]->port = port;
	m_pPorts[port]->decimal = decimal;
	m_pPorts[port]->binary = boost::dynamic_bitset<>(m_iBits, decimal);
	m_pPorts[port]->remote_node = NULL;
	m_pPorts[port]->remote_port = NULL;
	m_pPorts[port]->net_port = -1;
}

void CMINNode::InitTable(int switches, int switch_ports, matchingType matching)
{
	int * tmp_cluster_port;
	int cluster_id, port;

	m_iSwitches = switches;

	/*
	 * AQUI SE CONFIGURA LA CONEXION DE LOS SWITCHES QUE COMPONEN CADA NODO
	 * SE TRATA DEL SWITCH 0 (LEFT) Y SWITCH 1 (RIGHT)
	 */
	int left_sw_port;
	int right_sw_port;

	switch (m_iNodeType)
	{
	case switchSTD :
		// Nothing to do
		break;

	case switch2T :
		switch (matching)
		{
		case Alpha:
			/* (1) Correspondencia directa */
			for (int side_port=0; side_port<m_iSidePorts; side_port++)
			{
				m_pPortTable[side_port]->switch_id = TOPGEN_LEFT;
				m_pPortTable[side_port]->switch_port = side_port;
			}

			for (int side_port=m_iSidePorts; side_port<m_iPorts; side_port++)
			{
				m_pPortTable[side_port]->switch_id = TOPGEN_RIGHT;
				m_pPortTable[side_port]->switch_port = side_port - m_iSidePorts;
			}
			break;

		case Bravo:
			if (m_bIsLastStage)
			{
				// Left is full connected. Any internal cross
				for (int node_port=0; node_port<m_iSidePorts; node_port++)
				{
					m_pPortTable[node_port]->switch_id = TOPGEN_LEFT;
					m_pPortTable[node_port]->switch_port = node_port;
				}

				for (int node_port=m_iSidePorts; node_port<m_iPorts; node_port++)
				{
					m_pPortTable[node_port]->switch_id = TOPGEN_RIGHT;
					m_pPortTable[node_port]->switch_port = node_port;
				}
			}
			else
			{
				/*
				 * (2) Asignacion por orden de puerto
				 */
				left_sw_port = 0;
				right_sw_port = 0;
				for (int node_port=0; node_port<m_iSidePorts/2; node_port++)
				{
					// left alpha
					m_pPortTable[node_port]->switch_id = TOPGEN_LEFT;
					m_pPortTable[node_port]->switch_port = left_sw_port;
					left_sw_port++;
				}

				for (int node_port=m_iSidePorts/2; node_port<m_iSidePorts; node_port++)
				{
					// left beta
					m_pPortTable[node_port]->switch_id = TOPGEN_RIGHT;
					m_pPortTable[node_port]->switch_port = right_sw_port;
					right_sw_port++;
				}

				for (int node_port=m_iSidePorts; node_port<m_iSidePorts+(m_iSidePorts/2); node_port++)
				{
					// right alpha
					m_pPortTable[node_port]->switch_id = TOPGEN_LEFT;
					m_pPortTable[node_port]->switch_port = left_sw_port;
					left_sw_port++;
				}

				for (int node_port=m_iSidePorts+(m_iSidePorts/2); node_port<m_iSidePorts*2; node_port++)
				{
					// right beta
					m_pPortTable[node_port]->switch_id = TOPGEN_RIGHT;
					m_pPortTable[node_port]->switch_port = right_sw_port;
					right_sw_port++;
				}
			}
			break;

		case Charlie:
			tmp_cluster_port = new int[m_iSidePorts]; // HACK one cluster per port/2
			for (port=0; port<m_iSidePorts; port++)
				tmp_cluster_port[port] = 0;

			for (port=0; port<m_iPorts; port++)
			{
				cluster_id = m_pSCPManager->GetCluster(m_iStage, m_iSwitch, port);
				m_pPortTable[port]->switch_id = cluster_id;
				m_pPortTable[port]->switch_port = tmp_cluster_port[cluster_id]; // there is a counter per cluster
				tmp_cluster_port[cluster_id]++;
			}

			delete [] tmp_cluster_port;

			break;

		default:
			throw new CException("CMINNode: InitTable(): Matching not supported");
		}
		break;

	default:
		throw new CException("CMINNode: InitTable(): Node type not supported");
	}
}

minNodeStructType * CMINNode::GetPortByPort(int port)
{
	return m_pPorts[port];
}

minNodeStructType * CMINNode::GetLeftPortByDecimal(unsigned long decimal)
{
	for (int i=0; i<m_iSidePorts; i++)
	{
		if (m_pPorts[i]->decimal == decimal)
			return m_pPorts[i];
	}
	return NULL;
}

minNodeStructType * CMINNode::GetRightPortByDecimal(unsigned long decimal)
{
	for (int i=m_iSidePorts; i<m_iPorts; i++)
	{
		if (m_pPorts[i]->decimal == decimal)
			return m_pPorts[i];
	}
	return NULL;
}

int CMINNode::SearchInterval(int at_port, bool first)
{
	minNodeStructType * local_port = GetPortByPort(at_port);
	CMINNode * remote_node = local_port->remote_node;

	while (true)
	{
		if (remote_node)
		{
			if (first)
				local_port = remote_node->GetPortByPort(0);
			else
				local_port = remote_node->GetPortByPort(m_iSidePorts-1);

			remote_node = local_port->remote_node;
		}
		else
		{
			return local_port->net_port;
		}
	}
}

int CMINNode::GetSwitchID(int port)
{
	return m_pPortTable[port]->switch_id;
}

int CMINNode::GetSwitchPort(int port)
{
	return m_pPortTable[port]->switch_port;
}

void CMINNode::Print(FILE * file, const char * type)
{
	switch (m_iNodeType)
	{
	case switchSTD:
	case switch2T:
	default:
		throw new CException("CMINNode: Print: Unknown value");
	}
}
void CMINNode::ResetCounter() {
    g_iCounter = 0; // required when GTest launches various test in sequence
}
}  // namespace topology
