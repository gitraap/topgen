/*
 *  PBGL.cpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/04/2019.
 *  Copyright 2019 UCLM. All rights reserved.
 *
 */

#include "topgen/pbgl.hpp"

#ifdef TOPGEN_PBGL
#include <boost/graph/properties.hpp>
#include <boost/graph/graphviz.hpp>

#include <boost/config.hpp>//example
#include <iostream>//example
#include <fstream>//example
#include <boost/mpi.hpp>//PBGL
#include <boost/graph/use_mpi.hpp>//PBGL
#include <boost/graph/distributed/mpi_process_group.hpp>//PBGL
#include <boost/graph/copy.hpp> //graph_copy

//#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>//PBGL
#include <boost/property_map/parallel/process_group.hpp>
#include <boost/graph/graph_traits.hpp>//example
#include <boost/graph/distributed/local_subgraph.hpp>
#include <boost/graph/parallel/distribution.hpp>
#include <boost/graph/distributed/dijkstra_shortest_paths.hpp>//example
#include <boost/property_map/property_map.hpp>//example
#include <boost/graph/graph_utility.hpp> // print_graph
#endif

namespace topgen {

    CPBGL::CPBGL() : topology() {
        
    }

    CPBGL::~CPBGL() {
        _freeMemory();
    }

    void CPBGL::RunAll() {

    }

    void CPBGL::BuildNetwork() {

    }

    void CPBGL::BuildRoutes() {

    }

    void CPBGL::BuildInterconnection() {

    }

    void CPBGL::BuildChannels() {

    }

    int CPBGL::GetNumberOfSwitches() {
        return 0;
    }

    int CPBGL::GetNodes() {
        return 0;
    }

    void CPBGL::LoadNetwork() {
        
    }

    void CPBGL::LoadRoutes() {

    }
    
    void CPBGL::run_pbgl(const std::string gvfile, int from, int to)
    {
#ifdef TOPGEN_PBGL
        std::cout << "*** Inside ***" << std::endl;
        std::ifstream inf(gvfile);
//        boost::dynamic_properties dp;
//        dp.property("node_id", get(&VertexData::name, m_PGraph)); 
//        dp.property("label", get(&VertexData::name, m_PGraph));
//        dp.property("shape", get(&VertexData::shape, m_PGraph));
//        dp.property("color", get(&VertexData::color, m_PGraph));
//        dp.property("weight", get(&EdgeData::weight, m_PGraph));
//        bool status = boost::read_graphviz(inf, m_PGraph, dp/*, "node_id"*/);
//        if (!status)
//            throw new CException("run_pbgl(): cannot read graphviz file");
        
        // Uncomment to compare
//        std::string outf("pbgl_compare.gv");
//        boost::write_graphviz(outf, m_PGraph, dp);
        

        boost::mpi::communicator world;
        
        std::cout << "*** The " << world.rank() << " calls to dijkstra_example()\n";
        std::cout.flush();
        //call an example with a hardcoded distributed graph
        //if (world.rank() == 0)
            dijkstra_example();
        std::cout << "*** The " << world.rank() << " exits from dijkstra_example()\n";
        std::cout.flush();
/* para construir la solucion       
        Vertex_Descriptor s = boost::vertex(from, m_PGraph); // indicate the index of the vextex
        Vertex_Descriptor t = boost::vertex(to, m_PGraph);   // idem

        const int num_vertixes = boost::num_vertices(m_PGraph);

        // Define weights
        boost::property_map<m_PGraph, double EdgeData::*>::type weight_pmap = boost::get(&EdgeData::weight, m_PGraph);

        // Define distances
        std::vector<int> distances(num_vertixes);
        for (int v=0; v<num_vertixes; ++v) {
            distances[v] = 1; // TODO allow users to preset this
        }
        auto distances_ipmap = boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index, m_PGraph));

        // Define predecessors
        std::vector<Vertex_Descriptor> predecessors(num_vertixes);
        for (int v=0; v<num_vertixes; ++v) {
            distances[v] = v;
        }
        auto predecessors_ipmap = boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index, m_PGraph));

        // Run algorithm
        boost::dijkstra_shortest_paths(m_PGraph, s,
            boost::weight_map(weight_pmap).distance_map(distances_ipmap).predecessor_map(predecessors_ipmap));

        if (world.rank() == 0) {
            // Extract the shortest path from s to t.
            typedef std::vector<Edge_Descriptor> path_t;
            path_t path;

            Vertex_Descriptor v = t;
            for(Vertex_Descriptor u = predecessors[v]; u != v; v=u, u=predecessors[v])
            {
              std::pair<Edge_Descriptor, bool> edge_pair = boost::edge(u,v,m_PGraph);
              path.push_back( edge_pair.first );
            }

            std::cout << std::endl;
            std::cout << "Dijsktra's shortest path from s to t:" << std::endl;
            for(path_t::reverse_iterator riter = path.rbegin(); riter != path.rend(); ++riter)
            {
              Vertex_Descriptor u_tmp = boost::source(*riter, m_PGraph);
              Vertex_Descriptor v_tmp = boost::target(*riter, m_PGraph);
              Edge_Descriptor   e_tmp = boost::edge(u_tmp, v_tmp, m_PGraph).first;

              std::cout << "  " << m_PGraph[u_tmp].name << " -> " << m_PGraph[v_tmp].name << "    (weight: " << m_PGraph[e_tmp].weight << ")" << std::endl;
            }

            std::cout << std::endl;
        } else {
            std::cout << "The rank " << world.rank() << "waits" << std::endl;
        }
        std::cout.flush();
        // All processes synchronize at this point, then the graph is complete
        //boost::synchronize(m_PGraph);
*/
#else
        throw new CException("CPBGL::run_pbgl(): the PBGL support is not enabled");
#endif
    
    }
    
