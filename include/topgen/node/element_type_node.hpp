/*
 *  ElementType_Node.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_ELEMENT_TYPE_NODE_HPP
#define TOPGEN_ELEMENT_TYPE_NODE_HPP

#include <stdio.h>

namespace topgen {

    class CElementType_Node {
    public:
        CElementType_Node(const char * type, int identifier, int ports, int category);
        CElementType_Node(const char * type, const char * name, int identifier, int ports, int category);
        CElementType_Node(const char * type, const char * name, int identifier, bool laststage, int ports, int category, CElementType_Node * template_node);
        virtual ~CElementType_Node();

        void WriteTuple(int length, int * coordinates);
        void WriteTuple(int dimension, int * position, int length, int * coordinates);
        int * NextElement(int dimension);
        int GetLevel();
        int GetDimension();
        int GetCategory();

        char * m_sType;
        char * m_sName;
        int m_iIdentifier;
        char m_iPorts;
        char m_iCategory;

        // KNS & XGFT
        // XGFT considers the switches are inside dimension 0, position {level, node}
        char m_iDimension;
        char m_iLength;
        bool m_bLastStage;
        int * m_iPosition;
        int * m_pCoordinates;
        CElementType_Node * m_pTemplateNode;
    };

} // namespace topology

#endif /* TOPGEN_ELEMENT_TYPE_NODE_HPP */
