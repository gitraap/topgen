/*
 *  XGFT.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <math.h>
#include "topgen/xgft.hpp"

namespace topgen {

    CXGFT::CXGFT(int radix, int height, int * children, int * parents, bool verbose) : topology() {
        if (parents[0] != 1) {
            char * errmsg = new char[256];
            sprintf(errmsg, "CXGFT::CXGFT(): We assume that each processing node has one network port (W0=1)\n");
            throw new CException(errmsg);
        }

        m_iRadix = radix;
        m_iHeight = height;
        m_iLevels = height + 1;
        m_pChildren = new int[m_iHeight];
        m_pParents = new int[m_iHeight];
        for (int i = 0; i < m_iHeight; i++) {
            m_pChildren[i] = children[i];
            m_pParents[i] = parents[i];
        }

        for (int l = 0; l <= m_iHeight; l++) {
            int up, dp;

            if (l == 0) {
                up = m_pParents[l];
                dp = 0;
            } else if (l < m_iHeight) {
                up = m_pParents[l];
                dp = m_pChildren[l - 1];
            } else {
                up = 0;
                dp = m_pChildren[l - 1];
            }

            if (m_iRadix < dp + up) {
                char * errmsg = new char[256];
                sprintf(errmsg, "CXGFT::CXGFT(): switches at level %d have %d ports (%d needed)\n", l, m_iRadix, dp + up);
                throw new CException(errmsg);
            }
        }

        // Considering that level 0 contains processing nodes
        m_pNodesPerLevel = new int[m_iHeight + 1];
        m_pNodesPerLevel[0] = 1;
        for (int i = 0; i < m_iHeight; i++) {
            m_pNodesPerLevel[0] *= m_pChildren[i];
        }
        for (int k = 1; k <= m_iHeight; k++) {
            int A = 1;
            for (int i = k + 1; i <= m_iHeight; i++) {
                A *= m_pChildren[i - 1];
            }
            int B = 1;
            for (int i = 1; i <= k; i++) {
                B *= m_pParents[i - 1];
            }

            m_pNodesPerLevel[k] = A * B;
        }

        m_pNodes = new CXGFTNode**[m_iHeight + 1]();
        for (int lv = 0; lv <= m_iHeight; lv++) {
            const int nodeshere = m_pNodesPerLevel[lv];
            m_pNodes[lv] = new CXGFTNode*[nodeshere];
            for (int nd = 0; nd < nodeshere; nd++)
                m_pNodes[lv][nd] = NULL;
        }

        m_iNICPort = 0;
        m_iDestinations = 0; // range [0 ...] for IAs
        m_iIdentifiers = 0; // range [0 ...] for switches
        m_iChannels = 0;
        m_pChannelList = new std::list<CXGFTChannel*>;

        m_pRandoms = new double[m_iHeight + 1];
        for (int i = 0; i < m_iHeight + 1; i++) {
            m_pRandoms[i] = ((double) rand() / RAND_MAX);
        }
    }

    void CXGFT::PreBuildRoutes() {

    }

    CXGFT::~CXGFT() {
        _freeMemory();

        if (m_pChildren)
            delete [] m_pChildren;
        if (m_pParents)
            delete [] m_pParents; 
        
        for (int lv = 0; lv < m_iHeight + 1; lv++) {
            for (int nd = 0; nd < m_pNodesPerLevel[lv]; nd++) {
                if (m_pNodes[lv][nd])
                    delete m_pNodes[lv][nd];
            }
            delete [] m_pNodes[lv];
        }
        delete [] m_pNodes;
        
        if (m_pNodesPerLevel)
            delete [] m_pNodesPerLevel;

        CXGFTChannel * ch;
        while (!m_pChannelList->empty()) {
            ch = m_pChannelList->front();
            m_pChannelList->pop_front();
            delete ch;
        }
        delete m_pChannelList;

        if (m_pRandoms)
            delete [] m_pRandoms;
    }

    void CXGFT::RunAll() {
        BuildNetwork();
        BuildInterconnection();
        BuildChannels();
        LoadNetwork();
        Vectorize();
        PreBuildRoutes();
        BuildRoutes();
        LoadRoutes();
    }

    void CXGFT::BuildNetwork() {
        // creating nodes
        for (int lv = 0; lv <= m_iHeight; lv++) {
            for (int nd = 0; nd < m_pNodesPerLevel[lv]; nd++) {
                if (lv == 0) {
                    m_pNodes[lv][nd] = new CXGFTNode(m_iIdentifiers, lv, nd, m_iHeight, m_iRadix);
                    m_iIdentifiers++;
                } else {
                    m_pNodes[lv][nd] = new CXGFTNode(m_iDestinations, lv, nd, m_iHeight, m_iRadix);
                    m_iDestinations++;
                }

                m_pNodes[lv][nd]->WriteTuple(m_pChildren, m_pParents, m_pRandoms);
            }
        }
    }

    void CXGFT::BuildInterconnection() {
        // UP neighbor (direction: level -> level + 1)
        int * up_node_tuple = new int[m_iHeight + 1];
        for (int lv = 0; lv < m_iHeight; lv++) {
            for (int nd = 0; nd < m_pNodesPerLevel[lv]; nd++) {
                int * tuple = m_pNodes[lv][nd]->Tuple();

                for (int X = 0; X < m_pParents[lv]; X++) {
                    for (int i = 0; i <= m_iHeight; i++)
                        up_node_tuple[i] = tuple[i];

                    // a_{l+1} <- X
                    up_node_tuple[lv + 1] = X;

                    CXGFTNode * up_node = _GetNodeByTuple(lv + 1, up_node_tuple, m_pRandoms);
                    if (up_node == NULL)
                        throw new CException("CXGFT::BuildInterconnection(): up node is NULL");
                    const int first_upport = (lv == 0) ? 0 : m_pChildren[lv - 1];
                    m_pNodes[lv][nd]->SetNeighbor(first_upport + X, up_node);
                }
            }
        }
        delete [] up_node_tuple;

        // DOWN neighbor (direction: level +1 -> level)
        int * down_node_tuple = new int[m_iHeight + 1];
        for (int lv = 1; lv <= m_iHeight; lv++) {
            for (int nd = 0; nd < m_pNodesPerLevel[lv]; nd++) {
                int * tuple = m_pNodes[lv][nd]->Tuple();

                for (int Y = 0; Y < m_pChildren[lv - 1]; Y++) {
                    for (int i = 0; i <= m_iHeight; i++)
                        down_node_tuple[i] = tuple[i];

                    // XGFT terminology: a_l <- Y
                    down_node_tuple[lv] = Y;

                    CXGFTNode * down_node = _GetNodeByTuple(lv - 1, down_node_tuple, m_pRandoms);
                    if (down_node == NULL)
                        throw new CException("CXGFT::BuildInterconnection(): down node is NULL");
                    m_pNodes[lv][nd]->SetNeighbor(Y, down_node);
                }
            }
        }
        delete [] down_node_tuple;
    }

    void CXGFT::BuildChannels() {
        CXGFTChannel * ch;

        for (int lv = 0; lv < m_iHeight; lv++) {
            for (int nd = 0; nd < m_pNodesPerLevel[lv]; nd++) {
                const int first_upport = (lv == 0) ? 0 : m_pChildren[lv - 1];
                const int last_upport = (lv == 0) ? m_pParents[lv] : m_pChildren[lv - 1] + m_pParents[lv];
                for (int port = first_upport; port < last_upport; port++) {
                    CXGFTNode * neighbor_node = m_pNodes[lv][nd]->Neighbor(port);
                    int neighbor_port = -1; // -1 indicates not found

                    if (neighbor_node == NULL) {
                        char * errmsg = new char[256];
                        sprintf(errmsg, "CXGFTNode::BuildChannels(): neighbor_node is NULL (lv %d, nd %d, port %d", lv, nd, port);
                        throw new CException(errmsg);
                    }

                    // we need to determine the port of the neighbor node that connects to this node
                    // search the port in the other level
                    int nport = 0;
                    while (nport < m_pChildren[lv]) {
                        CXGFTNode * cur_node = neighbor_node->Neighbor(nport);
                        if (cur_node) {
                            if (m_pNodes[lv][nd]->GetID() == cur_node->GetID()) {
                                neighbor_port = nport;
                                break;
                            }
                        }
                        nport++;
                    }
                    if (neighbor_port == -1) {
                        throw new CException("CXGFTNode: BuildChannels(): neighbor_port not matched");
                    }


                    if (lv == 0) {
                        ch = new CXGFTChannel(m_iChannels++,
                            iaSTD_xgft,
                            neighbor_node, //node level 1
                            neighbor_port,
                            m_pNodes[lv][nd]->GetID()); // IA level 0

                    } else {
                        // Channels from a level to the upper-level switch
                        ch = new CXGFTChannel(m_iChannels++,
                            nodeSTD_xgft,
                            m_pNodes[lv][nd],
                            port,
                            neighbor_node,
                            neighbor_port);
                    }

                    m_pChannelList->push_back(ch);
                }
            }
        }
    }

    void CXGFT::BuildRoutes() {
        // nothing to do here
    }

    int CXGFT::GetNumberOfSwitches() {
        int total = 0;

        for (int lv = 1; lv <= m_iHeight; lv++)
            total += m_pNodesPerLevel[lv];

        return total;
    }

    int CXGFT::GetNumberOfSwitchesInLevel(int level) {
        return m_pNodesPerLevel[level];
    }

    int CXGFT::GetNumberOfDestinations() {
        return m_iDestinations;
    }

    int CXGFT::GetHeight() {
        return m_iHeight;
    }

    void CXGFT::LoadNetwork() {
        CInputAdapterType_Node * new_ia;
        CElementType_Node * new_element;

        int * position = new int[m_iHeight](); // KNS terminology used here
        const int coordinates_len = m_iHeight;
        int * coordinates = new int[coordinates_len];
        int ia_ids = 0;
        int node_ids = 0;
        for (int lv = 0; lv <= m_iHeight; lv++) {
            for (int nd = 0; nd < m_pNodesPerLevel[lv]; nd++) {
                const char * name = m_pNodes[lv][nd]->GetName();
                const int id = m_pNodes[lv][nd]->GetID();
                position[0] = lv;
                position[1] = nd;
                int * tuple = m_pNodes[lv][nd]->Tuple();
                for (int l = 0; l < coordinates_len; l++)
                    coordinates[l] = tuple[l + 1]; // omit the cell 0 of tuple

                if (lv == 0) {
                    // NetPoints (NICs) the nodes at level 0
                    new_ia = new CInputAdapterType_Node("cia",
                        name,
                        id,
                        "");
                    new_ia->WriteTuple(coordinates_len, coordinates);
                    _LoadIA(new_ia);
                    ia_ids++;
                } else {
                    // NetNodes (switches) the nodes at levels greater than 0
                    // XGFT considers the switches are inside dimension 0, position {level, node}
                    new_element = new CElementType_Node("cs",
                        id,
                        m_pNodes[lv][nd]->GetPorts(),
                        0);

                    new_element->WriteTuple(0, position, coordinates_len, coordinates);
                    m_pElements->push_back(new_element);
                    node_ids++;
                }
            }
        }

        // Channels
        std::list<CXGFTChannel*>::iterator it = m_pChannelList->begin();
        while (it != m_pChannelList->end()) {
            _LoadChannel((*it)->Create());
            ++it;
        }
        delete [] coordinates;
        delete [] position;
    }

    void CXGFT::LoadRoutes() {
        // nothing to do here
    }

    int CXGFT::GetNICPort() {
        return m_iNICPort;
    }

    CXGFTNode * CXGFT::_GetNodeByTuple(int level, int * tuple, double * randoms) {
        double tuple_sum = 0.0;
        for (int i = 1; i < m_iHeight + 1; i++) {
            tuple_sum += (tuple[i] * randoms[i]);
        }

        for (int nd = 0; nd < m_pNodesPerLevel[level]; nd++) {
            int * aux_tuple = m_pNodes[level][nd]->Tuple();
            double aux_sum = m_pNodes[level][nd]->Sum();
            // 1) check sum first
            if (tuple_sum == aux_sum) {
                bool found = true;
                for (int i = 1; i <= m_iHeight; i++)//remember cell 0 is not used
                    if (tuple[i] != aux_tuple[i]) {
                        // verify the tuple finally
                        found = false;
                    }

                if (found)
                    return m_pNodes[level][nd];
            }

        }
        return NULL;
    }

} // namespace topology
