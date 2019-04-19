/*
 * RLFT.cpp
 *
 *  Created on: 20/05/2013
 *      Author: jesus
 */

#include "topgen/rlft.hpp"

namespace topgen {

    CRLFT::CRLFT(int net_ports, int node_ports, connectionType connection_type, rtAlgorithmType rt_alg_type, bool verbose) : topology() {
        m_iNetPorts = net_ports;
        m_iNodePorts = node_ports;
        m_iConnectionType = connection_type;
        m_iRtAlgorithmType = rt_alg_type;
        m_iInternalPorts = 0;

        m_iSidePorts = m_iNodePorts / 2; //CBB - TODO Configure for Slimmed or Fat
        m_iStages = (log(m_iNetPorts / 2) / log(m_iSidePorts));

        /* Initialize the tuple attribute */
        m_pRLFTtuple = new rlftTupleType;
        m_pRLFTtuple->h = m_iStages;
        m_pRLFTtuple->downports = new int [m_iStages];
        m_pRLFTtuple->upports = new int [m_iStages];
        m_pRLFTtuple->parallelports = new int [m_iStages];
        for (int i = 0; i < m_iStages; i++) {
            m_pRLFTtuple->downports[i] = m_iSidePorts;
            m_pRLFTtuple->upports[i] = m_iSidePorts;
            m_pRLFTtuple->parallelports[i] = 1;
        }
        m_pRLFTtuple->downports[m_iStages - 1] = m_iNodePorts;
        m_pRLFTtuple->upports[0] = 1;

        /* Calculate the overall number of switch-nodes and fill the array
            containing the number of switches per stage in the RLFT */
        m_iNodes = 0;
        m_pNodesPerStage = new int [m_iStages];
        for (int i = 0; i < m_iStages; i++) {
            m_pNodesPerStage[i] = (m_iNetPorts / m_iSidePorts);
            if ((i + 1) == m_iStages)
                m_pNodesPerStage[i] = (m_iNetPorts / m_iNodePorts);
            m_iNodes += m_pNodesPerStage[i];
        }

        m_iNodeType = switchSTD;
        m_iNetPoint = standard;
        m_iChannels = 0;
        m_iIdentifiers = 0;

        /* Check the RFLT restrictions */
        _checkRestrictions();
    }

    void CRLFT::_buildSpecialAttributes() {
        m_pNodes = new CRLFTNode**[m_iStages];
        for (int st = 0; st < m_iStages; st++) {
            m_pNodes[st] = new CRLFTNode * [m_pNodesPerStage[st]];
            for (int nd = 0; nd < m_pNodesPerStage[st]; nd++) {
                m_pNodes[st][nd] = NULL;
            }
        }

        m_pChannelList = new std::list<CRLFTChannel*>;

        switch (m_iRtAlgorithmType) {
            case destro:
                m_pDestro = new CDestro(m_iNetPorts, m_iNodePorts);
                break;

            default:
                throw new CException("CRLFT: CRLFT(): Unknown routing algorithm");
        }
    }

    CRLFT::~CRLFT() {

        _freeMemory();

        /* Delete the tuple memory */
        delete [] m_pRLFTtuple->downports;
        delete [] m_pRLFTtuple->upports;
        delete [] m_pRLFTtuple->parallelports;
        delete m_pRLFTtuple;

        for (int st = 0; st < m_iStages; st++) {
            for (int nd = 0; nd < m_pNodesPerStage[st]; nd++)
                delete m_pNodes[st][nd];
            delete [] m_pNodes[st];
        }
        delete [] m_pNodes;
        delete [] m_pNodesPerStage;

        for (int id = 0; id < m_iFirstIAIdentifier; id++)
            delete [] m_pMINRoutes[id];
        delete [] m_pMINRoutes;

        for (int id = 0; id < m_iIdentifiers; id++)
            delete [] m_pSwitchRoutes[id];
        delete [] m_pSwitchRoutes;

        CRLFTChannel * ch;
        while (!m_pChannelList->empty()) {
            ch = m_pChannelList->front();
            m_pChannelList->pop_front();
            delete ch;
        }
        delete m_pChannelList;

        switch (m_iRtAlgorithmType) {
            case destro:
                delete m_pDestro;
                break;

            default: // Unknown routing algorithm
                break; 
        }

    }

    void CRLFT::_checkRestrictions() {
        int result = 1;

        /* Check that the number of nodes is correct */
        for (int i = 0; i < m_iStages; i++) {
            if ((i + 1) == m_iStages)
                result *= m_iNodePorts;
            else
                result *= m_iSidePorts;
        }

        if (result != m_iNetPorts) {
            char * errmsg = new char[256];
            sprintf(errmsg, "CRLFT: CheckRestrictions(): The number of nodes is incorrect: expected %d, read %d", result, m_iNetPorts);
            throw new CException(errmsg);
        }
    }

