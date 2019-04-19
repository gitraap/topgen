/*
 *  DIRNode.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <stdlib.h>
#include <string.h>
#include "topgen/dir_node.hpp"
#include "topgen/exception.hpp"
#include "topgen/ia.hpp"
#include "topgen.hpp"

namespace topgen {

    CDIRNode::CDIRNode(const int dimensions, const int ports, const int identifier, const nodeType node_type) {
        m_sName = new char[TOPGEN_MAX_STRING_LENGTH];
        snprintf(m_sName, TOPGEN_MAX_STRING_LENGTH, "s%d", identifier);

        m_iDimensions = dimensions;
        m_iPorts = ports;

        m_iIdentifier = identifier;
        m_iID = g_iCounter++;
        m_iAssignedPort = 0;

        m_iNodeType = node_type;
        switch (m_iNodeType) {
            case switchSTD:
                m_iIAs = 1;
                break;

            case switch2T:
                m_iIAs = 2;
                break;

            default:
                throw new CException("CDIRNode: CDIRNode(): NodeType unknown");
        }

        m_pIA = new CIA*[m_iIAs];
        for (int i = 0; i < m_iIAs; i++)
            m_pIA[i] = new CIA(m_iIdentifier + i);

        m_pNeighbours = new CDIRNode*[m_iPorts];
        for (int i = 0; i < m_iPorts; i++)
            m_pNeighbours[i] = NULL;

        m_pPosition = new int[m_iDimensions];
        for (int i = 0; i < m_iDimensions; i++)
            m_pPosition[i] = -1;

        m_pNextLinks = new int[m_iDimensions];
        m_pPrevLinks = new int[m_iDimensions];
        for (int i = 0; i < m_iDimensions; i++) {
            m_pNextLinks[i] = -1;
            m_pPrevLinks[i] = -1;
        }
        m_pInternalSwitchID = NULL;
        m_pInternalSwitchPort = NULL;
    }

    CDIRNode::~CDIRNode() {
        delete [] m_sName;
        for (int i = 0; i < m_iIAs; i++)
            delete m_pIA[i];
        delete [] m_pIA;
        delete [] m_pNeighbours;
        delete [] m_pPosition;
        delete [] m_pPrevLinks;
        delete [] m_pNextLinks;

        if (m_pInternalSwitchID) {
                delete [] m_pInternalSwitchID;
                delete [] m_pInternalSwitchPort;
        }
    }

    int CDIRNode::g_iCounter = 0; /* Initialize static counter */

    void CDIRNode::SetupInternalConfiguration() {
        m_pInternalSwitchID = new int[m_iPorts];
        m_pInternalSwitchPort = new int[m_iPorts];
        for (int i = 0; i < m_iPorts; i++) {
            m_pInternalSwitchID[i] = -1;
            m_pInternalSwitchPort[i] = -1;
        }

        // dimension X
        m_pInternalSwitchID[GetNextLink(1)] = m_iIdentifier + TOPGEN_BOTTOM; // X+
        m_pInternalSwitchPort[GetNextLink(1)] = 0;

        m_pInternalSwitchID[GetPrevLink(1)] = m_iIdentifier + TOPGEN_BOTTOM; // X-
        m_pInternalSwitchPort[GetPrevLink(1)] = 1;

        // dimension Y
        m_pInternalSwitchID[GetNextLink(2)] = m_iIdentifier + TOPGEN_BOTTOM; // Y+
        m_pInternalSwitchPort[GetNextLink(2)] = 2;

        m_pInternalSwitchID[GetPrevLink(2)] = m_iIdentifier + TOPGEN_TOP; // Y-
        m_pInternalSwitchPort[GetPrevLink(2)] = 0;

        // dimension Z
        m_pInternalSwitchID[GetNextLink(3)] = m_iIdentifier + TOPGEN_TOP; // Z+
        m_pInternalSwitchPort[GetNextLink(3)] = 1;

        m_pInternalSwitchID[GetPrevLink(3)] = m_iIdentifier + TOPGEN_TOP; // z-
        m_pInternalSwitchPort[GetPrevLink(3)] = 2;
    }

    int CDIRNode::GetSwitchPort(const int port) {
        return m_pInternalSwitchPort[port];
    }

    int CDIRNode::GetSwitchID(const int port) {
        return m_pInternalSwitchID[port];
    }

    char * CDIRNode::GetName() {
        return m_sName;
    }

    int CDIRNode::GetID() {
        return m_iID;
    }

    int CDIRNode::GetIdentifier() {
        return m_iIdentifier;
    }

    int CDIRNode::GetPorts() {
        return m_iPorts;
    }

    int CDIRNode::GetDimensions() {
        return m_iDimensions;
    }

    void CDIRNode::SetNetPort(const int number) {
        m_iNetPort = number;
    }

    int CDIRNode::GetNetPort() {
        return m_iNetPort;
    }

    void CDIRNode::SetNetPort2(const int number) {
        m_iNetPort2 = number;
    }

    int CDIRNode::GetNetPort2() {
        return m_iNetPort2;
    }

    void CDIRNode::SetNicPort(const int port) {
        m_iNicPort = port;
    }

    int CDIRNode::GetNicPort() {
        return m_iNicPort;
    }

    void CDIRNode::SetPosition(const int dimension, const int position) {
        m_pPosition[dimension - 1] = position;
    }

    int CDIRNode::GetPosition(const int dimension) {
        return m_pPosition[dimension - 1];
    }

    int * CDIRNode::GetPositionVector() {
        return m_pPosition;
    }

    void CDIRNode::SetNext(const int dimension, CDIRNode * next) {
        if (m_pNextLinks[dimension - 1] != -1)
            throw new CException("CDIRNode: SetNext(): Port already assigned");

        m_pNextLinks[dimension - 1] = m_iAssignedPort++;
        m_pNeighbours[m_pNextLinks[dimension - 1]] = next;
    }

    CDIRNode * CDIRNode::GetNext(const int dimension) {
        return m_pNeighbours[m_pNextLinks[dimension - 1]];
    }

    void CDIRNode::SetPrev(const int dimension, CDIRNode * prev) {
        if (m_pPrevLinks[dimension - 1] != -1)
            throw new CException("CDIRNode: SetNext(): Port already assigned");

        m_pPrevLinks[dimension - 1] = m_iAssignedPort++;

        m_pNeighbours[m_pPrevLinks[dimension - 1]] = prev;
    }

    CDIRNode * CDIRNode::GetPrev(const int dimension) {
        return m_pNeighbours[m_pPrevLinks[dimension - 1]];
    }

    int CDIRNode::GetNextLink(const int dimension) {
        return m_pNextLinks[dimension - 1];
    }

    int CDIRNode::GetPrevLink(const int dimension) {
        return m_pPrevLinks[dimension - 1];
    }

    void CDIRNode::Print(FILE * file, const char * type) {
        fprintf(file, "\t<declare type=\"%s\" name=\"%s%d\">\n", type, m_sName, m_iID);
        fprintf(file, "\t\t<parameter name=\"identifier\">%d</parameter>\n", m_iIdentifier);
        fprintf(file, "\t\t<parameter name=\"ports\">%d</parameter>\n", m_iPorts);
        fprintf(file, "\t</declare>\n");
    }
    
    void CDIRNode::ResetCounter() {
        g_iCounter = 0; // required when GTest launches various test in sequence
    }

} // namespace topology
