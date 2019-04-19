/*
 *  CUSTOMHU.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 26/09/2015.
 *  Copyright 2015 UCLM. All rights reserved.
 *
 */

#include <math.h>
#include "topgen/custom_hu.hpp"

namespace topgen {

    CCustomHU::CCustomHU(int topology_counter, char * topology_strings[], int routing_counter, char * routing_strings[]) : topology() {
        // This is an example that shows how to read parameters of the configuration file
        if (topology_counter < 3) {
            char * errmsg = new char[128];
            sprintf(errmsg, "CCustomHU::CCustomHU(): TOPOLOGY_TYPE topgen needs at least 3 parameters (found %d)\n", topology_counter);
            throw new CException(errmsg);
        }
        if (strcmp(topology_strings[0], "hu-star")) {
            char * errmsg = new char[128];
            sprintf(errmsg, "CCustomHU::CCustomHU(): topgen topology unknown (found %s)\n", topology_strings[0]);
            throw new CException(errmsg);
        }

        if (routing_counter < 1) {
            char * errmsg = new char[128];
            sprintf(errmsg, "CCustomHU::CCustomHU(): ROUTING_TYPE topgen needs at least 1 parameters (found %d)\n", routing_counter);
            throw new CException(errmsg);
        }
        if (!strcmp(routing_strings[0], "H")) {
            m_eRoutingType = H_type;
        } else if (!strcmp(routing_strings[0], "U")) {
            m_eRoutingType = U_type;
        } else {
            char * errmsg = new char[128];
            sprintf(errmsg, "CCustomHU::CCustomHU(): topgen routing must be middle or bottom (found %s)\n", routing_strings[0]);
            throw new CException(errmsg);
        }

        // init the atributes of the CustomHU topology
        m_iLEFT = 0;
        m_iRIGHT = 1;
        m_iNorthPort = 0;
        m_iSouthPort = 1;
        m_iEastPort = 2;
        m_pNICs[m_iLEFT] = atoi(topology_strings[1]); // param from configuration file
        m_pNICs[m_iRIGHT] = atoi(topology_strings[2]); // param from configuration file
        m_pTotalNICs[m_iLEFT] = m_pNICs[m_iLEFT] * 3; //assuming 3 switch on the left
        m_pTotalNICs[m_iRIGHT] = m_pNICs[m_iRIGHT] * 3; //assuming 3 switch on the right
        m_iTotalNICsOfNetwork = m_pTotalNICs[m_iLEFT] + m_pTotalNICs[m_iRIGHT];
        m_iTotalSwitchesOfNetwork = 2 * 3;

        // left & right sides
        m_pNodes = new CHUNode**[2]();
        for (int side = 0; side < 2; side++) {
            m_pNodes[side] = new CHUNode*[3];
            for (int sw = 0; sw < 3; sw++)
                m_pNodes[side][sw] = NULL;
        }

        m_iNICPort = 0; // port of the NIC connecting to the neighbor switch
        if (m_pNICs[m_iLEFT] > m_pNICs[m_iRIGHT])
            m_iRadix = 3 + m_pNICs[m_iLEFT];
        else
            m_iRadix = 3 + m_pNICs[m_iRIGHT];
        m_iDestinations = 0; // range [0 ...] for IAs
        m_iIdentifiers = 0; // range [0 ...] for switches
        m_iChannels = 0;
        m_pChannelList = new std::list<CHUChannel*>;
    }

    void CCustomHU::PreBuildRoutes() {
        // nothing to do here for this topology
    }

    CCustomHU::~CCustomHU() {
        _freeMemory();

        for (int side = 0; side < 2; side++) {
            for (int sw = 0; sw < 3; sw++)
                delete m_pNodes[side][sw];
            delete [] m_pNodes[side];
        }
        delete [] m_pNodes;

        CHUChannel * ch;
        while (!m_pChannelList->empty()) {
            ch = m_pChannelList->front();
            m_pChannelList->pop_front();
            delete ch;
        }
        delete m_pChannelList;
    }

    void CCustomHU::RunAll() {
        BuildNetwork();
        BuildInterconnection();
        BuildChannels();
        LoadNetwork();
        Vectorize();
        PreBuildRoutes();
        BuildRoutes();
        LoadRoutes();
    }

    void CCustomHU::BuildNetwork() {
        // creating switches of the network with unique identifiers
        for (int side = 0; side < 2; side++) {
            for (int sw = 0; sw < 3; sw++) {
                m_pNodes[side][sw] = new CHUNode(m_iIdentifiers, side, sw, m_iRadix);
                m_iIdentifiers++;
            }
        }
    }