    void CPBGL::dijkstra_example()
    {
#ifdef TOPGEN_PBGL
        boost::mpi::communicator world;
        
        //local redefinition
        typedef boost::adjacency_list< boost::setS,
                                   boost::vecS,
                                   boost::undirectedS,
                                   VertexData,
                                   EdgeData > Graph;
        typedef boost::graph_traits<Graph>::vertex_descriptor Vertex_Descriptor1;
        typedef boost::graph_traits<Graph>::edge_descriptor Edge_Descriptor1;
        typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
        typedef boost::graph_traits<Graph>::edge_iterator edge_iter;
        
        Graph gg;
        std::ifstream inf("bgl.gv");
        boost::dynamic_properties dp;
        dp.property("node_id", boost::get(&VertexData::name, gg)); 
        dp.property("label", boost::get(&VertexData::label, gg));
        dp.property("shape", boost::get(&VertexData::shape, gg));
        dp.property("color", boost::get(&VertexData::color, gg));
        dp.property("label", boost::get(&EdgeData::label, gg));
        dp.property("weight", boost::get(&EdgeData::weight, gg));
        bool status = boost::read_graphviz(inf, gg, dp/*, "node_id"*/);
        if (!status)
            throw new CException("dijkstra_example(): cannot read from file");
        // Print out some useful information on standard output
        std::cout << "Graph:" << std::endl;
        boost::print_graph(gg, boost::get(&VertexData::name, gg));
        std::cout << "num_vertexes: " << boost::num_vertices(gg) << std::endl;
        std::cout << "num_edges: " << boost::num_edges(gg) << std::endl;
       
        /****/
        
        using namespace boost;
        using boost::graph::distributed::mpi_process_group;
  
                
        std::cout << ">>> The " << world.rank() << " reaches A\n";std::cout.flush();
            
        /* conversion at building time*/
        const int num_vertixes1 = boost::num_vertices(gg);
        std::cout << "set num_vertixes = " << num_vertixes1 << "\n";
        PGraph g(num_vertixes1); // initialize with the total number of vertices
        
        //boost::copy_graph(gg, g);
        
        //if (world.rank() == 0) {
            // Only process 0 loads the graph, which is distributed automatically
            
            /*typedef boost::property_map<Graph, boost::vertex_distance_t>::type Distance_Map;
            Distance_Map distance_id = boost::get(boost::vertex_distance, gg);
            
            vertex_iter vi, vi_end;
            for (tie(vi,vi_end) = vertices(gg); vi != vi_end; v++) {
                Vertex_Descriptor1 v = boost::owner(*vi, gg);
                //boost::local(*v) << "@" << owner(*v) << ")\n";
                
            }*/
            
            /*typedef boost::property_map<Graph, boost::vertex_index_t>::type VertexID_Map;
            VertexID_Map vertex_id = boost::get(boost::vertex_index, gg);
            
            int i = 1;
            edge_iter ei, ei_end;
            for (tie(ei, ei_end) = edges(gg); ei != ei_end; ++ei)
            {
                Vertex_Descriptor1 from = boost::source(*ei, gg);//saco del secuencial
                Vertex_Descriptor1 to   = boost::target(*ei, gg);
                //sacar numero
                std::cout << i << ":" << vertex_id[from] << " --> " << vertex_id[to] << " ";
                
                boost::add_edge(vertex(vertex_id[from], g), vertex(vertex_id[to], g), g);
                i++;
                std::cout << "stored" << "\n";
                
            }*/
            
            std::cout << "I am 0, I finish" << "\n";std::cout.flush();
        //}
        std::cout << ">>> The " << world.rank() << " reaches B\n";std::cout.flush();
        // All processes synchronize at this point, then the graph is complete
        //synchronize();
        

        

        
        std::cout << ">>> The " << world.rank() << " reaches C\n";std::cout.flush();

//        if (world.rank() == 0)
//        {
//            // Print out some useful information on standard output
//            std::cout << "Graph:" << std::endl;
//            boost::print_graph(g, boost::get(&VertexData::name, g));
//            std::cout << "num_vertexes: " << boost::num_vertices(g) << std::endl;
//            std::cout << "num_edges: " << boost::num_edges(g) << std::endl;
//        }
            
        std::cout << ">>> The " << world.rank() << " reaches D\n";std::cout.flush();
        Vertex_Descriptor s = boost::vertex(0, g); // indicate the index of the vextex
        Vertex_Descriptor t = boost::vertex(23, g);   // idem
      
        const int num_vertixes = boost::num_vertices(g);
        
        
        // Define weights
        boost::property_map<PGraph, double EdgeData::*>::type weight_pmap = boost::get(&EdgeData::weight, g);
        std::cout << ">>> The " << world.rank() << " reaches E\n";std::cout.flush();
        
        // Define distances
        std::vector<int> distances(num_vertixes);
        for (int v=0; v<num_vertixes; ++v) {
            distances[v] = 1; // TODO allow users to preset this
        }
        auto distances_ipmap = boost::make_iterator_property_map(distances.begin(), boost::get(boost::vertex_index, g));
        std::cout << ">>> The " << world.rank() << " reaches F\n";std::cout.flush();
        // Define predecessors
        std::vector<Vertex_Descriptor> predecessors(num_vertixes);
        for (int v=0; v<num_vertixes; ++v) {
            distances[v] = v;
        }
        auto predecessors_ipmap = boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index, g));
        
        std::cout << ">>> The " << world.rank() << " reaches G\n";std::cout.flush();
        // Run algorithm
        boost::dijkstra_shortest_paths(g, s,
            boost::weight_map(weight_pmap).distance_map(distances_ipmap).predecessor_map(predecessors_ipmap));
        std::cout << ">>> The " << world.rank() << " reaches H\n";std::cout.flush();
        // Extract the shortest path from s to t.


//        typedef std::vector<Edge_Descriptor> path_t;
//        path_t path;

//        Vertex_Descriptor v = t;
    //        for(Vertex_Descriptor u = predecessors[v]; u != v; v=u, u=predecessors[v])
     //       {
    //          std::pair<Edge_Descriptor, bool> edge_pair = boost::edge(u,v,g);
    //          path.push_back( edge_pair.first );
    //        }

//            std::cout << std::endl;
//            std::cout << "Dijsktra's shortest path from s to t:" << std::endl;
//            for(path_t::reverse_iterator riter = path.rbegin(); riter != path.rend(); ++riter)
//            {
//              Vertex_Descriptor u_tmp = boost::source(*riter, g);
//              Vertex_Descriptor v_tmp = boost::target(*riter, g);
//              Edge_Descriptor   e_tmp = boost::edge(u_tmp, v_tmp, g).first;
//
//              std::cout << "  " << g[u_tmp].name << " -> " << g[v_tmp].name << "    (weight: " << g[e_tmp].weight << ")" << std::endl;
//            }

        std::cout << ">>> Here, the " << world.rank() << "finished. Now waiting. ***" << std::endl;
        
        // All processes synchronize at this point, then the graph is complete
        //synchronize();
#endif
    }
} // namespace topology
