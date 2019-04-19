/*
 *  Custom1.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include "topgen/custom1.hpp"

namespace topgen {

    CCustom1::CCustom1() : topology() {
        m_iEndNodes = 16;
        m_iChannels = 0;
    }

    CCustom1::~CCustom1() {

    }

    void CCustom1::RunAll() {
        LoadNetwork();
        LoadRoutes();
    }

    void CCustom1::BuildNetwork() {

    }

    void CCustom1::BuildRoutes() {

    }

    void CCustom1::BuildInterconnection() {

    }

    void CCustom1::BuildChannels() {

    }

    int CCustom1::GetNumberOfSwitches() {
        return 1;
    }

    int CCustom1::GetNodes() {
        return m_iEndNodes;
    }

    void CCustom1::LoadNetwork() {
        char * name;

        // IA
        for (int i = 0; i < m_iEndNodes; i++) {
            name = new char[TOPGEN_MAX_STRING_LENGTH];
            snprintf(name, TOPGEN_MAX_STRING_LENGTH, "ia%d", i);
            CInputAdapterType_Node * new_ia = new CInputAdapterType_Node("cia", name, i, "");
            _LoadIA(new_ia);
        }

        // NodeTypes
        for (int i = 0; i < 1; i++) {
            name = new char[TOPGEN_MAX_STRING_LENGTH];
            snprintf(name, TOPGEN_MAX_STRING_LENGTH, "s%d", i);
            CElementType_Node * new_element = new CElementType_Node("cs", name, i, 32, 0);
            m_pElements->push_back(new_element);
        }

        // one switch
        _LoadChannel(1, "ext", m_iChannels++, 0, 0, 0, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 1, 1, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 2, 2, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 3, 3, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 4, 4, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 5, 5, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 6, 6, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 7, 7, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 8, 8, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 9, 9, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 10, 10, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 11, 11, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 12, 12, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 13, 13, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 14, 14, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 15, 15, 0);

    }

    void CCustom1::LoadRoutes() {
        // alpha switch
        _LoadRoute(0, 0, 0);
        _LoadRoute(0, 1, 1);
        _LoadRoute(0, 2, 2);
        _LoadRoute(0, 3, 3);
        _LoadRoute(0, 4, 4);
        _LoadRoute(0, 5, 5);
        _LoadRoute(0, 6, 6);
        _LoadRoute(0, 7, 7);
        _LoadRoute(0, 8, 8);
        _LoadRoute(0, 9, 9);
        _LoadRoute(0, 10, 10);
        _LoadRoute(0, 11, 11);
        _LoadRoute(0, 12, 12);
        _LoadRoute(0, 13, 13);
        _LoadRoute(0, 14, 14);
        _LoadRoute(0, 15, 15);
    }
} // namespace topology