    void CCustomHU::BuildInterconnection() {
        for (int sw = 0; sw < (3 - 1); sw++) { // 3 switches per side minus 1 (the last switch)
            m_pNodes[m_iLEFT][sw]->SetNeighbor(m_iSouthPort, m_pNodes[m_iLEFT][sw + 1]);
        }
        
        for (int sw = 1; sw < 3; sw++) { // 3 switches per side minus 1 (the first switch)
            m_pNodes[m_iLEFT][sw]->SetNeighbor(m_iNorthPort, m_pNodes[m_iLEFT][sw - 1]);
        }

        for (int sw = 0; sw < (3 - 1); sw++) { // 3 switches per side minus 1 (the last switch)
            m_pNodes[m_iRIGHT][sw]->SetNeighbor(m_iSouthPort, m_pNodes[m_iRIGHT][sw + 1]);
        }
        
        for (int sw = 1; sw < 3; sw++) { // 3 switches per side minus 1 (the first switch)
            m_pNodes[m_iRIGHT][sw]->SetNeighbor(m_iNorthPort, m_pNodes[m_iRIGHT][sw - 1]);
        }

        m_pNodes[m_iLEFT][1]->SetNeighbor(m_iEastPort, m_pNodes[m_iRIGHT][1]);
        m_pNodes[m_iRIGHT][1]->SetNeighbor(m_iEastPort, m_pNodes[m_iLEFT][1]);
        m_pNodes[m_iLEFT][2]->SetNeighbor(m_iEastPort, m_pNodes[m_iRIGHT][2]);
        m_pNodes[m_iRIGHT][2]->SetNeighbor(m_iEastPort, m_pNodes[m_iLEFT][2]);
    }

    void CCustomHU::BuildChannels() {
        CHUChannel * ch;
        CHUNode * neighbor_node;
        int neighbor_port;

        for (int side = 0; side < 2; side++) {
            for (int sw = 0; sw < 3; sw++) {
                neighbor_node = m_pNodes[side][sw]->Neighbor(m_iSouthPort);
                neighbor_port = m_iNorthPort;

                // 1) Inter-switch channels (always North to South direction)
                if (sw < (3 - 1)) {
                    if (neighbor_node == NULL) {
                        char * errmsg = new char[200];
                        sprintf(errmsg, "CustomHU::BuildChannels(): neighbor_node is NULL (side %d, sw %d, port %d", side, sw, m_iSouthPort);
                        throw new CException(errmsg);
                    }

                    ch = new CHUChannel(m_iChannels++,
                        nodeSTD_customhu,
                        m_pNodes[side][sw],
                        m_iSouthPort,
                        neighbor_node,
                        neighbor_port);

                    // Note that ch is now loaded in the local list, later we
                    // call _LoadChannel to queue it into the list of the library
                    m_pChannelList->push_back(ch);
                }

                // 2) NIC to switch channels
                for (int port = 3; port < 3 + m_pNICs[side]; port++) {
                    ch = new CHUChannel(m_iChannels++,
                        iaSTD_customhu,
                        m_pNodes[side][sw],
                        port,
                        m_iDestinations);

                    m_iDestinations++;
                    m_pChannelList->push_back(ch);
                }
            }
        }

        // 2) two channels between the two sides
        // 2.1) center channel
        neighbor_node = m_pNodes[m_iLEFT][1]->Neighbor(m_iEastPort);
        neighbor_port = m_iEastPort;

        if (neighbor_node == NULL) {
            throw new CException("CustomHU::BuildChannels(): inter-side channel is NULL");
        }

        ch = new CHUChannel(m_iChannels++,
            nodeSTD_customhu,
            m_pNodes[m_iLEFT][1],
            m_iEastPort,
            neighbor_node,
            neighbor_port);

        m_pChannelList->push_back(ch);

        // 2.2) bottom channel
        neighbor_node = m_pNodes[m_iLEFT][2]->Neighbor(m_iEastPort);
        neighbor_port = m_iEastPort;

        if (neighbor_node == NULL) {
            throw new CException("CustomHU::BuildChannels(): inter-side channel is NULL");
        }

        ch = new CHUChannel(m_iChannels++,
            nodeSTD_customhu,
            m_pNodes[m_iLEFT][2],
            m_iEastPort,
            neighbor_node,
            neighbor_port);

        m_pChannelList->push_back(ch);
    }

    void CCustomHU::BuildRoutes() {
        // nothing to do here for this topology because routers are dynamically determined
        // the method CCustomHU::Routing()
    }

    int CCustomHU::GetNumberOfSwitches() {

        return m_iTotalSwitchesOfNetwork;
    }

    int CCustomHU::GetNumberOfDestinations() {

        return m_iTotalNICsOfNetwork;
    }

