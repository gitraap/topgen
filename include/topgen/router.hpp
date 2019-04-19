/*
 *  Router.h
 *  Topology2
 *
 *  Created by Juan Villar on 26/05/2015.
 *  Copyright 2015 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_ROUTER_HPP
#define TOPGEN_ROUTER_HPP

#include <list>
#include <topgen/topology.hpp>
#include <topgen/dir_node.hpp>
#include <topgen/dir_channel.hpp>
#include <topgen/exception.hpp>

namespace topgen {

    class CRouter : public topology {
    public:
        CRouter(int radix);
        virtual ~CRouter();

        virtual void RunAll();
        virtual void BuildNetwork();
        virtual void BuildRoutes();
        virtual void BuildInterconnection();
        virtual void BuildChannels();

        virtual void LoadNetwork();
        virtual void LoadRoutes();

        virtual int GetNumberOfSwitches();
        virtual int GetNodes();
        virtual int * GetCoordinates(int idx);

    private:
        int m_iNodes;
        int m_iChannels;
    };

} // namespace topology

#endif /* TOPGEN_ROUTER_HPP */
