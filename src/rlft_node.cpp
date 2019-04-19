/*
 * RLFTNode.cpp
 *
 *  Created on: 20/05/2013
 *      Author: jesus
 */

#include "topgen/rlft_node.hpp"

namespace topgen {

    CRLFTNode::CRLFTNode(int _stage, int _switch, int identifier, int total_ports, int side_ports, bool isLast, int internal_ports, CSCPManager * scp) {
        m_iNumStages = 0;
        m_iStage = _stage;
        m_iSwitch = _switch;
        m_iIdentifier = identifier;
        m_iPorts = total_ports;
        m_iSidePorts = side_ports;
        m_bIsLastStage = isLast;
        m_iInternalPorts = internal_ports;
        m_pSCPManager = scp;

        m_iSwitches = 1;
        m_sName = new char[TOPGEN_MAX_STRING_LENGTH];
        sprintf(m_sName, "s%d", m_iIdentifier);
        m_iID = g_iCounter++;
        m_iNodeType = switchSTD;

        m_pIDTuple = NULL;
        m_pPorts = new rlftNodeStructType * [m_iPorts];
    }

    CRLFTNode::~CRLFTNode() {
        delete [] m_sName;
        delete [] m_pIDTuple;

        for (int p = 0; p < m_iPorts; p++) {
            delete m_pPorts[p];
        }
        delete [] m_pPorts;
    }

    int CRLFTNode::g_iCounter = 0; /* Initialize static counter */

    void CRLFTNode::SetNodeType(nodeType type) {
        m_iNodeType = type;
    }

    void CRLFTNode::SetNetPort(int port, int net_port) {
        m_pPorts[port]->net_port = net_port;
    }

    int CRLFTNode::GetIdentifier() {
        return m_iIdentifier;
    }

    char * CRLFTNode::GetName() {
        return m_sName;
    }

    int * CRLFTNode::getIDTuple() {
        return m_pIDTuple;
    }

    void CRLFTNode::Init(int * nodetuple, int numstages) {

        m_iNumStages = numstages;
        m_pIDTuple = new int [numstages];
        for (int i = 0; i < m_iNumStages; i++) {
            m_pIDTuple[i] = nodetuple[i];
        }
    }

    void CRLFTNode::InitPort(int port, unsigned long decimal) {
        /* Init the port structures */
        m_pPorts[port] = new rlftNodeStructType;
        m_pPorts[port]->port = port;
        m_pPorts[port]->decimal = decimal;
        m_pPorts[port]->remote_node = NULL;
        m_pPorts[port]->remote_port = NULL;
        m_pPorts[port]->net_port = -1;
    }

    void CRLFTNode::InitTable(int switches, int switch_ports, matchingType matching) {

        m_iSwitches = switches;

        /*
         * AQUI SE CONFIGURA LA CONEXION DE LOS SWITCHES QUE COMPONEN CADA NODO
         * SE TRATA DEL SWITCH 0 (LEFT) Y SWITCH 1 (RIGHT)
         */

        switch (m_iNodeType) {
            case switchSTD:
                // Nothing to do
                break;

            default:
                throw new CException("CRLFTNode: InitTable(): Node type not supported");
        }
    }

    rlftNodeStructType * CRLFTNode::GetPortByPort(int port) {
        return m_pPorts[port];
    }

    bool CRLFTNode::checkNodeConnection(int * tuple, int pos) {

        bool equal = true;

        for (int i = 0; i < m_iNumStages; i++) {
            if (i != pos) {
                if (m_pIDTuple[i] != tuple[i])
                    equal = false;
            }
        }

        return equal;
    }

    int CRLFTNode::GetSwitchID(int port) {
        return m_pPorts[port]->decimal;
    }

    int CRLFTNode::GetSwitchPort(int port) {
        return m_pPorts[port]->port;
    }
    
    void CRLFTNode::ResetCounter() {
        g_iCounter = 0; // required when GTest launches various test in sequence
    }
} /* namespace topology */
