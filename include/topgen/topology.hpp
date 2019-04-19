/*
 *  BaseNetwork.h
 *  Topology2
 *
 *  Created by Juan Villar on 01/12/2014.
 *  Copyright 2014 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_BASE_NETWORK_HPP
#define TOPGEN_BASE_NETWORK_HPP

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <string>
#include <topgen/node/channel_type_node.hpp>
#include <topgen/node/element_type_node.hpp>
#include <topgen/node/input_adapter_type_node.hpp>
#include <topgen/node/route_node.hpp>
#include <map>
#include <topgen/bgl.hpp>


#define TOPGEN_MAX_STRING_LENGTH 128     //FIXME: Remove or 128 as minimum

namespace topgen {

    enum TopologyEnum {
        fully, router, mesh, torus, cube, min, custom1, custom2, xgft, customhu, torusND, kns, rlft, dfly, pbgl
    };
    typedef enum TopologyEnum topologyType;

    enum NetPointEnum {
        standard
    };
    typedef enum NetPointEnum netPointType;

    enum NodeTypeEnum {
        switchSTD, switch2T
    };
    typedef enum NodeTypeEnum nodeType;

    enum ChannelTypeEnum {
        nodeSTD_cube,
        nodeSTD_mesh, iaSTD_mesh,
        nodeSTD_torus, node2T_torus, inode2T_torus, iaSTD_torus, ia2T_torus_top, ia2T_torus_bottom,
        nodeSTD_min, node2T_min, inode2T_min, iaSTD_min, ia2T_min,
        nodeSTD_kns, iaSTD_kns, virtSTD_kns,
        nodeSTD_xgft, iaSTD_xgft,
        nodeSTD_customhu, iaSTD_customhu,
        nodeSTD_dfly, iaSTD_dfly
    };
    typedef enum ChannelTypeEnum channelType;

    enum ConnectionTypeEnum {
        perfectshuffle, butterfly, dfly_canonic
    };
    typedef enum ConnectionTypeEnum connectionType;

    enum RoutingAlgorithmTypeEnum {
        destro, psdestro, selfrouting, H_type, U_type, minimal
    };
    typedef enum RoutingAlgorithmTypeEnum rtAlgorithmType;

    enum MatchingEnum {
        Alpha, Bravo, Charlie, Delta, Eco, Fox, Golf, Hotel,
        India, Juliet, Kilo, Lima, Mike, November, Oscar, Papa, Quebec, Romeo,
        Sierra, Tango, Union, Victor, Whisky, Xray, Yanky, Zulu
    };
    typedef enum MatchingEnum matchingType;

    class topology {
    public:
        topology();

        virtual ~topology() {
        };

        virtual void RunAll() = 0;
        virtual void BuildNetwork() = 0;
        virtual void BuildInterconnection() = 0;
        virtual void BuildChannels() = 0;
        virtual void BuildRoutes() = 0;

        virtual void LoadNetwork() = 0;
        virtual void LoadRoutes() = 0;

        virtual void Vectorize();
        virtual void PreBuildRoutes();
        virtual void PostVectorize();

        virtual void _LoadChannel(int selector, const char * type, int id, int upNode, int upPort, int downNode, int downPort);
        virtual void _LoadChannel(CChannelType_Node *channel);
        virtual void _LoadRoute(int identifier, int destination, int output);
        void _LoadIA(CInputAdapterType_Node * ia);
        void _LoadElement(CElementType_Node * element);

        // By position inside the lists
        virtual int NumberOfDestinations();
        virtual int NumberOfSwitches();
        virtual int GetElementIdentifier(int index);
        virtual int GetInputAdapterIdentifier(int index);

        // By identifier
        virtual CElementType_Node * GetElement(int identifier);
        virtual CInputAdapterType_Node * GetInputAdapter(int identifier);
        virtual int GetNumberOfPorts(int identifier);
        virtual int GetElementConnectedTo(int ia_id);

        // By name
        virtual CChannelType_Node * GetChannel(const char * name, int port);
        virtual int GetElementIdentifier(const char * name);
        virtual int GetInputAdapterIdentifier(const char * name);
        virtual int GetChannelIdentifier(const char * name);
        virtual int GetIdentifier(const char * name);

        // What is Next?
        virtual bool IsElementToIA(int node_id, int link_id, unsigned int * next_ia_id, unsigned int * next_ia_port);
        virtual bool IsElementToElement(int node_id, int link_id, unsigned int * next_node_id, unsigned int * next_node_port);
        virtual bool IsElementToAir(int node_id, int link_id, unsigned int * next_node_id, unsigned int * next_node_port);
        virtual bool IsIAToElement(int node_id, int link_id, unsigned int * next_node_id, unsigned int * next_node_port);

        // Channel information
        virtual bool IsExternalChannel(int node_id, int link_id);

        // routing information
        virtual int GetOutputport(int identifier, int destination);

        virtual int MyLog(int number, int base);

        virtual int GetElementCategories();

        long long unsigned GetMemoryOccupancy();

        // Topology information
        void printTopology();

#ifndef TOPGEN_PBGL
        // Boost Graph Library
        void build_bgl();
        void write_graphviz(const std::string gvfile);
        void run_read_graphviz(const std::string gvfile);
        void write_graphml(const std::string mlfile);
        void print_graph();
        void run_dijkstra_shortest_paths(int from, int to);
        void run_bellman_ford_shortest_paths(int from, int to);
        void run_breadth_first_search(int from);
        void run_depth_first_search(int from);
        void run_kruskal_minimum_spanning_tree(int root_tree);
        void run_prim_minimum_spanning_tree(int root_tree);
#endif

    protected:
        //storage lists
        std::list<CInputAdapterType_Node*> * m_pInputAdapters;
        std::list<CElementType_Node*> * m_pElements;
        std::list<CChannelType_Node*> * m_pChannels;
        std::list<CRoute_Node*> * m_pRoutes; /* switch_identifier x destination -> output port */

        // list size
        int m_iInputAdapters;
        int m_iElements;
        int m_iChannels;
        int m_iSwitchIdentifiers;
        int m_iNetworkDestinations;

        // lists to vectors (fast access)
        CInputAdapterType_Node* * m_pInputAdaptersVector; /* switch_identifier is the index of vector*/
        CElementType_Node* * m_pElementsVector;
        std::list<CChannelType_Node*> ** m_pChannelsArray; /* swith port is the index of the vector */
        int m_iChannelsArrayLen;

        int ** m_pRoutesArray; /* switch_identifier x destination -> output port */

        int m_iElementCategories; // number of elements categories

        // breakdown of memory occupancy in bytes
        // do calculation meanwhile items are vectorized
        // TODO complete calculation

        struct stMemoryBreakdown {
            double dInputAdapters;
            double dElements;
            double dChannels;
            double dRouters;
        } m_pMemoryBreakdown;
        
#ifndef TOPGEN_PBGL
        //Boost Graph Library
        Graph m_Graph;       
#endif

        void _freeMemory();
    };
    
} // namespace topology

#endif
