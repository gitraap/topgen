/*
 *  Mesh.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <omp.h>
#include <assert.h>
#include "topgen/mesh.hpp"

namespace topgen {

    CMesh::CMesh(const int dimensions, const int karity1, const int karity2, const int karity3) : topology() {
        m_iDimension = dimensions;

        switch (dimensions) {
            case 2:
                m_pKary = new int[2];
                m_pKary[0] = karity1;
                m_pKary[1] = karity2;
                m_iNodes = karity1 * karity2;
                break;

            case 3:
                m_pKary = new int[3];
                m_pKary[0] = karity1;
                m_pKary[1] = karity2;
                m_pKary[2] = karity3;
                m_iNodes = karity1 * karity2 * karity3;
                break;

            default:
                throw new CException("CMesh: CMesh(): dimension not supported");
        }

        m_iChannels = 0;
        m_iIdentifiers = 0;
        m_pChannelList = new std::list<CDIRChannel*>;

        switch (m_iDimension) {
            case 2:
                m_pNodes2D = new CDIRNode**[GetKary(1)];
#pragma omp parallel for
                for (int i = 0; i < GetKary(1); i++) {
                    m_pNodes2D[i] = new CDIRNode*[GetKary(2)];
                    for (int j = 0; j < GetKary(2); j++)
                        m_pNodes2D[i][j] = NULL;
                }

                break;

            case 3:
                m_pNodes3D = new CDIRNode***[GetKary(1)];
#pragma omp parallel for
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
                throw new CException("CMesh: CMesh(): dimension not supported");
        }


    }

    CMesh::~CMesh() {
        switch (m_iDimension) {
            case 2:
                for (int i = 0; i < GetKary(1); i++) {
                    for (int j = 0; j < GetKary(2); j++) {
                        assert(m_pNodes2D[i][j]);
                        delete m_pNodes2D[i][j];
                    }
                    delete [] m_pNodes2D[i];
                }
                delete [] m_pNodes2D;
                break;

            case 3:
                for (int i = 0; i < GetKary(1); i++) {
                    for (int j = 0; j < GetKary(2); j++) {
                        for (int k=0; k < GetKary(3); k++) {
                            assert(m_pNodes3D[i][j][k]);
                            delete m_pNodes3D[i][j][k];
                        }
                        delete [] m_pNodes3D[i][j];
                    }
                    delete [] m_pNodes3D[i];
                }
                delete [] m_pNodes3D;
                break;

            default: //Error at deleting
                break;
        }

        std::list<CDIRChannel*>::iterator it;
        it = m_pChannelList->begin();
        while (it != m_pChannelList->end()) {
            CDIRChannel * aux = (*it);
            delete aux;
            ++it;
        }
        delete m_pChannelList;
        delete [] m_pKary;

        _freeMemory();
    }

    int CMesh::GetNumberOfSwitches() {
        return m_iNodes;
    }

    int CMesh::GetNodes() {
        return m_iNodes;
    }

    void CMesh::SetKary(const int dimension, const int kary) {
        m_pKary[dimension - 1] = kary;
    }

    inline int CMesh::GetKary(const int dimension) {
        return m_pKary[dimension - 1];
    }

    void CMesh::RunAll() {
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

    void CMesh::BuildNetwork() {
        CDIRNode::ResetCounter();
        switch (m_iDimension) {
            case 2:
                for (int i = 0; i < GetKary(1); i++)
                    for (int j = 0; j < GetKary(2); j++) {
                        int extpts = _ExternalPorts(i, j);
                        m_pNodes2D[i][j] = new CDIRNode(m_iDimension, extpts + 1, m_iIdentifiers, switchSTD);
                        m_pNodes2D[i][j]->SetPosition(1, i);
                        m_pNodes2D[i][j]->SetPosition(2, j);
                        m_pNodes2D[i][j]->SetNicPort(extpts);
                        m_pNodes2D[i][j]->SetNetPort(m_iIdentifiers);
                        m_iIdentifiers++;
                    }

                break;

            case 3:
                for (int i = 0; i < GetKary(1); i++)
                    for (int j = 0; j < GetKary(2); j++)
                        for (int k = 0; k < GetKary(3); k++) {
                            int extpts = _ExternalPorts(i, j, k);
                            m_pNodes3D[i][j][k] = new CDIRNode(m_iDimension, extpts + 1, m_iIdentifiers, switchSTD);
                            m_pNodes3D[i][j][k]->SetPosition(1, i);
                            m_pNodes3D[i][j][k]->SetPosition(2, j);
                            m_pNodes3D[i][j][k]->SetPosition(3, k);
                            m_pNodes3D[i][j][k]->SetNicPort(extpts);
                            m_pNodes3D[i][j][k]->SetNetPort(m_iIdentifiers);
                            m_iIdentifiers++;
                        }

                break;

            default:
                throw new CException("CMesh: BuildTopology(): dimension not supported");
        }
    }

    void CMesh::BuildRoutes() {

    }

    void CMesh::BuildInterconnection() {
        int i, j, k;

        switch (m_iDimension) {
            case 2:
                for (i = 0; i < GetKary(1); i++) {
                    for (j = 0; j < GetKary(2); j++) {
                        if (i < GetKary(1) - 1)
                            m_pNodes2D[i][j]->SetNext(1, m_pNodes2D[i + 1][j]);

                        if (0 < i)
                            m_pNodes2D[i][j]->SetPrev(1, m_pNodes2D[i - 1][j]);

                        if (j < GetKary(2) - 1)
                            m_pNodes2D[i][j]->SetNext(2, m_pNodes2D[i][j + 1]);

                        if (0 < j)
                            m_pNodes2D[i][j]->SetPrev(2, m_pNodes2D[i][j - 1]);
                    }
                }
                break;

            case 3:
                for (i = 0; i < GetKary(1); i++) {
                    for (j = 0; j < GetKary(2); j++) {
                        for (k = 0; k < GetKary(3); k++) {
                            if (i < GetKary(1) - 1)
                                m_pNodes3D[i][j][k]->SetNext(1, m_pNodes3D[i + 1][j][k]);

                            if (0 < i)
                                m_pNodes3D[i][j][k]->SetPrev(1, m_pNodes3D[i - 1][j][k]);

                            if (j < GetKary(2) - 1)
                                m_pNodes3D[i][j][k]->SetNext(2, m_pNodes3D[i][j + 1][k]);

                            if (0 < j)
                                m_pNodes3D[i][j][k]->SetPrev(2, m_pNodes3D[i][j - 1][k]);

                            if (k < GetKary(3) - 1)
                                m_pNodes3D[i][j][k]->SetNext(3, m_pNodes3D[i][j][k + 1]);

                            if (0 < k)
                                m_pNodes3D[i][j][k]->SetPrev(3, m_pNodes3D[i][j][k - 1]);
                        }
                    }
                }
                break;

            default:
                throw new CException("CMesh: BuildInterconnection(): dimension not supported");
        }
    }

    void CMesh::BuildChannels() {
        switch (m_iDimension) {
            case 2:
                for (int i = 0; i < GetKary(1); i++) {
                    for (int j = 0; j < GetKary(2); j++) {
                        // mesh
                        if (i < GetKary(1) - 1) {
                            CDIRChannel * ch1 = new CDIRChannel(nodeSTD_mesh,
                                m_iChannels++,
                                m_pNodes2D[i][j],
                                m_pNodes2D[i][j]->GetNextLink(1),
                                m_pNodes2D[i + 1][j],
                                m_pNodes2D[i + 1][j]->GetPrevLink(1));

                            m_pChannelList->push_back(ch1);
                        }

                        if (j < GetKary(2) - 1) {
                            CDIRChannel * ch2 = new CDIRChannel(nodeSTD_mesh,
                                m_iChannels++,
                                m_pNodes2D[i][j],
                                m_pNodes2D[i][j]->GetNextLink(2),
                                m_pNodes2D[i][j + 1],
                                m_pNodes2D[i][j + 1]->GetPrevLink(2));

                            m_pChannelList->push_back(ch2);

                        }
                    }
                }

                // interface
                for (int i = 0; i < GetKary(1); i++) {
                    for (int j = 0; j < GetKary(2); j++) {
                        CDIRChannel * ch3 = new CDIRChannel(iaSTD_mesh,
                            m_iChannels++,
                            m_pNodes2D[i][j],
                            m_pNodes2D[i][j]->GetNicPort(),
                            NULL,
                            -1);

                        m_pChannelList->push_back(ch3);
                    }
                }
                break;

            case 3:
                for (int i = 0; i < GetKary(1); i++) {
                    for (int j = 0; j < GetKary(2); j++) {
                        for (int k = 0; k < GetKary(3); k++) {
                            // mesh
                            if (i < GetKary(1) - 1) {
                                CDIRChannel * ch1 = new CDIRChannel(nodeSTD_mesh,
                                    m_iChannels++,
                                    m_pNodes3D[i][j][k],
                                    m_pNodes3D[i][j][k]->GetNextLink(1),
                                    m_pNodes3D[i + 1][j][k],
                                    m_pNodes3D[i + 1][j][k]->GetPrevLink(1));

                                m_pChannelList->push_back(ch1);
                            }

                            if (j < GetKary(2) - 1) {
                                CDIRChannel * ch2 = new CDIRChannel(nodeSTD_mesh,
                                    m_iChannels++,
                                    m_pNodes3D[i][j][k],
                                    m_pNodes3D[i][j][k]->GetNextLink(2),
                                    m_pNodes3D[i][j + 1][k],
                                    m_pNodes3D[i][j + 1][k]->GetPrevLink(2));

                                m_pChannelList->push_back(ch2);

                            }

                            if (k < GetKary(3) - 1) {
                                CDIRChannel * ch3 = new CDIRChannel(nodeSTD_mesh,
                                    m_iChannels++,
                                    m_pNodes3D[i][j][k],
                                    m_pNodes3D[i][j][k]->GetNextLink(3),
                                    m_pNodes3D[i][j][k + 1],
                                    m_pNodes3D[i][j][k + 1]->GetPrevLink(3));

                                m_pChannelList->push_back(ch3);

                            }

                        }
                    }
                }

                // interface
                for (int i = 0; i < GetKary(1); i++) {
                    for (int j = 0; j < GetKary(2); j++) {
                        for (int k = 0; k < GetKary(3); k++) {
                            CDIRChannel * ch4 = new CDIRChannel(iaSTD_mesh,
                                m_iChannels++,
                                m_pNodes3D[i][j][k],
                                m_pNodes3D[i][j][k]->GetNicPort(),
                                NULL,
                                -1);

                            m_pChannelList->push_back(ch4);
                        }
                    }
                }
                break;

            default:
                throw new CException("CMesh: PrintTopology(): Dimension not supported");
        }

    }

    void CMesh::LoadNetwork() {
        CInputAdapterType_Node * new_ia;
        CElementType_Node * new_element;
        char name[TOPGEN_MAX_STRING_LENGTH];

        /*
         * IAs
         */
        switch (m_iDimension) {
            case 2:
                for (int i = 0; i < GetKary(1); i++)
                    for (int j = 0; j < GetKary(2); j++) {
                        snprintf(name, TOPGEN_MAX_STRING_LENGTH, "ia%d", m_pNodes2D[i][j]->GetID());
                        new_ia = new CInputAdapterType_Node("cia", name, m_pNodes2D[i][j]->GetID(), "");
                        _LoadIA(new_ia);
                    }

                break;

            case 3:
                for (int i = 0; i < GetKary(1); i++)
                    for (int j = 0; j < GetKary(2); j++)
                        for (int k = 0; k < GetKary(3); k++) {
                            snprintf(name, TOPGEN_MAX_STRING_LENGTH, "ia%d", m_pNodes3D[i][j][k]->GetID());
                            new_ia = new CInputAdapterType_Node("cia", name, m_pNodes3D[i][j][k]->GetID(), "");
                            _LoadIA(new_ia);
                        }
                break;

            default:
                throw new CException("CMesh: PrintTopology(): Dimension not supported");
        }

        /*
         * NodeTypes
         */
        switch (m_iDimension) {
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
                            new_element = new CElementType_Node("cs", m_pNodes3D[i][j][k]->GetIdentifier(), m_pNodes3D[i][j][k]->GetPorts(), 0);
                            m_pElements->push_back(new_element);
                        }
                break;

            default:
                throw new CException("CMesh: PrintTopology(): Dimension not supported");
        }

        /*
         * Channels
         */
        std::list<CDIRChannel*>::iterator it = m_pChannelList->begin();
        while (it != m_pChannelList->end()) {
            _LoadChannel((*it)->Create());
            ++it;
        }
    }

    void CMesh::LoadRoutes() {
        CDIRNode * current_node;
        CDIRNode * dest_node;
        int current_i, current_j, current_k;
        int dest_i, dest_j, dest_k;

        for (int id = 0; id < m_iIdentifiers; id++) {
            for (int dest = 0; dest < m_iIdentifiers; dest++) {
                switch (m_iDimension) {
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

                        // this is XY routing
                        if (current_i == dest_i) {
                            if (current_j == dest_j) {
                                _LoadRoute(id, dest, current_node->GetNicPort());
                            } else if (current_j < dest_j) {
                                _LoadRoute(id, dest, current_node->GetNextLink(2));
                            } else {
                                _LoadRoute(id, dest, current_node->GetPrevLink(2));
                            }
                        } else if (current_i < dest_i) {
                            _LoadRoute(id, dest, current_node->GetNextLink(1));
                        } else if (current_i > dest_i) {
                            _LoadRoute(id, dest, current_node->GetPrevLink(1));
                        } else throw new CException("CMesh: Illegal link");
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
                    
                        // this is XY routing
                        if (current_i == dest_i) {
                            if (current_j == dest_j) {
                                if (current_k == dest_k) {
                                    _LoadRoute(id, dest, current_node->GetNicPort());
                                } else if (current_k < dest_k) {
                                    _LoadRoute(id, dest, current_node->GetNextLink(3));
                                } else if (current_k > dest_k) {
                                    _LoadRoute(id, dest, current_node->GetPrevLink(3));
                                } else throw new CException("CMesh: Illegal link (index k)");
                            } else if (current_j < dest_j) {
                                _LoadRoute(id, dest, current_node->GetNextLink(2));
                            } else if (current_j > dest_j) {
                                _LoadRoute(id, dest, current_node->GetPrevLink(2));
                            } else throw new CException("CMesh: Illegal link (index j)");
                        } else if (current_i < dest_i) {
                            //m_pRoutes->push_back(new CRoute_Node(id,dest,1,current_node->GetNextLink(1)));
                            _LoadRoute(id, dest, current_node->GetNextLink(1));
                        } else if (current_i > dest_i) {
                            _LoadRoute(id, dest, current_node->GetPrevLink(1));
                        } else throw new CException("CMesh: Illegal link(index i) ");
                        break;

                    default:
                        throw new CException("CMesh: Dimension not supported");
                }
            }
        }
    }

    int CMesh::_ExternalPorts(const int i, const int j) {
        int a = (i == 0 || i == GetKary(1) - 1) ? 1 : 2;
        int b = (j == 0 || j == GetKary(2) - 1) ? 1 : 2;
        return a + b;
    }

    int CMesh::_ExternalPorts(const int i, const int j, const int k) {
        int a = (i == 0 || i == GetKary(1) - 1) ? 1 : 2;
        int b = (j == 0 || j == GetKary(2) - 1) ? 1 : 2;
        int c = (k == 0 || k == GetKary(3) - 1) ? 1 : 2;
        return a + b + c;
    }

    CDIRNode * CMesh::_SearchNode(const int identifier) {
        switch (m_iDimension) {
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
                            if (m_pNodes3D[i][j][k]->GetIdentifier() == identifier)
                                return m_pNodes3D[i][j][k];
                        }

                break;

            default:
                throw new CException("CMesh: _SearchNode(): Dimension not supported");
        }

        return NULL;
    }

} // namespace topology
