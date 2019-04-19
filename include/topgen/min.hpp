/*
 *  MIN.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_MIN_HPP
#define TOPGEN_MIN_HPP

#include <stdlib.h>
#include <math.h>
#include <list>
#include <topgen/topology.hpp>
#include <topgen/min_node.hpp>
#include <topgen/min_channel.hpp>
#include <topgen/exception.hpp>
#include <topgen/destro.hpp>
#include <topgen/psdestro.hpp>
#include <topgen/scp_manager.hpp>

namespace topgen {

#define CLUSTERS 2

    class CMINNode;
    class CMINChannel;

    class CMIN : public topology {
    public:
        CMIN(int net_ports, int node_ports, connectionType connection_type, rtAlgorithmType rt_alg_type);
        virtual ~CMIN();

        virtual void RunAll();
        virtual void BuildNetwork();
        virtual void BuildInterconnection();
        virtual void BuildChannels();
        virtual void BuildRoutes();

        virtual void Settings(netPointType net_type, nodeType node_type, matchingType matching_type, int internal_ports, const char * pattern_file);
        virtual int GetNumberOfSwitches();
        virtual int GetNodes();
        virtual int GetStages();

        virtual void LoadNetwork();
        virtual void LoadRoutes();

        virtual void PreBuildRoutes();

        virtual int * GetCoordinates(int idx);
        virtual int GetSidePorts();
        virtual std::list<CMINChannel*> * GetNodeChannels();

    private:
        CMINNode * _GetNodeByIdentifier(int identifier);
        CMINNode * _GetLeftNodeByDecimal(int stage, unsigned long decimal);
        CMINNode * _GetRightNodeByDecimal(int stage, unsigned long decimal);

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

        CMINNode *** m_pNodes;
        std::list<CMINChannel*> * m_pChannelList;

        int ** m_pMINRoutes; // identifier x destination -> output port

        int ** m_pSwitchRoutes; // identifier x destination -> switch output port

        CDestro * m_pDestro;
        CPSDestro * m_pPSDestro;
        CSCPManager * m_pSCPManager;
    };

} // namespace topology

#endif
