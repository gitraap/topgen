/*
 *  Custom2.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include "topgen/custom2.hpp"

namespace topgen {

    CCustom2::CCustom2() : topology() {
        m_iEndNodes = 16;
        m_iChannels = 0;
    }

    CCustom2::~CCustom2() {

    }

    void CCustom2::RunAll() {
        LoadNetwork();
        LoadRoutes();
    }

    void CCustom2::BuildNetwork() {

    }

    void CCustom2::BuildRoutes() {

    }

    void CCustom2::BuildInterconnection() {

    }

    void CCustom2::BuildChannels() {

    }

    int CCustom2::GetNumberOfSwitches() {
        return 1;
    }

    int CCustom2::GetNodes() {
        return m_iEndNodes;
    }

    void CCustom2::LoadNetwork() {
        char * name;

        // IA
        for (int i = 0; i < m_iEndNodes; i++) {
            name = new char[TOPGEN_MAX_STRING_LENGTH];
            snprintf(name, TOPGEN_MAX_STRING_LENGTH, "ia%d", i);
            CInputAdapterType_Node * new_ia = new CInputAdapterType_Node("cia", name, i, "");
            _LoadIA(new_ia);
        }

        // NodeTypes
        for (int i = 0; i < 2; i++) {
            name = new char[TOPGEN_MAX_STRING_LENGTH];
            snprintf(name, TOPGEN_MAX_STRING_LENGTH, "s%d", i);
            CElementType_Node * new_element = new CElementType_Node("cs", name, i, m_iEndNodes + 8, 0);
            m_pElements->push_back(new_element);
        }

        // internal channels
        _LoadChannel(0, "int", m_iChannels++, 0, 16, 1, 16);
        _LoadChannel(0, "int", m_iChannels++, 0, 17, 1, 17);
        _LoadChannel(0, "int", m_iChannels++, 0, 18, 1, 18);
        _LoadChannel(0, "int", m_iChannels++, 0, 19, 1, 19);
        _LoadChannel(0, "int", m_iChannels++, 0, 20, 1, 20);
        _LoadChannel(0, "int", m_iChannels++, 0, 21, 1, 21);
        _LoadChannel(0, "int", m_iChannels++, 0, 22, 1, 22);
        _LoadChannel(0, "int", m_iChannels++, 0, 23, 1, 23);

        // alpha switch
        _LoadChannel(1, "ext", m_iChannels++, 0, 0, 0, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 1, 1, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 2, 2, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 3, 3, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 4, 4, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 5, 5, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 6, 6, 0);
        _LoadChannel(1, "ext", m_iChannels++, 0, 7, 7, 0);

        // beta switch
        _LoadChannel(1, "ext", m_iChannels++, 1, 0, 8, 0);
        _LoadChannel(1, "ext", m_iChannels++, 1, 1, 9, 0);
        _LoadChannel(1, "ext", m_iChannels++, 1, 2, 10, 0);
        _LoadChannel(1, "ext", m_iChannels++, 1, 3, 11, 0);
        _LoadChannel(1, "ext", m_iChannels++, 1, 4, 12, 0);
        _LoadChannel(1, "ext", m_iChannels++, 1, 5, 13, 0);
        _LoadChannel(1, "ext", m_iChannels++, 1, 6, 14, 0);
        _LoadChannel(1, "ext", m_iChannels++, 1, 7, 15, 0);

    }

    void CCustom2::LoadRoutes() {
        // alpha switch
        _LoadRoute(0, 0, 0);
        _LoadRoute(0, 1, 1);
        _LoadRoute(0, 2, 2);
        _LoadRoute(0, 3, 3);
        _LoadRoute(0, 4, 4);
        _LoadRoute(0, 5, 5);
        _LoadRoute(0, 6, 6);
        _LoadRoute(0, 7, 7);

        _LoadRoute(0, 8, 16); // reparto
        _LoadRoute(0, 9, 17);
        _LoadRoute(0, 10, 18);
        _LoadRoute(0, 11, 19);
        _LoadRoute(0, 12, 20);
        _LoadRoute(0, 13, 21);
        _LoadRoute(0, 14, 22);
        _LoadRoute(0, 15, 23);

        // beta switch
        _LoadRoute(1, 0, 16); // reparto
        _LoadRoute(1, 1, 17);
        _LoadRoute(1, 2, 18);
        _LoadRoute(1, 3, 19);
        _LoadRoute(1, 4, 20);
        _LoadRoute(1, 5, 21);
        _LoadRoute(1, 6, 22);
        _LoadRoute(1, 7, 23);

        _LoadRoute(1, 8, 0);
        _LoadRoute(1, 9, 1);
        _LoadRoute(1, 10, 2);
        _LoadRoute(1, 11, 3);
        _LoadRoute(1, 12, 4);
        _LoadRoute(1, 13, 5);
        _LoadRoute(1, 14, 6);
        _LoadRoute(1, 15, 7);
    }
} // namespace topology
