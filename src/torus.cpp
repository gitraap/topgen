/*
 *  Torus.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include "topgen/torus.hpp"

namespace topgen {

    CTorus::CTorus(const int n, const int k1, const int k2, const int k3) : topology() {
        switch (n) {
            case 2:
                m_pKary = new int[2];
                m_pKary[0] = k1;
                m_pKary[1] = k2;
                m_iNcube = n;
                m_iNodes = k1 * k2;
                break;

            case 3:
                m_pKary = new int[3];
                m_pKary[0] = k1;
                m_pKary[1] = k2;
                m_pKary[2] = k3;
                m_iNcube = n;
                m_iNodes = k1 * k2 * k3;
                break;

            default:
                throw new CException("CTorus: CTorus(): dimension not supported");
        }

        m_iChannels = 0;
        m_iIdentifiers = 0;
        m_pChannelList = new std::list<CDIRChannel*>;
        m_iNodeType = switchSTD;
        m_iINicPort = 4; // for T-switches
        m_iIIntraPort = 3; // for T-switches
    }

    CTorus::~CTorus() {
        switch (m_iNcube) {
            case 2:
                for (int i = 0; i < GetKary(1); i++) {
                    for (int j = 0; j < GetKary(2); j++)
                        delete m_pNodes2D[i][j];
                    delete [] m_pNodes2D[i];
                }
                delete [] m_pNodes2D;
                break;

            case 3:
                for (int i = 0; i < GetKary(1); i++) {
                    for (int j = 0; j < GetKary(2); j++) {
                        for (int k = 0; k < GetKary(3); k++)
                            delete m_pNodes3D[i][j][k];
                        delete [] m_pNodes3D[i][j];
                    }
                    delete [] m_pNodes3D[i];
                }
                delete [] m_pNodes3D;
                break;

            default: // Dimension not supported
                break;
        }

        delete [] m_pKary;
        while (m_pChannelList->size() > 0) {
            CDIRChannel* ch = m_pChannelList->front();
            m_pChannelList->pop_front();
            delete ch;
        }
        delete m_pChannelList;
        _freeMemory();
    }

    void CTorus::RunAll() {
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

    void CTorus::BuildNetwork() {
        //initialization
        CDIRNode::ResetCounter();
        switch (m_iNcube) {
            case 2:
                m_pNodes2D = new CDIRNode**[GetKary(1)];
                for (int i = 0; i < GetKary(1); i++) {
                    m_pNodes2D[i] = new CDIRNode*[GetKary(2)];
                    for (int j = 0; j < GetKary(2); j++)
                        m_pNodes2D[i][j] = NULL;
                }

                break;

            case 3:
                m_pNodes3D = new CDIRNode***[GetKary(1)];
                for (int i = 0; i < GetKary(1); i++) {
                    m_pNodes3D[i] = new CDIRNode**[GetKary(2)];
                    for (int j = 0; j < GetKary(2); j++) {
                        m_pNodes3D[i][j] = new CDIRNode*[GetKary(3)];
                        for (int k = 0; k < GetKary(3); k++)
                            m_pNodes3D[i][j][k] = NULL;
                    }
                }

                break;

            default:
                throw new CException("CTorus: CTorus(): dimension not supported");
        }

        // building network nodes
        switch (m_iNcube) {
            case 2:
                for (int i = 0; i < GetKary(1); i++)
                    for (int j = 0; j < GetKary(2); j++) {
                        int extpts = _ExternalPorts(i, j);
                        m_pNodes2D[i][j] = new CDIRNode(2, extpts + 1, m_iIdentifiers, m_iNodeType);
                        m_pNodes2D[i][j]->SetPosition(1, i);
                        m_pNodes2D[i][j]->SetPosition(2, j);
                        m_pNodes2D[i][j]->SetNicPort(4);
                        m_pNodes2D[i][j]->SetNetPort(m_iIdentifiers);
                        m_iIdentifiers++;
                    }

                break;

            case 3:
                for (int i = 0; i < GetKary(1); i++)
                    for (int j = 0; j < GetKary(2); j++)
                        for (int k = 0; k < GetKary(3); k++) {
                            int extpts = _ExternalPorts(i, j, k);
                            m_pNodes3D[i][j][k] = new CDIRNode(3, extpts + 1, m_iIdentifiers, m_iNodeType);
                            m_pNodes3D[i][j][k]->SetPosition(1, i);
                            m_pNodes3D[i][j][k]->SetPosition(2, j);
                            m_pNodes3D[i][j][k]->SetPosition(3, k);
                            m_pNodes3D[i][j][k]->SetNicPort(6);
                            m_pNodes3D[i][j][k]->SetNetPort(m_iIdentifiers);
                            m_iIdentifiers += 1;

                            if (m_iNodeType == switch2T) {
                                m_pNodes3D[i][j][k]->SetNetPort2(m_iIdentifiers);
                                m_iIdentifiers += 1;
                            }
                        }

                break;

            default:
                throw new CException("CTorus: BuildTopology(): dimension not supported");
        }
    }

    void CTorus::BuildRoutes() {

    }

    void CTorus::BuildInterconnection() {
        int i, j, k;

        switch (m_iNcube) {
            case 2:
                for (i = 0; i < GetKary(1); i++) {
                    for (j = 0; j < GetKary(2); j++) {
                        m_pNodes2D[i][j]->SetNext(1, m_pNodes2D[ _Mod(1, i + 1) ][j]);
                        m_pNodes2D[i][j]->SetPrev(1, m_pNodes2D[ _Mod(1, i - 1) ][j]);

                        m_pNodes2D[i][j]->SetNext(2, m_pNodes2D[i][ _Mod(2, j + 1) ]);
                        m_pNodes2D[i][j]->SetPrev(2, m_pNodes2D[i][ _Mod(2, j - 1) ]);
                    }
                }
                break;

            case 3:
                for (i = 0; i < GetKary(1); i++) {
                    for (j = 0; j < GetKary(2); j++) {
                        for (k = 0; k < GetKary(3); k++) {
                            m_pNodes3D[i][j][k]->SetNext(1, m_pNodes3D[ _Mod(1, i + 1) ][j][k]);
                            m_pNodes3D[i][j][k]->SetPrev(1, m_pNodes3D[ _Mod(1, i - 1) ][j][k]);

                            m_pNodes3D[i][j][k]->SetNext(2, m_pNodes3D[i][ _Mod(2, j + 1) ][k]);
                            m_pNodes3D[i][j][k]->SetPrev(2, m_pNodes3D[i][ _Mod(2, j - 1) ][k]);

                            m_pNodes3D[i][j][k]->SetNext(3, m_pNodes3D[i][j][ _Mod(3, k + 1) ]);
                            m_pNodes3D[i][j][k]->SetPrev(3, m_pNodes3D[i][j][ _Mod(3, k - 1) ]);

                            m_pNodes3D[i][j][k]->SetupInternalConfiguration(); // HACK: for twin cards
                        }
                    }
                }
                break;

            default:
                throw new CException("CTorus: BuildInterconnection(): dimension not supported");
        }
    }

    void CTorus::BuildChannels() {
        CDIRChannel * ch;
        int i, j, k;

        switch (m_iNcube) {
            case 2:
                for (i = 0; i < GetKary(1); i++) {
                    for (j = 0; j < GetKary(2); j++) {
                        ch = new CDIRChannel(nodeSTD_torus,
                            m_iChannels++,
                            m_pNodes2D[i][j],
                            m_pNodes2D[i][j]->GetNextLink(1),
                            m_pNodes2D[ _Mod(1, i + 1) ][j],
                            m_pNodes2D[ _Mod(1, i + 1) ][j]->GetPrevLink(1));

                        m_pChannelList->push_back(ch);

                        ch = new CDIRChannel(nodeSTD_torus,
                            m_iChannels++,
                            m_pNodes2D[i][j],
                            m_pNodes2D[i][j]->GetNextLink(2),
                            m_pNodes2D[i][ _Mod(2, j + 1) ],
                            m_pNodes2D[i][ _Mod(2, j + 1) ]->GetPrevLink(2));

                        m_pChannelList->push_back(ch);
                    }
                }

                // IAs
                for (i = 0; i < GetKary(1); i++) {
                    for (j = 0; j < GetKary(2); j++) {
                        ch = new CDIRChannel(iaSTD_torus,
                            m_iChannels++,
                            m_pNodes2D[i][j],
                            m_pNodes2D[i][j]->GetNicPort(),
                            NULL,
                            -1);

                        m_pChannelList->push_back(ch);
                    }
                }

                break;

            case 3:
                for (i = 0; i < GetKary(1); i++) {
                    for (j = 0; j < GetKary(2); j++) {
                        for (k = 0; k < GetKary(3); k++) {
                            channelType channel_type;

                            switch (m_iNodeType) {
                                case switchSTD:
                                    channel_type = nodeSTD_torus;
                                    break;

                                case switch2T:
                                    channel_type = node2T_torus;
                                    break;

                                default:
                                    throw new CException("CTorus: CTorus(): NodeType unknown");
                            }

                            ch = new CDIRChannel(channel_type,
                                m_iChannels++,
                                m_pNodes3D[i][j][k],
                                m_pNodes3D[i][j][k]->GetNextLink(1),
                                m_pNodes3D[ _Mod(1, i + 1) ][j][k],
                                m_pNodes3D[ _Mod(1, i + 1) ][j][k]->GetPrevLink(1));

                            m_pChannelList->push_back(ch);

                            ch = new CDIRChannel(channel_type,
                                m_iChannels++,
                                m_pNodes3D[i][j][k],
                                m_pNodes3D[i][j][k]->GetNextLink(2),
                                m_pNodes3D[i][ _Mod(2, j + 1) ][k],
                                m_pNodes3D[i][ _Mod(2, j + 1) ][k]->GetPrevLink(2));

                            m_pChannelList->push_back(ch);

                            ch = new CDIRChannel(channel_type,
                                m_iChannels++,
                                m_pNodes3D[i][j][k],
                                m_pNodes3D[i][j][k]->GetNextLink(3),
                                m_pNodes3D[i][j][ _Mod(3, k + 1) ],
                                m_pNodes3D[i][j][ _Mod(3, k + 1) ]->GetPrevLink(3));

                            m_pChannelList->push_back(ch);

                            // Inter-card channels
                            if (m_iNodeType == switch2T) {
                                ch = new CDIRChannel(inode2T_torus,
                                    m_iChannels++,
                                    m_pNodes3D[i][j][k],
                                    -1,
                                    NULL,
                                    -1);

                                m_pChannelList->push_back(ch);
                            }
                        }
                    }
                }

                // IAs
                for (i = 0; i < GetKary(1); i++) {
                    for (j = 0; j < GetKary(2); j++) {
                        for (k = 0; k < GetKary(3); k++) {
                            switch (m_iNodeType) {
                                case switchSTD:
                                    ch = new CDIRChannel(iaSTD_torus,
                                        m_iChannels++,
                                        m_pNodes3D[i][j][k],
                                        m_pNodes3D[i][j][k]->GetNicPort(),
                                        NULL,
                                        -1);

                                    m_pChannelList->push_back(ch);
                                    break;

                                case switch2T:
                                    ch = new CDIRChannel(ia2T_torus_top,
                                        m_iChannels++,
                                        m_pNodes3D[i][j][k],
                                        -1,
                                        NULL,
                                        -1);

                                    m_pChannelList->push_back(ch);

                                    ch = new CDIRChannel(ia2T_torus_bottom,
                                        m_iChannels++,
                                        m_pNodes3D[i][j][k],
                                        -1,
                                        NULL,
                                        -1);

                                    m_pChannelList->push_back(ch);
                                    break;

                                default:
                                    throw new CException("CTorus::BuildChannels(): NodeType unknown");
                            }
                        }
                    }
                }

                break;

            default:
                throw new CException("CTorus: PrintTopology(): Dimension not supported");
        }
    }

    void CTorus::Settings(nodeType node_type) {
        m_iNodeType = node_type;
    }

    int CTorus::GetNumberOfSwitches() {
        return m_iNodes;
    }

    int CTorus::GetNumberOfDestinations() {
        return m_iIdentifiers;
    }

    int CTorus::GetNodes() {
        return m_iNodes;
    }

    inline void CTorus::SetKary(const int dimension, const int kary) {
        m_pKary[dimension - 1] = kary;
    }

    inline int CTorus::GetKary(const int dimension) {
        return m_pKary[dimension - 1];
    }

    unsigned int CTorus::GetSameDimensionOutport(unsigned int node_id, unsigned int link_id)
    {
        int port = UINT_MAX;

        switch (m_iNcube)
        {
            case 2:
                switch (m_iNodeType)
                {
                    case switchSTD:
                        switch (link_id) {
                            case 0: port = 1; break;
                            case 1: port = 0; break;
                            case 2: port = 3; break;
                            case 3: port = 2; break;
                        }
                        break;
                    case switch2T:
                        // TODO request nodes
                        break;
                    default:
                        throw new CException("CTorus: GetSameDimensionOutport(): NodeType unknown");
                }
                break;

            case 3:
                switch (m_iNodeType)
                {
                    case switchSTD:
                        switch (link_id) {
                            case 0: port = 1; break;
                            case 1: port = 0; break;
                            case 2: port = 3; break;
                            case 3: port = 2; break;
                            case 4: port = 5; break;
                            case 5: port = 4; break;
                        }
                        break;

                    case switch2T:
                        // request nodes
                        break;
                    default:
                        throw new CException("CTorus: GetSameDimensionOutport(): NodeType unknown");
                }
                break;

            default:
                throw new CException("CTorus: GetSameDimensionOutport(): Dimension not supported");
                break;
        }

        return port;
    }

    void CTorus::LoadNetwork() {
        CInputAdapterType_Node * new_ia;
        CElementType_Node * new_element;
        char * name = new char[TOPGEN_MAX_STRING_LENGTH];

        // NetPoints
        switch (m_iNcube) {
            case 2:
                for (int i = 0; i < GetKary(1); i++)
                    for (int j = 0; j < GetKary(2); j++) {
                        snprintf(name, TOPGEN_MAX_STRING_LENGTH, "ia%d", m_pNodes2D[i][j]->GetIdentifier());
                        new_ia = new CInputAdapterType_Node("cia", name, m_pNodes2D[i][j]->GetIdentifier(), "");
                        _LoadIA(new_ia);
                    }
                break;

            case 3:
                for (int i = 0; i < GetKary(1); i++)
                    for (int j = 0; j < GetKary(2); j++)
                        for (int k = 0; k < GetKary(3); k++) {
                            switch (m_iNodeType) {
                                case switchSTD:
                                    snprintf(name, TOPGEN_MAX_STRING_LENGTH, "ia%d", m_pNodes3D[i][j][k]->GetIdentifier());
                                    new_ia = new CInputAdapterType_Node("cia", name, m_pNodes3D[i][j][k]->GetIdentifier(), "");
                                    _LoadIA(new_ia);

                                    break;

                                case switch2T:
                                    snprintf(name, TOPGEN_MAX_STRING_LENGTH, "ia%d", m_pNodes3D[i][j][k]->GetIdentifier() + TOPGEN_TOP);
                                    new_ia = new CInputAdapterType_Node("cia", name, m_pNodes3D[i][j][k]->GetIdentifier() + TOPGEN_TOP, "");
                                    _LoadIA(new_ia);

                                    snprintf(name, TOPGEN_MAX_STRING_LENGTH, "ia%d", m_pNodes3D[i][j][k]->GetIdentifier() + TOPGEN_BOTTOM);
                                    new_ia = new CInputAdapterType_Node("cia", name, m_pNodes3D[i][j][k]->GetIdentifier() + TOPGEN_BOTTOM, "");
                                    _LoadIA(new_ia);

                                    break;

                                default:
                                    throw new CException("CTorus: LoadNetwork(): NodeType unknown");
                            }
                        }
                break;

            default:
                throw new CException("CTorus: LoadNetwork(): Dimension not supported");
        }
        delete [] name;

        // NodeTypes
        switch (m_iNcube) {
            case 2:
                for (int i = 0; i < GetKary(1); i++)
                    for (int j = 0; j < GetKary(2); j++) {
                        new_element = new CElementType_Node("cs", m_pNodes2D[i][j]->GetIdentifier(), m_pNodes2D[i][j]->GetPorts(), 0);
                        m_pElements->push_back(new_element);
                    }
                break;

            case 3:
                for (int i = 0; i < GetKary(1); i++)
                    for (int j = 0; j < GetKary(2); j++)
                        for (int k = 0; k < GetKary(3); k++) {
                            switch (m_iNodeType) {
                                case switchSTD:
                                    new_element = new CElementType_Node("cs", m_pNodes3D[i][j][k]->GetIdentifier(), m_pNodes3D[i][j][k]->GetPorts(), 0);
                                    m_pElements->push_back(new_element);
                                    break;

                                case switch2T:
                                    new_element = new CElementType_Node("cs", m_pNodes3D[i][j][k]->GetIdentifier() + TOPGEN_TOP, 5, 0);
                                    m_pElements->push_back(new_element);

                                    new_element = new CElementType_Node("cs", m_pNodes3D[i][j][k]->GetIdentifier() + TOPGEN_BOTTOM, 5, 0);
                                    m_pElements->push_back(new_element);
                                    break;

                                default:
                                    throw new CException("CTorus: LoadNetwork(): NodeType unknown");
                            }
                        }
                break;

            default:
                throw new CException("CTorus: LoadNetwork(): Dimension not supported");
        }

        // Channels

        std::list<CDIRChannel*>::iterator it = m_pChannelList->begin();
        while (it != m_pChannelList->end()) {
            _LoadChannel((*it)->Create());
            ++it;
        }
    }

    void CTorus::LoadRoutes() {
        CDIRNode * current_node;
        CDIRNode * dest_node;
        int current_i, current_j, current_k;
        int dest_i, dest_j, dest_k;
        int m_i, m_j, m_k;
        int delta_i, delta_j, delta_k;
        int iport;

        for (int id = 0; id < m_iIdentifiers; id++) {
            for (int dest = 0; dest < GetNumberOfDestinations(); dest++) {
                switch (m_iNcube) {
                    case 2:
                        #pragma omp parallel sections num_threads(2)
                        {
                            #pragma omp section
                            {
                                current_node = _SearchNode(id);
                                current_i = current_node->GetPosition(1);
                                current_j = current_node->GetPosition(2);
                            }
                            #pragma omp section
                            {
                                dest_node = _SearchNode(dest);
                                dest_i = dest_node->GetPosition(1);
                                dest_j = dest_node->GetPosition(2);
                            }
                        }

                        /*
                         * Dimension Order Routing (Dally's book, page 166)
                         */

                        m_i = ((dest_i - current_i) + GetKary(1)) % GetKary(1); // Note: We suppose that -1 mod 6 is 5
                        delta_i = m_i - ((m_i <= GetKary(1) / 2) ? 0 : GetKary(1));

                        if (delta_i < 0) {
                            _LoadRoute(id, dest, current_node->GetPrevLink(1));
                        } else if (delta_i > 0) {
                            _LoadRoute(id, dest, current_node->GetNextLink(1));
                        } else {
                            m_j = ((dest_j - current_j) + GetKary(2)) % GetKary(2);
                            delta_j = m_j - ((m_j <= GetKary(2) / 2) ? 0 : GetKary(2));

                            if (delta_j < 0) {
                                _LoadRoute(id, dest, current_node->GetPrevLink(2));
                            } else if (delta_j > 0) {
                                _LoadRoute(id, dest, current_node->GetNextLink(2));
                            } else {
                                _LoadRoute(id, dest, current_node->GetNicPort());
                            }
                        }
                        break;

                    case 3:
                        #pragma omp parallel sections num_threads(2)
                        {
                            #pragma omp section
                            {
                                current_node = _SearchNode(id);
                                current_i = current_node->GetPosition(1);
                                current_j = current_node->GetPosition(2);
                                current_k = current_node->GetPosition(3);
                            }
                            #pragma omp section
                            {
                                dest_node = _SearchNode(dest);
                                dest_i = dest_node->GetPosition(1);
                                dest_j = dest_node->GetPosition(2);
                                dest_k = dest_node->GetPosition(3);
                            }
                        }

                        /*
                         * Dimension Order Routing (Dally's book, page 166)
                         */

                        m_i = ((dest_i - current_i) + GetKary(1)) % GetKary(1);
                        delta_i = m_i - ((m_i <= GetKary(1) / 2) ? 0 : GetKary(1));

                        if (delta_i < 0) {
                            iport = _CalculateInternalPort(current_node, dest_node, id, dest, current_node->GetPrevLink(1));
                            _LoadRoute(id, dest, iport);
                        } else if (delta_i > 0) {
                            iport = _CalculateInternalPort(current_node, dest_node, id, dest, current_node->GetNextLink(1));
                            _LoadRoute(id, dest, iport);
                        } else {
                            m_j = ((dest_j - current_j) + GetKary(2)) % GetKary(2);
                            delta_j = m_j - ((m_j <= GetKary(2) / 2) ? 0 : GetKary(2));

                            if (delta_j < 0) {
                                iport = _CalculateInternalPort(current_node, dest_node, id, dest, current_node->GetPrevLink(2));
                                _LoadRoute(id, dest, iport);
                            } else if (delta_j > 0) {
                                iport = _CalculateInternalPort(current_node, dest_node, id, dest, current_node->GetNextLink(2));
                                _LoadRoute(id, dest, iport);
                            } else {
                                m_k = ((dest_k - current_k) + GetKary(3)) % GetKary(3);
                                delta_k = m_k - ((m_k <= GetKary(3) / 2) ? 0 : GetKary(3));

                                if (delta_k < 0) {
                                    iport = _CalculateInternalPort(current_node, dest_node, id, dest, current_node->GetPrevLink(3));
                                    _LoadRoute(id, dest, iport);
                                } else if (delta_k > 0) {
                                    iport = _CalculateInternalPort(current_node, dest_node, id, dest, current_node->GetNextLink(3));
                                    _LoadRoute(id, dest, iport);
                                } else {
                                    iport = _CalculateInternalPort(current_node, dest_node, id, dest, current_node->GetNicPort());
                                    _LoadRoute(id, dest, iport);
                                }
                            }
                        }

                        break;

                    default:
                        throw new CException("CTorus: Dimension not supported");
                }
            }
        }
    }

    inline int CTorus::_Mod(const int dimension, const int x) {
        if (0 <= x && x < GetKary(dimension))
            return x;
        else if (x == -1)
            return GetKary(dimension) - 1;
        else if (x == GetKary(dimension))
            return 0;
        else
            throw new CException("CTorus: _Mod(): Out of bound");
    }

    inline int CTorus::_ExternalPorts(const int i, const int j) {
        const int a = 2;
        const int b = 2;
        return a + b;
    }

    inline int CTorus::_ExternalPorts(const int i, const int j, const int k) {
        const int a = 2;
        const int b = 2;
        const int c = 2;
        return a + b + c;
    }

    CDIRNode * CTorus::_SearchNode(const int identifier) {
        switch (m_iNcube) {
            case 2:
                for (int i = 0; i < GetKary(1); i++)
                    for (int j = 0; j < GetKary(2); j++) {
                        if (m_pNodes2D[i][j]->GetIdentifier() == identifier)
                            return m_pNodes2D[i][j];
                    }

                break;

            case 3:
                for (int i = 0; i < GetKary(1); i++)
                    for (int j = 0; j < GetKary(2); j++)
                        for (int k = 0; k < GetKary(3); k++) {
                            if (m_pNodes3D[i][j][k]->GetNetPort() == identifier)
                                return m_pNodes3D[i][j][k];

                            if ((m_pNodes3D[i][j][k]->GetNetPort2() == identifier) && m_iNodeType == switch2T)
                                return m_pNodes3D[i][j][k];
                        }

                break;

            default:
                throw new CException("CTorus: _SearchNode(): Dimension not supported");
        }

        return NULL;
    }

    int CTorus::_CalculateInternalPort(CDIRNode * current_node, CDIRNode * dest_node, const int identifier, const int destination, const int node_port) {
        int port;
        int netport;
        int netport2;
        int swid, swport;

        switch (m_iNodeType) {
            case switchSTD:
                port = node_port;
                break;

            case switch2T:
                if (current_node->GetID() == dest_node->GetID()) {
                    netport = current_node->GetNetPort();
                    netport2 = current_node->GetNetPort2();

                    if (identifier == netport && destination == netport) {
                        port = m_iINicPort;
                    } else if (identifier == netport && destination == netport2) {
                        port = m_iIIntraPort;
                    } else if (identifier == netport2 && destination == netport2) {
                        port = m_iINicPort;
                    } else if (identifier == netport2 && destination == netport) {
                        port = m_iIIntraPort;
                    } else {
                        // translate the external port into internal port
                        // HACK: the function calculates the port for identifier, not for the sibling card.
                        swid = current_node->GetSwitchID(node_port);
                        swport = current_node->GetSwitchPort(node_port);

                        if (identifier == swid) {
                            port = swport;
                        } else {
                            port = m_iIIntraPort;
                        }
                    }
                } else if (current_node->GetID() != current_node->GetNetPort2()) {
                    // translate the external port into internal port
                    // HACK: the function calculates the port for identifier, not for the sibling card.
                    swid = current_node->GetSwitchID(node_port);
                    swport = current_node->GetSwitchPort(node_port);

                    if (identifier == swid) {
                        port = swport;
                    } else {
                        port = m_iIIntraPort;
                    }
                } else throw new CException("CTorus: _CalculateInternalPort(): Node Identifier unknown");
                break;

            default:
                throw new CException("CTorus: _CalculateInternalPort(): NodeType unknown");
        }

        return port;
    }

} // namespace topology
