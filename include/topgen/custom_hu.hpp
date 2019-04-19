/*
 *  CustomHU.h
 *  Topology2
 *
 *  Created by Juan Villar on 26/09/2015.
 *  Copyright 2015 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_CUSTOM_HU_HPP
#define TOPGEN_CUSTOM_HU_HPP

#include <math.h>
#include <stdlib.h>
#include <list>
#include <topgen/topology.hpp>
#include <topgen/hu_node.hpp>
#include <topgen/hu_channel.hpp>
#include <topgen/exception.hpp>


namespace topgen {
    struct sRoutingHU {
        int current_switch;
        int input_port;
        int input_tc;
        int input_vc;
        int packet_src_node;
        int packet_dst_node;
        int packet_src_router;
        int packet_dst_router;
    };

    class CHUNode;
    class CHUChannel;

    class CCustomHU : public topology {
    public:
        CCustomHU(int topology_counter, char * topology_strings[], int routing_counter, char * routing_strings[]);
        virtual ~CCustomHU();

        virtual void RunAll();
        virtual void BuildNetwork();
        virtual void BuildInterconnection();
        virtual void BuildChannels();
        virtual void BuildRoutes();

        virtual int GetNumberOfSwitches();
        virtual int GetNumberOfDestinations();
        virtual int GetNICPort();
        virtual int GetRadix();

        virtual void LoadNetwork();
        virtual void LoadRoutes();

        virtual void PreBuildRoutes();

        void Routing(struct sRoutingHU * routinghu, int * output_port, int * requested_vc);

    private:
        CHUNode * _GetNeighborAtPort(int port);

        int m_iLEFT;
        int m_iRIGHT;
        int m_pNICs[2];
        int m_pTotalNICs[2];
        int m_iTotalNICsOfNetwork;
        int m_iTotalSwitchesOfNetwork;
        int m_iNICPort;
        int m_iRadix;
        int m_iNorthPort;
        int m_iSouthPort;
        int m_iEastPort;
        rtAlgorithmType m_eRoutingType;

        int m_iChannels;
        int m_iDestinations;
        int m_iIdentifiers;

        CHUNode *** m_pNodes;
        std::list<CHUChannel*> * m_pChannelList;
    };

} // namespace topology

#endif
