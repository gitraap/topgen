/*
 *  Cube.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <iostream>
#include "topgen/cube.hpp"

namespace topgen {

    CCube::CCube(int dimension) : topology() {
        m_iDimension = dimension;
        m_iNodes = pow(2, m_iDimension);
        m_iWordWide = m_iDimension;
        m_iChannels = 0;
        m_iIdentifiers = 0;

        m_pNodeVector = new CDIRNode*[m_iNodes];
        for (int i = 0; i < m_iNodes; i++)
            m_pNodeVector[i] = NULL;

        m_pChannelList = new std::list<CDIRChannel*>;
    }

    CCube::~CCube() {
        delete m_pChannelList;
        delete [] m_pNodeVector;
    }

    void CCube::RunAll() {
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

    void CCube::BuildNetwork() {
        CDIRNode::ResetCounter();
        for (int i = 0; i < m_iNodes; i++) {
            m_pNodeVector[i] = new CDIRNode(m_iDimension, m_iDimension + 1, m_iIdentifiers, switchSTD);
            m_pNodeVector[i]->SetNicPort(m_iDimension);
            m_pNodeVector[i]->SetNetPort(m_iIdentifiers);
            m_iIdentifiers++;
        }
    }

    void CCube::BuildInterconnection() {
        for (int i = 0; i < m_iNodes; i++) {
            int identifier = m_pNodeVector[i]->GetIdentifier();
            boost::dynamic_bitset<> current = boost::dynamic_bitset<>(m_iWordWide, identifier);

            for (int dim = 1; dim <= m_iDimension; dim++) {
                boost::dynamic_bitset<> remote = boost::dynamic_bitset<>(m_iWordWide, identifier);
                remote[dim - 1].flip();

                //std::cout << "Current " << current << std::endl;
                //std::cout << "Remote  " << remote << " Dim " << dim - 1 << std::endl << std::endl;

                CDIRNode * remote_node = _GetNodeByIdentifier(remote.to_ulong());
                m_pNodeVector[i]->SetNext(dim, remote_node);
            }
        }
    }

    void CCube::BuildChannels() {
        for (int i = 0; i < m_iNodes; i++) {
            CDIRNode * current_node = m_pNodeVector[i];

            for (int dim = 1; dim <= m_iDimension; dim++) {
                CDIRNode * remote_node = current_node->GetNext(dim);
                if (!remote_node)
                    throw new CException("CCube: BuildChannels(): remote node is NULL");

                CDIRChannel * ch = new CDIRChannel(nodeSTD_cube,
                    m_iChannels++,
                    current_node,
                    current_node->GetNextLink(dim),
                    remote_node,
                    remote_node->GetNextLink(dim));

                m_pChannelList->push_back(ch);

                /*
                 * Hack: Prev links remain unused
                 */
            }
        }
    }

    void CCube::BuildRoutes() {

    }

    int CCube::GetNumberOfSwitches() {
        return m_iNodes;
    }

    int CCube::GetNodes() {
        return m_iNodes;
    }

    void CCube::LoadNetwork() {
        char * name;

        /*
         * IAs
         */
        for (int i = 0; i < m_iNodes; i++) {
            name = new char[TOPGEN_MAX_STRING_LENGTH];
            snprintf(name, TOPGEN_MAX_STRING_LENGTH, "ia%d", m_pNodeVector[i]->GetIdentifier());
            CInputAdapterType_Node * new_ia = new CInputAdapterType_Node("cia", name, m_pNodeVector[i]->GetIdentifier(), "");
            _LoadIA(new_ia);
        }

        /*
         * NodeTypes
         */
        for (int i = 0; i < m_iNodes; i++) {
            CElementType_Node * new_element = new CElementType_Node("cs", m_pNodeVector[i]->GetIdentifier(), m_pNodeVector[i]->GetPorts(), 0);
            m_pElements->push_back(new_element);
        }

        /*
         * Channels
         */

        // net
        std::list<CDIRChannel*>::iterator it = m_pChannelList->begin();
        while (it != m_pChannelList->end()) {
            CDIRChannel * ch = (*it);
            _LoadChannel(0, "ext", ch->m_iID, ch->m_pUpNode->GetIdentifier(), ch->m_iUpPort, ch->m_pDownNode->GetIdentifier(), ch->m_iDownPort);
            ++it;
        }


        // ia -- net
        for (int i = 0; i < m_iNodes; i++) {
            _LoadChannel(1, "ext", m_iChannels++, i, m_pNodeVector[i]->GetNicPort(), i, 0);
        }
    }

    void CCube::LoadRoutes() {
        for (int id = 0; id < m_iIdentifiers; id++) {
            for (int dest = 0; dest < m_iIdentifiers; dest++) {
                boost::dynamic_bitset<> current = boost::dynamic_bitset<>(m_iWordWide, id);
                boost::dynamic_bitset<> destination = boost::dynamic_bitset<>(m_iWordWide, dest);

                int dim = 1;
                while (dim <= m_iWordWide && current[dim - 1] == destination[dim - 1]) {
                    dim++;
                }

                if (dim > m_iWordWide) {
                    _LoadRoute(id, dest, m_pNodeVector[id]->GetNicPort());
                } else if (current[dim - 1] != destination[dim - 1]) {
                    _LoadRoute(id, dest, m_pNodeVector[id]->GetNextLink(dim));
                } else throw new CException("CCube: BuildRoutes(): Impossible to be here");
            }
        }
    }

    CDIRNode * CCube::_GetNodeByIdentifier(int identifier) {
        for (int i = 0; i < m_iNodes; i++) {
            if (m_pNodeVector[i]->GetIdentifier() == identifier)
                return m_pNodeVector[i];
        }

        return NULL;
    }

} // namespace topology