    CRLFTNode * CRLFT::_GetLeftNodeByDecimal(int stage, unsigned long decimal, int leftports) {
        for (int nd = 0; nd < m_pNodesPerStage[stage]; nd++) {
            for (int p = 0; p < leftports; p++) {
                rlftNodeStructType * node_port = m_pNodes[stage][nd]->GetPortByPort(p);

                if (node_port && node_port->decimal == decimal)
                    return m_pNodes[stage][nd];
            }
        }

        return NULL;
    }

    void CRLFT::_createNodes(int * v, int pos, int stage, int * currentid) {
        int max = 0;
        int nd = 0;

        if (pos == 0) {

            /* Create the RLFT node object */
            nd = (*currentid) % m_pNodesPerStage[stage];
            m_pNodes[stage][nd] = new CRLFTNode(stage, nd, m_iIdentifiers, m_iNodePorts, m_iSidePorts, stage == m_iStages - 1, m_iInternalPorts, m_pSCPManager);
            m_pNodes[stage][nd]->SetNodeType(m_iNodeType);
            m_pNodes[stage][nd]->Init(v, m_iStages);

            switch (m_iNodeType) {
                case switchSTD:
                    m_iIdentifiers += 1;
                    break;

                case switch2T:
                    m_pNodes[stage][nd]->InitTable(2, m_iSidePorts, m_iMatchingType);

                    // Reservo los identificadores necesarios para todos los switches anidados
                    m_iIdentifiers += 2;
                    break;

                default:
                    throw new CException("CRLFTNode: BuildNetwork(): Illegal node type");
            }

            /* Increase the identifier counter */
            *currentid += 1;
        } else {
            if (pos > stage)
                max = m_pRLFTtuple->downports[pos]; // Deal with m[pos]
            else
                max = m_pRLFTtuple->upports[pos]; // Deal with w[pos]

            for (int i = 0; i < max; i++) {
                v[pos] = i;
                _createNodes(v, pos - 1, stage, currentid);
            }
        }
    }

