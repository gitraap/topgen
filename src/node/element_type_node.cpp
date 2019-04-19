/*
 *  ElementType_Node.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include <stdio.h>

#include "topgen/topology.hpp"
#include "topgen/node/element_type_node.hpp"

namespace topgen {

    CElementType_Node::CElementType_Node(const char * type, int identifier, int ports, int category) {
        m_sType = new char[TOPGEN_MAX_STRING_LENGTH];
        sprintf(m_sType, "%s", type);
        m_sName = new char[TOPGEN_MAX_STRING_LENGTH];
        sprintf(m_sName, "s%d", identifier);
        m_iIdentifier = identifier;
        m_iPorts = ports;
        m_iCategory = category;

        // KNS & XGFT
        m_iDimension = -1;
        m_iPosition = NULL;
        m_iLength = 0;
        m_pCoordinates = NULL;
        m_pTemplateNode = NULL;
        m_bLastStage = false;
    }

    CElementType_Node::CElementType_Node(const char * type, const char * name, int identifier, int ports, int category) {
        m_sType = new char[TOPGEN_MAX_STRING_LENGTH];
        sprintf(m_sType, "%s", type);
        m_sName = new char[TOPGEN_MAX_STRING_LENGTH];
        sprintf(m_sName, "%s", name);
        m_iIdentifier = identifier;
        m_iPorts = ports;
        m_iCategory = category;

        // KNS & XGFT
        m_iDimension = -1;
        m_iPosition = NULL;
        m_iLength = 0;
        m_pCoordinates = NULL;
        m_pTemplateNode = NULL;
        m_bLastStage = false;
    }

    CElementType_Node::CElementType_Node(const char * type, const char * name, int identifier, bool laststage, int ports, int category, CElementType_Node * template_node) {
        m_sType = new char[TOPGEN_MAX_STRING_LENGTH];
        sprintf(m_sType, "%s", type);
        m_sName = new char[TOPGEN_MAX_STRING_LENGTH];
        sprintf(m_sName, "%s", name);
        m_iIdentifier = identifier;
        m_iPorts = ports;
        m_iCategory = category;

        // KNS & XGFT
        m_iDimension = -1;
        m_iPosition = NULL;
        m_iLength = 0;
        m_pCoordinates = NULL;
        m_bLastStage = laststage;
        m_pTemplateNode = template_node;
    }

    CElementType_Node::~CElementType_Node() {
        delete [] m_sType;
        delete [] m_sName;
        if (m_iPosition != NULL)
            delete [] m_iPosition;
        if (m_pCoordinates != NULL)
            delete [] m_pCoordinates;
    }

    void CElementType_Node::WriteTuple(int length, int * coordinates) {
        m_iDimension = -1;
        m_iPosition = NULL;
        m_iLength = length;
        m_pCoordinates = new int[length];
        for (int i = 0; i < length; i++)
            m_pCoordinates[i] = coordinates[i];
    }

    void CElementType_Node::WriteTuple(int dimension, int * position, int length, int * coordinates) {
        m_iDimension = dimension;

        m_iLength = length;
        m_pCoordinates = new int[length];
        m_iPosition = new int[length];
        for (int i = 0; i < length; i++)
            m_iPosition[i] = position[i];
        for (int i = 0; i < 2; i++)
            m_pCoordinates[i] = coordinates[i];
    }

    int * CElementType_Node::NextElement(int dimension) {
        //duplicate the coordinates of this node
        int * next = new int[m_iLength];
        for (int i = 0; i < m_iLength; i++)
            next[i] = m_pCoordinates[i];

        // increment the coordinate of the dimension provided
        next[dimension]++;
        return next;
    }

    int CElementType_Node::GetLevel() {
        return m_iPosition[0]; // XGFT level
    }

    int CElementType_Node::GetDimension() {
        return m_iDimension;
    }

    int CElementType_Node::GetCategory() {
        return m_iCategory;
    }
} // namespace topology
