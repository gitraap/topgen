/*
 *  Router.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 26/05/2015.
 *  Copyright 2015 UCLM. All rights reserved.
 *
 */

#include "topgen/router.hpp"

#define ROUTER_MIN_SIZE 1

namespace topgen {

    CRouter::CRouter(int radix) : topology() {
        if (radix < ROUTER_MIN_SIZE)
            throw new CException("CRouter: CRouter(): radix < 1 not supported");

        m_iNodes = radix;
        m_iChannels = 0;
    }

    CRouter::~CRouter() {
        _freeMemory();
    }

    void CRouter::RunAll() {
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

    void CRouter::BuildNetwork() {
        // nothing to do here
    }

    void CRouter::BuildRoutes() {
        // nothing to do here
    }

    void CRouter::BuildInterconnection() {
        // nothing to do here
    }

    void CRouter::BuildChannels() {
        // nothing to do here
    }

    int CRouter::GetNumberOfSwitches() {
        return 1;
    }

    int CRouter::GetNodes() {
        return m_iNodes;
    }

    void CRouter::LoadNetwork() {
        char * name = new char[TOPGEN_MAX_STRING_LENGTH];

        // IA
        for (int i = 0; i < m_iNodes; i++) {
            sprintf(name, "ia%d", i);
            CInputAdapterType_Node * new_ia = new CInputAdapterType_Node("cia", name, i, "");
            _LoadIA(new_ia);
        }

        // NodeTypes (only one router)
        CElementType_Node * new_element = new CElementType_Node("cs", "s0", 0, m_iNodes, 0);
        m_pElements->push_back(new_element);

        // Channels ROUTER <-- IA
        for (int i = 0; i < m_iNodes; i++) {
            _LoadChannel(1, "ch", m_iChannels++, 0, i, i, 0);
        }

        delete [] name;
    }

    void CRouter::LoadRoutes() {
        for (int ia = 0; ia < m_iNodes; ia++) {
            _LoadRoute(0, ia, ia);
        }
    }

    int * CRouter::GetCoordinates(int idx) {
        int * tmp = new int[2]; // stage + switch
        tmp[0] = -1; // stage
        tmp[1] = -1; // switch
        return tmp;
    }
} // namespace topology