    void CCustomHU::LoadNetwork() {
        CInputAdapterType_Node * new_ia;
        CElementType_Node * new_element;

        int ia_ids = 0;
        int node_ids = 0;

        for (int side = 0; side < 2; side++) {
            for (int sw = 0; sw < 3; sw++) {
                // Switches (aka NetNodes)
                new_element = new CElementType_Node("cs",
                    m_pNodes[side][sw]->GetID(),
                    m_pNodes[side][sw]->GetPorts(),
                    0); //there is only one category

                _LoadElement(new_element); //--Load of the switch (aka element)
                node_ids++;

                // NICs connected to switches
                char name[TOPGEN_MAX_STRING_LENGTH];
                for (int port = 3; port < 3 + m_pNICs[side]; port++) {
                    sprintf(name, "ia%d", ia_ids);
                    new_ia = new CInputAdapterType_Node("cia", name, ia_ids, "");
                    _LoadIA(new_ia); //--Load of the NICs/IAs
                    ia_ids++;
                }
            }
        }

        // Channels
        std::list<CHUChannel*>::iterator it = m_pChannelList->begin();
        while (it != m_pChannelList->end()) {
            _LoadChannel((*it)->Create()); //--Load of the channels
            ++it;
        }
    }

    void CCustomHU::LoadRoutes() {
        // nothing to do here for this topology
    }

    int CCustomHU::GetNICPort() {
        return m_iNICPort;
    }

    int CCustomHU::GetRadix() {
        return m_iRadix;
    }

    void CCustomHU::Routing(struct sRoutingHU * routinghu, int * output_port, int * requested_vc) {
        // write your code here
        // we include here the d-mod-k algorithm as example
        int sw = routinghu->current_switch;
        int ivc = routinghu->input_vc;
        int dstNode = routinghu->packet_dst_node;
        int dstRouter = routinghu->packet_dst_router;

        int port = 255;


        //1) direct delivery
        if (sw == dstRouter) {
            int min_dst;
            int max_dst;

            if (sw < 3) {
                //left side
                min_dst = sw * m_pNICs[m_iLEFT];
                max_dst = min_dst + m_pNICs[m_iLEFT] - 1; // switch IDs starts in 0
            } else {
                //right side
                min_dst = m_pTotalNICs[m_iLEFT] + (sw - 3) * m_pNICs[m_iRIGHT]; // 3 switch in the left side
                max_dst = min_dst + m_pNICs[m_iRIGHT] - 1; // switch IDs starts in 0
            }

            int d = min_dst;
            int p = 3; // first out port to end-nodes hu-star
            while (d <= max_dst && p < 255) { //p<255 prevents infinite loops
                if (d == dstNode) {
                    port = p;
                    break;
                }
                d++;
                p++;
            }
            if (p >= 255) {
                throw new CException("CustomHU::Routing(): direct delivery fails (destination end-node not found)");
            }
        } else {
            //2,3) forward packets
            if ((sw < 3 && dstRouter < 3) ||
                (sw >= 3 && dstRouter >= 3)) {
                // move inside the same side
                if (sw < dstRouter)
                    port = m_iSouthPort;
                else
                    port = m_iNorthPort;
            } else if (sw < 3 && dstRouter >= 3) {
                // change left --> right
                if (m_eRoutingType == H_type) {
                    switch (sw) {
                        case 0:
                            port = m_iSouthPort;
                            break;
                        case 1:
                            port = m_iEastPort;
                            break;
                        case 2:
                            port = m_iNorthPort;
                            break;
                        default:
                            throw new CException("CustomHU::Routing(): found unknown switch while moving to to the right side");
                    }
                } else {
                    switch (sw) {
                        case 0:
                        case 1:
                            port = m_iSouthPort;
                            break;
                        case 2:
                            port = m_iEastPort;
                            break;
                        default:
                            throw new CException("CustomHU::Routing(): found unknown switch while moving to to the right side");
                    }
                }
            } else if (sw >= 3 && dstRouter < 3) {
                // change right --> left
                if (m_eRoutingType == H_type) {
                    switch (sw) {
                        case 3:
                            port = m_iSouthPort;
                            break;
                        case 4:
                            port = m_iEastPort;
                            break;
                        case 5:
                            port = m_iNorthPort;
                            break;
                        default:
                            throw new CException("CustomHU::Routing(): found unknown switch while moving to to the left side");
                    }
                } else {
                    switch (sw) {
                        case 3:
                        case 4:
                            port = m_iSouthPort;
                            break;
                        case 5:
                            port = m_iEastPort;
                            break;
                        default:
                            throw new CException("CustomHU::Routing(): found unknown switch while moving to to the left side");
                    }
                }
            } else {
                throw new CException("CustomHU:Routing(): impossible to be here");
            }
        }

        *output_port = port;
        *requested_vc = ivc;
    }
} // namespace topology
