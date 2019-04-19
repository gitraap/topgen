/*
 *  MIN.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <math.h>
#include "topgen/min.hpp"

namespace topgen {

    CMIN::CMIN(int net_ports, int node_ports, connectionType connection_type, rtAlgorithmType rt_alg_type) : topology() {
        m_iNetPorts = net_ports;
        m_iNodePorts = node_ports;
        m_iConnectionType = connection_type;
        m_iRtAlgorithmType = rt_alg_type;
        m_iInternalPorts = 0;

        m_iSidePorts = node_ports / 2;
        m_iStages = MyLog(m_iNetPorts, m_iSidePorts);
        m_iNodesPerStage = m_iNetPorts / m_iSidePorts;
        m_iNodes = m_iStages * m_iNodesPerStage;
        m_iNodeType = switchSTD;
        m_iNetPoint = standard;
        m_iChannels = 0;
        m_iIdentifiers = 0;
        m_iBits = MyLog(m_iNodesPerStage * (m_iSidePorts), 2); //solo potencia de 2!!

        m_pNodes = new CMINNode**[m_iStages];
        for (int st = 0; st < m_iStages; st++) {
            m_pNodes[st] = new CMINNode*[m_iNodesPerStage];
            for (int nd = 0; nd < m_iNodesPerStage; nd++)
                m_pNodes[st][nd] = NULL;
        }

        m_pChannelList = new std::list<CMINChannel*>;

        switch (m_iRtAlgorithmType) {
            case destro:
                m_pDestro = new CDestro(m_iNetPorts, m_iNodePorts);
                break;

            case psdestro:
                m_pPSDestro = new CPSDestro(m_iNetPorts, m_iNodePorts, false, this);
                break;

            case selfrouting:
                m_pPSDestro = new CPSDestro(m_iNetPorts, m_iNodePorts, true, this);
                break;

            default:
                throw new CException("CMIN: CMIN(): Unknown routing algorithm");
        }
    }

    void CMIN::PreBuildRoutes() {
        switch (m_iRtAlgorithmType) {
            case destro:
                break;
            case psdestro:
                m_pPSDestro->initialize();
            case selfrouting:
                break;
            default:
                throw new CException("CMIN: PreBuildRoutes(): Unknown routing algorithm");
        }
    }

    CMIN::~CMIN() {
        _freeMemory();

        for (int st = 0; st < m_iStages; st++) {
            for (int nd = 0; nd < m_iNodesPerStage; nd++)
                delete m_pNodes[st][nd];
            delete [] m_pNodes[st];
        }
        delete [] m_pNodes;

        if (m_pMINRoutes != NULL) {
            for (int id = 0; id < m_iFirstIAIdentifier; id++)
                delete [] m_pMINRoutes[id];
            delete [] m_pMINRoutes;
        }

        for (int id = 0; id < m_iIdentifiers; id++)
            delete [] m_pSwitchRoutes[id];
        delete [] m_pSwitchRoutes;

        CMINChannel * ch;
        while (!m_pChannelList->empty()) {
            ch = m_pChannelList->front();
            m_pChannelList->pop_front();
            delete ch;
        }
        delete m_pChannelList;

        delete m_pDestro;
    }

    void CMIN::RunAll() {
        BuildNetwork();
        BuildInterconnection();
        BuildChannels();
        LoadNetwork();
        Vectorize();
        PreBuildRoutes();
        BuildRoutes();
        LoadRoutes();
        PostVectorize();
    }

    void CMIN::BuildNetwork() {
        // creating nodes
        CMINNode::ResetCounter();
        for (int st = 0; st < m_iStages; st++) {
            for (int nd = 0; nd < m_iNodesPerStage; nd++) {
                m_pNodes[st][nd] = new CMINNode(st, nd, m_iIdentifiers, m_iNodePorts, m_iSidePorts, m_iBits, st == m_iStages - 1, m_iInternalPorts, m_pSCPManager);
                m_pNodes[st][nd]->SetNodeType(m_iNodeType);

                switch (m_iNodeType) {
                    case switchSTD:
                        m_iIdentifiers += 1;
                        break;

                    case switch2T:
                        m_pNodes[st][nd]->InitTable(2, m_iSidePorts, m_iMatchingType);

                        // Reservo los identificadores necesarios para todos los switches anidados
                        m_iIdentifiers += 2;
                        break;

                    default:
                        throw new CException("CMINNode: BuildNetwork(): Illegal node type");
                }
            }
        }

        m_iFirstIAIdentifier = m_iIdentifiers;

        // node ports
        for (int st = 0; st < m_iStages; st++) {
            int left_decimal = 0;
            int right_decimal = 0;

            for (int nd = 0; nd < m_iNodesPerStage; nd++) {
                // left ports
                for (int port = 0; port < m_iSidePorts; port++) {
                    m_pNodes[st][nd]->Init(port, left_decimal);

                    if (st == 0)
                        m_pNodes[st][nd]->SetNetPort(port, left_decimal);

                    left_decimal++;
                }

                // right ports
                for (int port = m_iSidePorts; port < m_iNodePorts; port++) {
                    m_pNodes[st][nd]->Init(port, right_decimal);

                    right_decimal++;
                }
            }
        }
    }

    void CMIN::BuildInterconnection() {
        int ith_bitset;
        int copied;
        int wide_bitset = MyLog(m_iSidePorts, 2);
        int first_bit;
        int last_bit;

        for (int st = 0; st < m_iStages - 1; st++) {
            for (int nd = 0; nd < m_iNodesPerStage; nd++) {
                // I only process from left ports
                for (int p = m_iSidePorts; p < m_iNodePorts; p++) {
                    minNodeStructType * local_port = m_pNodes[st][nd]->GetPortByPort(p);

                    boost::dynamic_bitset<> local_binary_port = local_port->binary;
                    boost::dynamic_bitset<> remote_binary_port = local_port->binary;

                    switch (m_iConnectionType) {
                        case butterfly:
                            ith_bitset = st + 1;
                            first_bit = ith_bitset * wide_bitset;
                            last_bit = first_bit + wide_bitset;
                            copied = 0;
                            for (int bit = first_bit; bit < last_bit && copied < wide_bitset; bit++, copied++) {
                                remote_binary_port[0 + copied] = local_binary_port[bit];
                                remote_binary_port[bit] = local_binary_port[0 + copied];
                            }
                            break;

                        case perfectshuffle:
                            remote_binary_port = (remote_binary_port << wide_bitset);
                            first_bit = local_binary_port.size() - wide_bitset;
                            last_bit = local_binary_port.size();
                            copied = 0;

                            for (int bit = first_bit; bit < last_bit; bit++, copied++)
                                remote_binary_port[copied] = local_binary_port[bit];

                            break;

                        default:
                            throw new CException("CMIN: BuildInterconnection(): Illegal Interconnection Type");
                    }

                    // Connecting links
                    CMINNode * remote_node = _GetLeftNodeByDecimal(st + 1, remote_binary_port.to_ulong());
                    if (!remote_node)
                        throw new CException("CMIN: BuildInterconnection(): Null remote node");

                    minNodeStructType * remote_port = remote_node->GetLeftPortByDecimal(remote_binary_port.to_ulong());

                    local_port->remote_node = remote_node;
                    local_port->remote_port = remote_port;
                    remote_port->remote_node = m_pNodes[st][nd];
                    remote_port->remote_port = local_port;
                }
            }
        }
    }

    void CMIN::BuildChannels() {
        CMINChannel *ch = NULL;

        for (int st = 0; st < m_iStages - 1; st++) {
            for (int nd = 0; nd < m_iNodesPerStage; nd++) {
                for (int p = m_iSidePorts; p < m_iNodePorts; p++) {
                    minNodeStructType * local_port = m_pNodes[st][nd]->GetPortByPort(p);

                    switch (m_iNodeType) {
                        case switchSTD:
                            ch = new CMINChannel(m_iChannels++,
                                nodeSTD_min,
                                m_pNodes[st][nd],
                                local_port,
                                local_port->remote_node,
                                local_port->remote_port);
                            break;

                        case switch2T:
                            ch = new CMINChannel(m_iChannels++,
                                node2T_min,
                                m_pNodes[st][nd],
                                local_port,
                                local_port->remote_node,
                                local_port->remote_port);
                            break;

                    }

                    m_pChannelList->push_back(ch);
                }
            }
        }

        // Inter-switch channels
        for (int st = 0; st < m_iStages; st++) {
            for (int nd = 0; nd < m_iNodesPerStage; nd++) {
                switch (m_iNodeType) {
                    case switchSTD:
                        // Nothing to do
                        break;

                    case switch2T:
                        for (int l = 0; l < m_iInternalPorts; l++) {
                            // HACK. The internal link consists of m_iInternalPorts lines
                            // inside interval [m_iSidePorts, m_iSidePorts + m_iInternalPorts]
                            ch = new CMINChannel(m_iChannels++,
                                inode2T_min,
                                m_pNodes[st][nd],
                                m_iSidePorts + l);

                            m_pChannelList->push_back(ch);
                        }
                        break;
                }
            }
        }

        // IAs -> network nodes
        for (int nd = 0; nd < m_iNodesPerStage; nd++) {
            for (int p = 0; p < m_iSidePorts; p++) {
                switch (m_iNodeType) {
                    case switchSTD:
                        ch = new CMINChannel(m_iChannels++,
                            iaSTD_min,
                            m_pNodes[0][nd],
                            m_pNodes[0][nd]->GetPortByPort(p));
                        break;

                    case switch2T:
                        ch = new CMINChannel(m_iChannels++,
                            ia2T_min,
                            m_pNodes[0][nd],
                            m_pNodes[0][nd]->GetPortByPort(p));
                        break;

                }

                m_pChannelList->push_back(ch);
            }
        }
    }

    void CMIN::BuildRoutes() {
        // Initialization of matrixes
        m_pMINRoutes = new int*[m_iFirstIAIdentifier];
        for (int id = 0; id < m_iFirstIAIdentifier; id++) {
            m_pMINRoutes[id] = new int[m_iNetPorts];
            for (int dest = 0; dest < m_iNetPorts; dest++)
                m_pMINRoutes[id][dest] = -1;
        }

        m_pSwitchRoutes = new int*[m_iIdentifiers];
        for (int id = 0; id < m_iIdentifiers; id++) {
            m_pSwitchRoutes[id] = new int[m_iNetPorts];

            for (int dest = 0; dest < m_iNetPorts; dest++)
                m_pSwitchRoutes[id][dest] = -1;
        }

        // DESTRO Routing Algorithm
        for (int st = 0; st < m_iStages; st++) {
            for (int nd = 0; nd < m_iNodesPerStage; nd++) {
                int identifier = m_pNodes[st][nd]->GetIdentifier();
                std::list<int> * destinations;
                int next_left_internal_port = 0;
                int next_right_internal_port = 0;

                for (int pt = 0; pt < m_iNodePorts; pt++) {
                    switch (m_iRtAlgorithmType) {
                        case destro:
                        {
                            if (pt < m_iSidePorts) {
                                // backward ports
                                destinations = m_pDestro->NbRl(st, nd, pt);
                            } else {
                                // forward ports
                                destinations = m_pDestro->NfRl(st, nd, pt);
                            }
                            break;
                        }
                        case selfrouting:
                        {
                            throw new CException("Self-rouiting for MFTs not implemented yet");
                            break;
                        }
                        case psdestro:
                        {
                            destinations = m_pPSDestro->getDstList(st, nd, pt);
                            break;
                        }
                        default:
                            throw new CException("MIN:BuildRoutes - Routing algorithm not supported in MINs");
                    }


                    std::list<int>::iterator it = destinations->begin();
                    while (it != destinations->end()) {
                        int dest = (*it);
                        m_pMINRoutes[identifier][dest] = pt;
                        int current_node_switch_id = m_pNodes[st][nd]->GetSwitchID(pt); // which is the switch id inside the MIN node ?
                        int current_node_switch_port = m_pNodes[st][nd]->GetSwitchPort(pt); // which is the switch port inside the MIN node ?

                        switch (m_iNodeType) {
                            case switchSTD:
                                m_pSwitchRoutes[identifier][dest] = pt;
                                break;

                            case switch2T:
                                if (current_node_switch_id == TOPGEN_LEFT) {
                                    m_pSwitchRoutes[identifier + TOPGEN_LEFT][dest] = current_node_switch_port;
                                    m_pSwitchRoutes[identifier + TOPGEN_RIGHT][dest] = m_iSidePorts + next_right_internal_port; // Hack: I do a static round robin
                                    next_right_internal_port = (next_right_internal_port + 1) % m_iInternalPorts;
                                } else if (current_node_switch_id == TOPGEN_RIGHT) {
                                    m_pSwitchRoutes[identifier + TOPGEN_RIGHT][dest] = current_node_switch_port;
                                    m_pSwitchRoutes[identifier + TOPGEN_LEFT][dest] = m_iSidePorts + next_left_internal_port; // Hack: I do a static round robin
                                    next_left_internal_port = (next_left_internal_port + 1) % m_iInternalPorts;
                                } else
                                    throw new CException("CMIN: BuildRoutes(): Node port caused a problem");

                                break;

                            default:
                                throw new CException("CMIN: BuildRoutes(): unknown node type");
                        }

                        ++it;

                    }
                    delete destinations;
                }
            }
        }
        // DESTRO end
    }

    void CMIN::Settings(netPointType net_type, nodeType node_type, matchingType matching_type, int internal_ports, const char * pattern_file) {
        m_iNetPoint = net_type;
        m_iNodeType = node_type;
        m_iMatchingType = matching_type;
        m_iInternalPorts = internal_ports;

        if (matching_type == Charlie)
            m_pSCPManager = new CSCPManager(pattern_file, m_iStages, m_iNodesPerStage, CLUSTERS, m_iNodePorts);
    }

    int CMIN::GetNumberOfSwitches() {
        int total;
        switch (m_iNodeType) {
            case switchSTD: total = m_iNodes * 1;
                break;
            case switch2T: total = m_iNodes * 2;
                break;
            default:
                throw new CException("CMIN::GetNumberOfSwitches(): Unknown node type");
        }

        return total;
    }

    int CMIN::GetNodes() {
        return m_iNetPorts; //m_iNodes;
    }

    int CMIN::GetStages() {
        return m_iStages;
    }

    void CMIN::LoadNetwork() {
        char * name;
        CElementType_Node * new_element;

        // NetPoints (NICs)
        for (int nd = 0; nd < m_iNodesPerStage; nd++) {
            for (int p = 0; p < m_iSidePorts; p++) {
                minNodeStructType * local_port = m_pNodes[0][nd]->GetPortByPort(p);
                name = new char[TOPGEN_MAX_STRING_LENGTH];
                sprintf(name, "ia%lu", local_port->decimal);
                CInputAdapterType_Node * new_ia = new CInputAdapterType_Node("cia", name, local_port->decimal, "");
                delete [] name;
                _LoadIA(new_ia);
            }
        }

        // NodeTypes
        for (int st = 0; st < m_iStages; st++) {
            for (int nd = 0; nd < m_iNodesPerStage; nd++) {
                switch (m_iNodeType) {
                    case switchSTD:
                        new_element = new CElementType_Node("cs", m_pNodes[st][nd]->GetIdentifier(), m_iNodePorts, 0);
                        m_pElements->push_back(new_element);
                        break;

                    case switch2T:
                        name = new char[TOPGEN_MAX_STRING_LENGTH];
                        sprintf(name, "s%d-l", m_pNodes[st][nd]->GetIdentifier());
                        new_element = new CElementType_Node("cs", name, m_pNodes[st][nd]->GetIdentifier(), m_iSidePorts + m_iInternalPorts, 0);
                        m_pElements->push_back(new_element);

                        name = new char[TOPGEN_MAX_STRING_LENGTH];
                        sprintf(name, "s%d-r", m_pNodes[st][nd]->GetIdentifier());
                        new_element = new CElementType_Node("cs", name, m_pNodes[st][nd]->GetIdentifier() + 1, m_iSidePorts + m_iInternalPorts, 0);
                        m_pElements->push_back(new_element);
                        break;
                }
            }
        }

        // Channels
        std::list<CMINChannel*>::iterator it = m_pChannelList->begin();
        while (it != m_pChannelList->end()) {
            _LoadChannel((*it)->Create());
            ++it;
        }
    }

    void CMIN::LoadRoutes() {
        // MIN nodes
        switch (m_iNodeType) {
            case switchSTD:
                for (int id = 0; id < m_iFirstIAIdentifier; id++) {
                    for (int dest = 0; dest < m_iNetPorts; dest++) {
                        _LoadRoute(id, dest, m_pSwitchRoutes[id][dest]);
                    }
                }
                break;
            case switch2T:
                // non-last stages
                for (int id = 0; id < m_iFirstIAIdentifier; id += 2) //except switch in the last stage
                {
                    for (int dest = 0; dest < m_iNetPorts; dest++) {
                        _LoadRoute(id + TOPGEN_LEFT, dest, m_pSwitchRoutes[id + TOPGEN_LEFT][dest]);
                        _LoadRoute(id + TOPGEN_RIGHT, dest, m_pSwitchRoutes[id + TOPGEN_RIGHT][dest]);
                    }
                }
                break;
        }
    }

    int * CMIN::GetCoordinates(int idx) {
        int * tmp = new int[2]; // stage + switch
        tmp[0] = idx / m_iNodesPerStage; // stage
        tmp[1] = idx % m_iNodesPerStage; // switch
        return tmp;
    }

    int CMIN::GetSidePorts() {
        return m_iSidePorts;
    }

    std::list<CMINChannel*> * CMIN::GetNodeChannels() {
        std::list<CMINChannel*> * vchannels = new std::list<CMINChannel*>();

        std::list<CMINChannel*>::iterator it = m_pChannelList->begin();
        while (it != m_pChannelList->end()) {
            CMINChannel * ch = (*it);

            if (ch->m_eType == nodeSTD_min) {
                /* load */
                vchannels->push_back(ch);
            } else if (ch->m_eType == iaSTD_min) {
                /* don't load it because KNS loads it already */
                // nothing to do here
            } else {
                /* so far, other type is unexpected */
                throw new CException("CMIN::GetVirtualChannelsWithTuple(): Unexpected node type");
            }

            ++it;
        }
        return vchannels;
    }

    CMINNode * CMIN::_GetNodeByIdentifier(int identifier) {
        for (int st = 0; st < m_iStages; st++)
            for (int nd = 0; nd < m_iNodesPerStage; nd++)
                if (identifier == m_pNodes[st][nd]->GetIdentifier())
                    return m_pNodes[st][nd];

        return NULL;
    }

    CMINNode * CMIN::_GetLeftNodeByDecimal(int stage, unsigned long decimal) {
        for (int nd = 0; nd < m_iNodesPerStage; nd++) {
            for (int p = 0; p < m_iSidePorts; p++) {
                minNodeStructType * node_port = m_pNodes[stage][nd]->GetPortByPort(p);

                if (node_port && node_port->decimal == decimal)
                    return m_pNodes[stage][nd];
            }
        }

        return NULL;
    }

    CMINNode * CMIN::_GetRightNodeByDecimal(int stage, unsigned long decimal) {
        for (int nd = 0; nd < m_iNodesPerStage; nd++) {
            for (int p = m_iSidePorts; p < m_iNodePorts; p++) {
                minNodeStructType * node_port = m_pNodes[stage][nd]->GetPortByPort(p);

                if (node_port->decimal == decimal)
                    return m_pNodes[stage][nd];
            }
        }

        return NULL;
    }

} // namespace topology
