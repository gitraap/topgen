/*
 * RLFT.h
 *
 *  Created on: 20/05/2013
 *      Author: jesus
 */

#ifndef TOPGEN_RLFT_HPP
#define TOPGEN_RLFT_HPP

#include <stdlib.h>
#include <math.h>
#include <list>
#include <topgen/topology.hpp>
#include <topgen/min.hpp>
#include <topgen/rlft_node.hpp>
#include <topgen/rlft_channel.hpp>
#include <topgen/exception.hpp>
#include <topgen/destro.hpp>
#include <topgen/psdestro.hpp>
#include <topgen/scp_manager.hpp>

namespace topgen {
    struct sRoutingRLFT {
        int current_switch;
        int input_port;
        int input_tc;
        int input_vc;
        int packet_src_node;
        int packet_dst_node;
        int packet_src_router;
        int packet_dst_router;
    };

    class CRLFTNode;
    class CRLFTChannel;

    /* A Real-Life Fat-Tree (RLFT) is given by the tuple RLFT(h; m[1]...m[h]; w[1]...w[h]; p[1]...p[h])*/
    struct rlftTuple {
        int h; /* h: Indicates the number of stages */
        int * downports; /* m[i]: Indicates the number of down ports per switch at the stage i */
        int * upports; /* w[i]: Indicates the number if up ports per switch at the stage i */
        int * parallelports; /* p[i]: Indicates the number of parallel ports per switch at the stage i */
    };

    typedef rlftTuple rlftTupleType;

    class CRLFT : public topology {
    public:
        CRLFT(int net_ports, int node_ports, connectionType connection_type, rtAlgorithmType rt_alg_type, bool verbose);
        virtual ~CRLFT();

        virtual void RunAll();
        virtual void BuildNetwork();
        virtual void BuildInterconnection();
        virtual void BuildChannels();
        virtual void BuildRoutes();

        virtual void LoadNetwork();
        virtual void LoadRoutes();
        virtual void PreBuildRoutes();

        int GetNICPort();
        void Routing(struct sRoutingRLFT * routing_rlft, int * output_port, int * requested_vc);


    private:
        CRLFTNode * _GetLeftNodeByDecimal(int stage, unsigned long decimal, int leftports);

        void _buildSpecialAttributes();
        void _checkRestrictions();
        void _createNodes(int * v, int pos, int stage, int * currentid);

        int m_iNetPorts;
        int m_iNodePorts;
        int m_iSidePorts;
        int m_iNodes;
        int m_iStages;
        int m_iNodesPerStage;
        int m_iChannels;
        int m_iFirstIAIdentifier;
        int m_iIdentifiers;
        int m_iBits;
        int m_iInternalPorts;

        netPointType m_iNetPoint;
        nodeType m_iNodeType;
        connectionType m_iConnectionType;
        rtAlgorithmType m_iRtAlgorithmType;
        matchingType m_iMatchingType;

        CRLFTNode *** m_pNodes;
        std::list<CRLFTChannel*> * m_pChannelList;

        char ** m_pMINRoutes; // identifier x destination -> output port

        int ** m_pSwitchRoutes; // identifier x destination -> switch output port

        CDestro * m_pDestro;
        CPSDestro * m_pPSDestro;
        CSCPManager * m_pSCPManager;

        rlftTupleType * m_pRLFTtuple;
        int * m_pNodesPerStage;

    };

} /* namespace topology */
#endif /* TOPGEN_RLFT_HPP */
