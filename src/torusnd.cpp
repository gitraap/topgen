/*
 *  TorusND.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <stdlib.h>
#include "topgen/torusnd.hpp"

namespace topgen {

    CTorusND::CTorusND(int n, int * karies) : topology() {
        m_iDimensions = n;
        m_iChannels = 0;
        m_iIdentifiers = 0;
        m_iNICPort = n;

        m_pKary = new int[m_iDimensions];
        m_iNodes = 1;
        for (int i = 0; i < m_iDimensions; i++) {
            m_pKary[i] = karies[i];
            m_iNodes *= karies[i];
        }
        m_iDestinations = m_iNodes;
    }

    CTorusND::~CTorusND() {
        for (int index = 0; index < m_iNodes; index++)
            delete m_pNodesND[index];
        delete [] m_pNodesND;
        delete [] m_pKary;
        _freeMemory();
    }

    void CTorusND::RunAll() {
        BuildNetwork();
        BuildInterconnection();
        BuildChannels();
        LoadNetwork();
        Vectorize();
        PreBuildRoutes();
        BuildRoutes();
        LoadRoutes();
    }

    void CTorusND::BuildNetwork() {
        //initialization
        CDIRNode::ResetCounter();
        m_pNodesND = new CDIRNode*[m_iNodes];

        // building network nodes
        int * coordinates = new int[m_iDimensions];
        for (int index = 0; index < m_iNodes; index++) {
            // first, identify the position of every node
            int pointer = index;
            for (int dim = 0; dim < m_iDimensions; dim++) {
                coordinates[dim] = pointer % m_pKary[dim];
                pointer = pointer / m_pKary[dim];
            }

            // second, we appropriately connect every node
            m_pNodesND[index] = new CDIRNode(m_iDimensions, m_iNICPort, m_iIdentifiers, switchSTD);
            for (int dim = 0; dim < m_iDimensions; dim++) {
                m_pNodesND[index]->SetPosition(dim + 1, coordinates[dim]);
            }
            m_pNodesND[index]->SetNicPort(m_iNICPort);
            m_pNodesND[index]->SetNetPort(m_iIdentifiers);
            m_iIdentifiers += 1;
        }

        delete [] coordinates;
    }

    void CTorusND::BuildInterconnection() {
        // nothing to do here
    }

    void CTorusND::BuildChannels() {
        // nothing to do here
    }

    void CTorusND::BuildRoutes() {
        // nothing to do here
    }

    void CTorusND::Settings(nodeType node_type) {
        m_iNodeType = node_type;
    }

    int CTorusND::GetDimensions() {
        return m_iDimensions;
    }

    int CTorusND::GetNumberOfSwitches() {
        return m_iNodes;
    }

    int CTorusND::GetNumberOfDestinations() {
        return m_iDestinations;
    }

    int CTorusND::GetNodes() {
        return m_iNodes;
    }

    void CTorusND::SetKary(const int dimension, const int kary) {
        m_pKary[dimension - 1] = kary;
    }

    int CTorusND::GetKary(const int dimension) {
        return m_pKary[dimension - 1];
    }

    int * CTorusND::GetCoordinates(const int idx) {
        int * tmp = new int[m_iDimensions];
        for (int i = 0; i < m_iDimensions; i++)
            tmp[i] = m_pNodesND[idx]->GetPosition(i + 1);

        return tmp;
    }

    void CTorusND::LoadNetwork() {
        CInputAdapterType_Node * new_ia;
        CElementType_Node * new_element;
        char * name = new char[TOPGEN_MAX_STRING_LENGTH];

        // NetPoints
        for (int idx = 0; idx < m_iNodes; idx++) {
            snprintf(name, TOPGEN_MAX_STRING_LENGTH, "ia %d", m_pNodesND[idx]->GetIdentifier());
            new_ia = new CInputAdapterType_Node("ia", name, m_pNodesND[idx]->GetIdentifier(), "");
            new_ia->WriteTuple(m_pNodesND[idx]->GetDimensions(), m_pNodesND[idx]->GetPositionVector());
            m_pInputAdapters->push_back(new_ia);
        }

        // NodeTypes
        for (int idx = 0; idx < m_iNodes; idx++) {
            new_element = new CElementType_Node("sw", m_pNodesND[idx]->GetIdentifier(), m_pNodesND[idx]->GetPorts(), 0);
            new_element->WriteTuple(m_pNodesND[idx]->GetDimensions(), m_pNodesND[idx]->GetPositionVector());
            m_pElements->push_back(new_element);
        }

        // Channels
        // nothing to do here

        delete [] name;
    }

    void CTorusND::LoadRoutes() {
        // nothing to do here
    }

    CDIRNode * CTorusND::_SearchNode(const int identifier) {
        for (int idx = 0; idx < m_iNodes; idx++) {
            if (m_pNodesND[idx]->GetIdentifier() == identifier)
                return m_pNodesND[idx];
        }

        return NULL;
    }

    void CTorusND::_GetCoordinates(const int index, int * coordinates) {
        for (int dim = 1; dim <= m_iDimensions; dim++) {
            coordinates[dim - 1] = m_pNodesND[index]->GetPosition(dim);
        }
    }

    int CTorusND::_GetPosition(int * coordinates) {
        int pos = 0;

        for (int dim = m_iDimensions; dim > 0; dim--) {
            pos += coordinates[dim - 1] * m_pKary[dim - 1];
        }
        return pos;
    }

} // namespace topology
