/*
 *  BGL.hpp
 *  Topology2
 *
 *  Created by Juan Villar on 01/02/2019.
 *  Copyright 2019 UCLM. All rights reserved.
 *
 */

#ifndef TOPGEN_BGL_HPP
#define TOPGEN_BGL_HPP

//#include <string>
//#include <iostream>

#include <boost/config.hpp>
#include <boost/throw_exception.hpp>

// Manual selection
//#define TOPGEN_PBGL

#ifndef TOPGEN_PBGL
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/breadth_first_search.hpp>//bfs
#include <boost/graph/depth_first_search.hpp>//dfs
#include <boost/pending/indirect_cmp.hpp>//bfs & dfs
#include <boost/range/irange.hpp>//bfs & dfs
#else

#include <boost/graph/use_mpi.hpp>//PBGL
#include <boost/graph/distributed/mpi_process_group.hpp>//PBGL
//#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>//PBGL
#include <boost/property_map/parallel/process_group.hpp>
#endif

#ifdef BOOST_NO_EXCEPTIONS

void boost::throw_exception(std::exception const& ex) {
    std::cout << ex.what() << std::endl;
    abort();
}
#endif



namespace topgen {

    struct VertexData {
        std::string name;
        std::string label;
        int id;
        std::string shape;
        std::string color;
        
        
        VertexData() { }
        VertexData(std::string _name, int _id, std::string _shape, std::string _color) :
        name(_name), label(_name), id(_id), shape(_shape), color(_color) { }
        
        template<typename Archiver>
        void serialize(Archiver& ar, const unsigned int /*version*/)
        {
            ar & name & label & id & shape & color ;
        }
        
        VertexData& operator=(const VertexData& v)
        {
            name=v.name;
            label=v.label;
            id=v.id;
            shape=v.shape;
            color=v.color;
            return *this;
        }
    };

    struct EdgeData {
        std::string name;
        std::string label;
        int id;
        double weight;
        
        EdgeData() { }
        EdgeData(std::string _name, int _id, double _weight) :
        name(_name), label(_name), id(_id), weight(_weight) { }
        
        template<typename Archiver>
        void serialize(Archiver& ar, const unsigned int /*version*/)
        {
            ar & name & label & id & weight ;
        }
        
        EdgeData& operator=(const EdgeData& e)
        {
            name=e.name;
            label=e.label;
            id=e.id;
            weight=e.weight;
            return *this;
        }
    };
    
    // define the type of graph
    
#ifndef TOPGEN_PBGL
    // setS removes duplicated out-edges, use vecS to consider duplicated ones
    typedef boost::adjacency_list< boost::setS,
                                   boost::vecS,
                                   boost::undirectedS,
                                   VertexData,
                                   EdgeData > Graph;
    
    // descriptors
    typedef boost::graph_traits<Graph>::vertex_descriptor Vertex_Descriptor;
    typedef boost::graph_traits<Graph>::edge_descriptor Edge_Descriptor;

    // iterators for accessing to vertices and edges
    typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
    typedef boost::graph_traits<Graph>::edge_iterator edge_iter;
    typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_iter;
    typedef boost::graph_traits<Graph>::out_edge_iterator out_edge_iter;
    typedef boost::graph_traits<Graph>::degree_size_type degree_t;

    // de-referencing the iterators to have the descriptors
    typedef std::pair<adjacency_iter, adjacency_iter> adjacency_vertex_range_t;
    typedef std::pair<out_edge_iter, out_edge_iter> out_edge_range_t;
    typedef std::pair<vertex_iter, vertex_iter> vertex_range_t;
    typedef std::pair<edge_iter, edge_iter> edge_range_t;
    
    // for use by Breadth First Search
    // source: https://www.boost.org/doc/libs/1_69_0/libs/graph/example/bfs-example.cpp
    template <typename TimeMap > class bfs_time_visitor : public boost::default_bfs_visitor {
    typedef typename boost::property_traits < TimeMap >::value_type T;
    public:

        bfs_time_visitor(TimeMap tmap, T & t)
        : m_timemap(tmap), m_time(t) {
        }

        template < typename Vertex, typename Graph >
        void discover_vertex(Vertex u, const Graph & g) const {
            put(m_timemap, u, m_time++);
        }
        TimeMap m_timemap;
        T & m_time;
    };
    
    // for use by Depth First Search
    // source: https://www.boost.org/doc/libs/1_69_0/libs/graph/example/dfs-example.cpp
    template < typename TimeMap > class dfs_time_visitor : public boost::default_dfs_visitor {
    typedef typename boost::property_traits < TimeMap >::value_type T;
    public:

        dfs_time_visitor(TimeMap dmap, TimeMap fmap, T & t)
        : m_dtimemap(dmap), m_ftimemap(fmap), m_time(t) {
        }

        template < typename Vertex, typename Graph >
        void discover_vertex(Vertex u, const Graph & g) const {
            put(m_dtimemap, u, m_time++);
        }

        template < typename Vertex, typename Graph >
        void finish_vertex(Vertex u, const Graph & g) const {
            put(m_ftimemap, u, m_time++);
        }
        TimeMap m_dtimemap;
        TimeMap m_ftimemap;
        T & m_time;
    };
    
#else    
    
    /* definition of basic boost::graph properties */
    typedef boost::adjacency_list< boost::setS,
                                   boost::distributedS<boost::graph::distributed::mpi_process_group, boost::vecS>,
                                   boost::undirectedS,
                                   VertexData,
                                   EdgeData
                                 > PGraph;
    
//        typedef boost::adjacency_list< boost::setS,
//                                   boost::distributedS<boost::graph::distributed::mpi_process_group, boost::vecS>,
//                                   boost::undirectedS,
//                                   boost::property<boost::vertex_name_t, std::string,
//                                    boost::property<boost::vertex_index_t, int,
//                                     boost::property<boost::vertex_color_t, std::string> > >,
//                                   boost::property<boost::edge_name_t, std::string,
//                                    boost::property<boost::edge_index_t, int,
//                                     boost::property<boost::edge_weight_t, double> > >
//                                 > PGraph;
    // descriptors
    typedef boost::graph_traits<PGraph>::vertex_descriptor Vertex_Descriptor;
    typedef boost::graph_traits<PGraph>::edge_descriptor Edge_Descriptor;
    
#endif  
    
};

#endif /* TOPGEN_BGL_HPP */
