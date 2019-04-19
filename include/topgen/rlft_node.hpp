/*
 * RLFTNode.h
 *
 *  Created on: 20/05/2013
 *      Author: jesus
 */

#ifndef TOPGEN_RLFT_NODE_HPP
#define TOPGEN_RLFT_NODE_HPP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <topgen/topology.hpp>
#include <topgen/exception.hpp>
#include <topgen/scp_manager.hpp>

namespace topgen {

    class CRLFTNode;

    struct rlftnodeStruct {
        int port;
        unsigned long decimal;
        CRLFTNode * remote_node;
        struct rlftnodeStruct * remote_port;
        int net_port;
    };

    typedef struct rlftnodeStruct rlftNodeStructType;

    class CRLFTNode {
    public:
        CRLFTNode(int _stage, int _switch, int identifier, int total_ports, int side_ports, bool isLast, int internal_ports, CSCPManager * scp);
        virtual ~CRLFTNode();

        void SetNodeType(nodeType type);
        void SetNetPort(int port, int net_port);

        int GetIdentifier();
        char * GetName();
        int * getIDTuple();

        void Init(int * nodetuple, int numstages);
        void InitPort(int port, unsigned long decimal);
        void InitTable(int switches, int switch_ports, matchingType matching);

        rlftNodeStructType * GetPortByPort(int port);
        bool checkNodeConnection(int * tuple, int pos);

        int GetSwitchID(int port);
        int GetSwitchPort(int port);

        static void ResetCounter();
        
    private:
        int m_iNumStages;
        int m_iStage;
        int m_iSwitch;
        int m_iIdentifier;
        int m_iPorts;
        int m_iSidePorts;
        int m_bIsLastStage;
        int m_iInternalPorts;
        CSCPManager * m_pSCPManager;

        char * m_sName;
        int m_iID;
        nodeType m_iNodeType;

        rlftNodeStructType ** m_pPorts;
        int * m_pIDTuple;

        // Embedded Switches
        int m_iSwitches;

    protected:
        static int g_iCounter;

    };

} /* namespace topology */
#endif /* TOPGEN_RLFT_NODE_HPP */