    void CRLFT::RunAll() {
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

    void CRLFT::BuildNetwork() {
        CRLFTNode::ResetCounter();
        int * nodeidtuple = new int [m_iStages];
        int id = 0;

        /* build the specific attributes for this class */
        _buildSpecialAttributes();

        for (int st = 0; st < m_iStages; st++)
            nodeidtuple[st] = 0;

        // creating nodes
        for (int st = 0; st < m_iStages; st++)
            _createNodes(nodeidtuple, m_iStages - 1, st, &id);

        delete [] nodeidtuple;

        m_iFirstIAIdentifier = m_iIdentifiers;

        // node ports
        for (int st = 0; st < m_iStages; st++) {
            int left_decimal = 0;
            int right_decimal = 0;

            for (int nd = 0; nd < m_pNodesPerStage[st]; nd++) {
                if (st < (m_iStages - 1)) {
                    // left ports
                    for (int port = 0; port < m_iSidePorts; port++) {
                        m_pNodes[st][nd]->InitPort(port, left_decimal);

                        if (st == 0)
                            m_pNodes[st][nd]->SetNetPort(port, left_decimal);

                        left_decimal++;
                    }

                    // right ports
                    for (int port = m_iSidePorts; port < m_iNodePorts; port++) {
                        m_pNodes[st][nd]->InitPort(port, right_decimal);

                        right_decimal++;
                    }
                } else {
                    // all ports
                    for (int port = 0; port < m_iNodePorts; port++) {
                        m_pNodes[st][nd]->InitPort(port, right_decimal);

                        right_decimal++;
                    }
                }
            }
        }
    }

    void CRLFT::BuildInterconnection() {
        int q, r;

        for (int st = 0; st < m_iStages - 1; st++) {

            for (int nd = 0; nd < m_pNodesPerStage[st]; nd++) {
                for (int nextnd = 0; nextnd < m_pNodesPerStage[st + 1]; nextnd++) {
                    /*Get the remote node */
                    CRLFTNode * remote_node = m_pNodes[st + 1][nextnd];

                    /* Check if there is a connection*/
                    if (remote_node == NULL)
                        throw new CException("(Build Interconnection ) -> Error in building. NULL remote node to connect");

                    if (remote_node->getIDTuple() == NULL)
                        throw new CException("(Build Interconnection ) -> Error in building. NULL tupla from remote node to connect");

                    if (m_pNodes[st][nd]->checkNodeConnection(remote_node->getIDTuple(), st + 1)) {

                        /* Calculate the local port */
                        q = (remote_node->getIDTuple())[st + 1] + m_iSidePorts;
                        rlftNodeStructType * local_port = m_pNodes[st][nd]->GetPortByPort(q);

                        /* Calculate the remote port */
                        if (!remote_node)
                            throw new CException("CRLFT: BuildInterconnection(): Null remote node");
                        r = (m_pNodes[st][nd]->getIDTuple())[st + 1];
                        rlftNodeStructType * remote_port = remote_node->GetPortByPort(r);

                        /* Connect the links */
                        local_port->remote_node = remote_node;
                        local_port->remote_port = remote_port;
                        remote_port->remote_node = m_pNodes[st][nd];
                        remote_port->remote_port = local_port;
                    }
                }
            }
        }
    }

    void CRLFT::BuildChannels() {
        CRLFTChannel *ch = NULL;

        for (int st = 0; st < (m_iStages - 1); st++) {
            for (int nd = 0; nd < m_pNodesPerStage[st]; nd++) {
                for (int p = m_iSidePorts; p < m_iNodePorts; p++) {
                    rlftNodeStructType * local_port = m_pNodes[st][nd]->GetPortByPort(p);

                    switch (m_iNodeType) {
                        case switchSTD:
                            ch = new CRLFTChannel(m_iChannels++,
                                nodeSTD_min,
                                m_pNodes[st][nd],
                                local_port,
                                local_port->remote_node,
                                local_port->remote_port);
                            break;

                        case switch2T:
                            ch = new CRLFTChannel(m_iChannels++,
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
            for (int nd = 0; nd < m_pNodesPerStage[st]; nd++) {
                switch (m_iNodeType) {
                    case switchSTD:
                        // Nothing to do
                        break;

                    case switch2T:
                        for (int l = 0; l < m_iInternalPorts; l++) {
                            // HACK. The internal link consists of m_iInternalPorts lines
                            // inside interval [m_iSidePorts, m_iSidePorts + m_iInternalPorts]
                            ch = new CRLFTChannel(m_iChannels++,
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

        for (int nd = 0; nd < m_pNodesPerStage[0]; nd++) {
            for (int p = 0; p < m_iSidePorts; p++) {
                switch (m_iNodeType) {
                    case switchSTD:
                        ch = new CRLFTChannel(m_iChannels++,
                            iaSTD_min,
                            m_pNodes[0][nd],
                            m_pNodes[0][nd]->GetPortByPort(p));
                        break;

                    case switch2T:
                        ch = new CRLFTChannel(m_iChannels++,
                            ia2T_min,
                            m_pNodes[0][nd],
                            m_pNodes[0][nd]->GetPortByPort(p));
                        break;

                }

                m_pChannelList->push_back(ch);
            }
        }
    }

    void CRLFT::LoadNetwork() {
        char * name;
        CElementType_Node * new_element;

        // NetPoints (NICs)
        for (int nd = 0; nd < m_pNodesPerStage[0]; nd++) {
            for (int p = 0; p < m_iSidePorts; p++) {
                rlftNodeStructType * local_port = m_pNodes[0][nd]->GetPortByPort(p);
                name = new char[TOPGEN_MAX_STRING_LENGTH];
                sprintf(name, "ia%lu", local_port->decimal);
                CInputAdapterType_Node * new_ia = new CInputAdapterType_Node("cia", name, local_port->decimal, "");
                delete [] name;
                _LoadIA(new_ia);
            }
        }

        // NodeTypes
        for (int st = 0; st < m_iStages; st++) {
            for (int nd = 0; nd < m_pNodesPerStage[st]; nd++) {
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
        std::list<CRLFTChannel*>::iterator it = m_pChannelList->begin();
        while (it != m_pChannelList->end()) {
            _LoadChannel((*it)->Create());
            ++it;
        }
    }

    void CRLFT::PreBuildRoutes() {
        // nothing to do here
    }

    void CRLFT::BuildRoutes() {
        // Initialization of matrixes
        m_pMINRoutes = new char*[m_iFirstIAIdentifier];
        for (int id = 0; id < m_iFirstIAIdentifier; id++) {
            m_pMINRoutes[id] = new char[m_iNetPorts];
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
            for (int nd = 0; nd < m_pNodesPerStage[st]; nd++) {
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
                            throw new CException("Self-routing for MFTs not implemented yet");
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
                                    throw new CException("CRLFT: BuildRoutes(): Node port caused a problem");

                                break;

                            default:
                                throw new CException("CRLFT: BuildRoutes(): unknown node type");
                        }

                        ++it;

                    }

                    delete destinations;
                }
            }
        }
        // DESTRO end
    }

    void CRLFT::LoadRoutes() {
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

    int CRLFT::GetNICPort() {
        return 0; //each IA connects to the switch thru port 0
    }

    void CRLFT::Routing(struct sRoutingRLFT * routing_rlft, int * output_port, int * requested_vc) {
        int sw = routing_rlft->current_switch;
        int dstNode = routing_rlft->packet_dst_node;
        int ivc = routing_rlft->input_vc;
        *output_port = m_pSwitchRoutes[sw][dstNode];
        *requested_vc = ivc;
    }
} /* namespace topology */
