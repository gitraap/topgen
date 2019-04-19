/*
 *  Fully.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#include "topgen/fully.hpp"

#define FULLY_MAX_SIZE 7
#define FULLY_MIN_SIZE 2

namespace topgen {

    CFully::CFully(int nodes) : topology() {
        if (nodes < FULLY_MIN_SIZE || FULLY_MAX_SIZE < nodes)
            throw new CException("CFully: CFully(): size not supported");

        m_iNodes = nodes;
        m_iChannels = 0;
    }

    CFully::~CFully() {
        _freeMemory();
    }

    void CFully::RunAll() {
        LoadNetwork();
        LoadRoutes();
    }

    void CFully::BuildNetwork() {

    }

    void CFully::BuildRoutes() {

    }

    void CFully::BuildInterconnection() {

    }

    void CFully::BuildChannels() {

    }

    int CFully::GetNumberOfSwitches() {
        return m_iNodes;
    }

    int CFully::GetNodes() {
        return m_iNodes;
    }

    void CFully::LoadNetwork() {
        char * name = new char[TOPGEN_MAX_STRING_LENGTH];

        // IA
        for (int i = 0; i < m_iNodes; i++) {
            sprintf(name, "ia%d", i);
            CInputAdapterType_Node * new_ia = new CInputAdapterType_Node("cia", name, i, "");
            _LoadIA(new_ia);
        }

        // NodeTypes
        for (int i = 0; i < m_iNodes; i++) {
            sprintf(name, "s%d", i);
            CElementType_Node * new_element = new CElementType_Node("cs", name, i, m_iNodes, 0);
            m_pElements->push_back(new_element);
        }

        // Channels
        switch (m_iNodes) {
            case 2:
                _LoadChannel(0, "ext", m_iChannels++, 1, 1, 0, 1);
                break;

            case 3:
                _LoadChannel(0, "ext", m_iChannels++, 2, 2, 0, 1);
                _LoadChannel(0, "ext", m_iChannels++, 1, 2, 0, 2);

                _LoadChannel(0, "ext", m_iChannels++, 2, 1, 1, 1);

                break;

            case 4:
                _LoadChannel(0, "ext", m_iChannels++, 3, 3, 0, 1);
                _LoadChannel(0, "ext", m_iChannels++, 2, 3, 0, 2);
                _LoadChannel(0, "ext", m_iChannels++, 1, 3, 0, 3);

                _LoadChannel(0, "ext", m_iChannels++, 3, 2, 1, 1);
                _LoadChannel(0, "ext", m_iChannels++, 2, 2, 1, 2);

                _LoadChannel(0, "ext", m_iChannels++, 3, 1, 2, 1);

                break;

            case 5:
                _LoadChannel(0, "ext", m_iChannels++, 4, 4, 0, 1);
                _LoadChannel(0, "ext", m_iChannels++, 3, 4, 0, 2);
                _LoadChannel(0, "ext", m_iChannels++, 2, 4, 0, 3);
                _LoadChannel(0, "ext", m_iChannels++, 1, 4, 0, 4);

                _LoadChannel(0, "ext", m_iChannels++, 4, 3, 1, 1);
                _LoadChannel(0, "ext", m_iChannels++, 3, 3, 1, 2);
                _LoadChannel(0, "ext", m_iChannels++, 2, 3, 1, 3);

                _LoadChannel(0, "ext", m_iChannels++, 4, 2, 2, 1);
                _LoadChannel(0, "ext", m_iChannels++, 3, 2, 2, 2);

                _LoadChannel(0, "ext", m_iChannels++, 4, 1, 3, 1);

                break;

            case 6:
                _LoadChannel(0, "ext", m_iChannels++, 5, 5, 0, 1);
                _LoadChannel(0, "ext", m_iChannels++, 4, 5, 0, 2);
                _LoadChannel(0, "ext", m_iChannels++, 3, 5, 0, 3);
                _LoadChannel(0, "ext", m_iChannels++, 2, 5, 0, 4);
                _LoadChannel(0, "ext", m_iChannels++, 1, 5, 0, 5);

                _LoadChannel(0, "ext", m_iChannels++, 5, 4, 1, 1);
                _LoadChannel(0, "ext", m_iChannels++, 4, 4, 1, 2);
                _LoadChannel(0, "ext", m_iChannels++, 3, 4, 1, 3);
                _LoadChannel(0, "ext", m_iChannels++, 2, 4, 1, 4);

                _LoadChannel(0, "ext", m_iChannels++, 5, 3, 2, 1);
                _LoadChannel(0, "ext", m_iChannels++, 4, 3, 2, 2);
                _LoadChannel(0, "ext", m_iChannels++, 3, 3, 2, 3);

                _LoadChannel(0, "ext", m_iChannels++, 5, 2, 3, 1);
                _LoadChannel(0, "ext", m_iChannels++, 4, 2, 3, 2);

                _LoadChannel(0, "ext", m_iChannels++, 5, 1, 4, 1);

                break;

            case 7:
                _LoadChannel(0, "ext", m_iChannels++, 6, 6, 0, 1);
                _LoadChannel(0, "ext", m_iChannels++, 5, 6, 0, 2);
                _LoadChannel(0, "ext", m_iChannels++, 4, 6, 0, 3);
                _LoadChannel(0, "ext", m_iChannels++, 3, 6, 0, 4);
                _LoadChannel(0, "ext", m_iChannels++, 2, 6, 0, 5);
                _LoadChannel(0, "ext", m_iChannels++, 1, 6, 0, 6);

                _LoadChannel(0, "ext", m_iChannels++, 6, 5, 1, 1);
                _LoadChannel(0, "ext", m_iChannels++, 5, 5, 1, 2);
                _LoadChannel(0, "ext", m_iChannels++, 4, 5, 1, 3);
                _LoadChannel(0, "ext", m_iChannels++, 3, 5, 1, 4);
                _LoadChannel(0, "ext", m_iChannels++, 2, 5, 1, 5);

                _LoadChannel(0, "ext", m_iChannels++, 6, 4, 2, 1);
                _LoadChannel(0, "ext", m_iChannels++, 5, 4, 2, 2);
                _LoadChannel(0, "ext", m_iChannels++, 4, 4, 2, 3);
                _LoadChannel(0, "ext", m_iChannels++, 3, 4, 2, 4);

                _LoadChannel(0, "ext", m_iChannels++, 6, 3, 3, 1);
                _LoadChannel(0, "ext", m_iChannels++, 5, 3, 3, 2);
                _LoadChannel(0, "ext", m_iChannels++, 4, 3, 3, 3);

                _LoadChannel(0, "ext", m_iChannels++, 6, 2, 4, 1);
                _LoadChannel(0, "ext", m_iChannels++, 5, 2, 4, 2);

                _LoadChannel(0, "ext", m_iChannels++, 6, 1, 5, 1);

                break;

            default:
                throw new CException("CFully: Illegal number of ports");
        }

        // IA
        for (int i = 0; i < m_iNodes; i++) {
            _LoadChannel(1, "ext", m_iChannels++, i, 0, i, 0);
        }

        delete [] name;
    }

    void CFully::LoadRoutes() {
        switch (m_iNodes) {
            case 2:
                _LoadRoute(0, 0, 0);
                _LoadRoute(0, 1, 1);

                _LoadRoute(1, 0, 1);
                _LoadRoute(1, 1, 0);
                break;

            case 3:
                _LoadRoute(0, 0, 0);
                _LoadRoute(0, 1, 2);
                _LoadRoute(0, 2, 1);

                _LoadRoute(1, 0, 2);
                _LoadRoute(1, 1, 0);
                _LoadRoute(1, 2, 1);

                _LoadRoute(2, 0, 2);
                _LoadRoute(2, 1, 1);
                _LoadRoute(2, 2, 0);
                break;

            case 4:
                _LoadRoute(0, 0, 0);
                _LoadRoute(0, 1, 3);
                _LoadRoute(0, 2, 2);
                _LoadRoute(0, 3, 1);

                _LoadRoute(1, 0, 3);
                _LoadRoute(1, 1, 0);
                _LoadRoute(1, 2, 2);
                _LoadRoute(1, 3, 1);

                _LoadRoute(2, 0, 3);
                _LoadRoute(2, 1, 2);
                _LoadRoute(2, 2, 0);
                _LoadRoute(2, 3, 1);

                _LoadRoute(3, 0, 3);
                _LoadRoute(3, 1, 2);
                _LoadRoute(3, 2, 1);
                _LoadRoute(3, 3, 0);
                break;

            case 5:
                _LoadRoute(0, 0, 0);
                _LoadRoute(0, 1, 4);
                _LoadRoute(0, 2, 3);
                _LoadRoute(0, 3, 2);
                _LoadRoute(0, 4, 1);

                _LoadRoute(1, 0, 4);
                _LoadRoute(1, 1, 0);
                _LoadRoute(1, 2, 3);
                _LoadRoute(1, 3, 2);
                _LoadRoute(1, 4, 1);

                _LoadRoute(2, 0, 4);
                _LoadRoute(2, 1, 3);
                _LoadRoute(2, 2, 0);
                _LoadRoute(2, 3, 2);
                _LoadRoute(2, 4, 1);

                _LoadRoute(3, 0, 4);
                _LoadRoute(3, 1, 3);
                _LoadRoute(3, 2, 2);
                _LoadRoute(3, 3, 0);
                _LoadRoute(3, 4, 1);

                _LoadRoute(4, 0, 4);
                _LoadRoute(4, 1, 3);
                _LoadRoute(4, 2, 2);
                _LoadRoute(4, 3, 1);
                _LoadRoute(4, 4, 0);
                break;

            case 6:
                _LoadRoute(0, 0, 0);
                _LoadRoute(0, 1, 5);
                _LoadRoute(0, 2, 4);
                _LoadRoute(0, 3, 3);
                _LoadRoute(0, 4, 2);
                _LoadRoute(0, 5, 1);

                _LoadRoute(1, 0, 5);
                _LoadRoute(1, 1, 0);
                _LoadRoute(1, 2, 4);
                _LoadRoute(1, 3, 3);
                _LoadRoute(1, 4, 2);
                _LoadRoute(1, 5, 1);

                _LoadRoute(2, 0, 5);
                _LoadRoute(2, 1, 4);
                _LoadRoute(2, 2, 0);
                _LoadRoute(2, 3, 3);
                _LoadRoute(2, 4, 2);
                _LoadRoute(2, 5, 1);

                _LoadRoute(3, 0, 5);
                _LoadRoute(3, 1, 4);
                _LoadRoute(3, 2, 3);
                _LoadRoute(3, 3, 0);
                _LoadRoute(3, 4, 2);
                _LoadRoute(3, 5, 1);

                _LoadRoute(4, 0, 5);
                _LoadRoute(4, 1, 4);
                _LoadRoute(4, 2, 3);
                _LoadRoute(4, 3, 2);
                _LoadRoute(4, 4, 0);
                _LoadRoute(4, 5, 1);

                _LoadRoute(5, 0, 5);
                _LoadRoute(5, 1, 4);
                _LoadRoute(5, 2, 3);
                _LoadRoute(5, 3, 2);
                _LoadRoute(5, 4, 1);
                _LoadRoute(5, 5, 0);
                break;

            case 7:
                _LoadRoute(0, 0, 0);
                _LoadRoute(0, 1, 6);
                _LoadRoute(0, 2, 5);
                _LoadRoute(0, 3, 4);
                _LoadRoute(0, 4, 3);
                _LoadRoute(0, 5, 2);
                _LoadRoute(0, 6, 1);

                _LoadRoute(1, 0, 6);
                _LoadRoute(1, 1, 0);
                _LoadRoute(1, 2, 5);
                _LoadRoute(1, 3, 4);
                _LoadRoute(1, 4, 3);
                _LoadRoute(1, 5, 2);
                _LoadRoute(1, 6, 1);

                _LoadRoute(2, 0, 6);
                _LoadRoute(2, 1, 5);
                _LoadRoute(2, 2, 0);
                _LoadRoute(2, 3, 4);
                _LoadRoute(2, 4, 3);
                _LoadRoute(2, 5, 2);
                _LoadRoute(2, 6, 1);

                _LoadRoute(3, 0, 6);
                _LoadRoute(3, 1, 5);
                _LoadRoute(3, 2, 4);
                _LoadRoute(3, 3, 0);
                _LoadRoute(3, 4, 3);
                _LoadRoute(3, 5, 2);
                _LoadRoute(3, 6, 1);

                _LoadRoute(4, 0, 6);
                _LoadRoute(4, 1, 5);
                _LoadRoute(4, 2, 4);
                _LoadRoute(4, 3, 3);
                _LoadRoute(4, 4, 0);
                _LoadRoute(4, 5, 2);
                _LoadRoute(4, 6, 1);

                _LoadRoute(5, 0, 6);
                _LoadRoute(5, 1, 5);
                _LoadRoute(5, 2, 4);
                _LoadRoute(5, 3, 3);
                _LoadRoute(5, 4, 2);
                _LoadRoute(5, 5, 0);
                _LoadRoute(5, 6, 1);

                _LoadRoute(6, 0, 6);
                _LoadRoute(6, 1, 5);
                _LoadRoute(6, 2, 4);
                _LoadRoute(6, 3, 3);
                _LoadRoute(6, 4, 2);
                _LoadRoute(6, 5, 1);
                _LoadRoute(6, 6, 0);
                break;

            default:
                throw new CException("CFully: Number of nodes not supported");
        }
    }
} // namespace topology
