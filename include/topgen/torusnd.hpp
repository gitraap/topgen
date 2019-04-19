/*
 *  TorusND.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_TORUSND_HPP
#define TOPGEN_TORUSND_HPP

#include <list>
#include <math.h>
#include <topgen/topology.hpp>
#include <topgen/min_node.hpp>
#include <topgen/dir_channel.hpp>
#include <topgen/exception.hpp>
#include <topgen/ia.hpp>

namespace topgen {

    class CTorusND : public topology {
    public:
        CTorusND(int n, int * karies);
        virtual ~CTorusND();

        virtual void RunAll();
        virtual void BuildNetwork();
        virtual void BuildInterconnection();
        virtual void BuildChannels();
        virtual void BuildRoutes();

        virtual void Settings(nodeType node_type);

        virtual int GetDimensions();
        virtual int GetNumberOfSwitches();
        virtual int GetNumberOfDestinations();
        virtual int GetNodes();
        virtual void SetKary(const int dimension, const int kary);
        virtual int GetKary(const int dimension);
        virtual int * GetCoordinates(const int index);
        
        virtual void LoadNetwork();
        virtual void LoadRoutes();

    private:

        CDIRNode * _SearchNode(const int identifier);
        void _GetCoordinates(const int position, int * coordinates);
        int _GetPosition(int * coordinates);

        int m_iDimensions;
        int * m_pKary;
        int m_iNodes;
        int m_iIdentifiers;
        int m_iDestinations;

        CDIRNode ** m_pNodesND;

        nodeType m_iNodeType;
        int m_iNICPort;
    };

} // namespace topology

#endif /* TOPGEN_TORUSND_HPP */
