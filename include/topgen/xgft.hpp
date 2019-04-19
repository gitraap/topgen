/*
 *  XGFT.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_XGFT_HPP
#define TOPGEN_XGFT_HPP

#include <stdlib.h>
#include <math.h>
#include <list>
#include <topgen/topology.hpp>
#include <topgen/xgft_node.hpp>
#include <topgen/xgft_channel.hpp>
#include <topgen/exception.hpp>


namespace topgen {

    class CXGFTNode;
    class CXGFTChannel;

    class CXGFT : public topology {
    public:
        CXGFT(int radix, int height, int * children, int * parents, bool verbose);
        virtual ~CXGFT();

        virtual void RunAll();
        virtual void BuildNetwork();
        virtual void BuildInterconnection();
        virtual void BuildChannels();
        virtual void BuildRoutes();

        virtual int GetNumberOfSwitches();
        virtual int GetNumberOfSwitchesInLevel(int level);
        virtual int GetNumberOfDestinations();
        virtual int GetHeight();
        virtual int GetNICPort();

        virtual void LoadNetwork();
        virtual void LoadRoutes();

        virtual void PreBuildRoutes();

    private:
        CXGFTNode * _GetNodeByTuple(int level, int * tuple, double * randoms);
        CXGFTNode * _GetNeighborAtPort(int port);

        int m_iRadix;
        int m_iHeight; // h in XGFT terminology
        int m_iLevels;
        int * m_pChildren; // M's in XGFT terminology (starts at 1)
        int * m_pParents; // W's in XGFT terminology (starts at 1)
        int * m_pNodesPerLevel;
        int m_iNICPort;

        int m_iChannels;
        int m_iDestinations;
        int m_iIdentifiers;

        CXGFTNode *** m_pNodes;
        std::list<CXGFTChannel*> * m_pChannelList;

        // The comparison of tuples is time consuming. We determine an unique number
        // based on the sum of random numbers corresponding to their components,
        // so we simply compare tuples with it
        double * m_pRandoms;
    };

} // namespace topology

#endif
