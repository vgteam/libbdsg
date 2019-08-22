//
//  test_libbdsg.cpp
//  
// Contains tests for the data structures in libbdsg
//

#include <stdio.h>
#include <iostream>
#include <vector>
#include <cassert>

#include "odgi.hpp"
#include "packed_graph.hpp"
#include "hash_graph.hpp"
#include "packed_structs.hpp"

using namespace bdsg;
using namespace handlegraph;
using namespace std;

void test_deletable_handle_graphs() {
    
    // first batch of tests
    {
        vector<DeletableHandleGraph*> implementations;
        
        // Add implementations
        
        PackedGraph pg;
        implementations.push_back(&pg);
        
        HashGraph hg;
        implementations.push_back(&hg);
        
        ODGI og;
        implementations.push_back(&og);
        
        // And test them
        
        for (DeletableHandleGraph* implementation : implementations) {
            
            DeletableHandleGraph& graph = *implementation;
            
            assert(graph.get_node_count() == 0);
            
            handle_t h = graph.create_handle("ATG", 2);
            
            // DeletableHandleGraph has correct structure after creating a node
            {
                assert(graph.get_sequence(h) == "ATG");
                assert(graph.get_sequence(graph.flip(h)) == "CAT");
                assert(graph.get_base(h, 1) == 'T');
                assert(graph.get_base(graph.flip(h), 2) == 'T');
                assert(graph.get_subsequence(h, 1, 3) == "TG");
                assert(graph.get_subsequence(graph.flip(h), 0, 2) == "CA");
                assert(graph.get_length(h) == 3);
                assert(graph.has_node(graph.get_id(h)));
                assert(!graph.has_node(graph.get_id(h) + 1));
                
                assert(graph.get_handle(graph.get_id(h)) == h);
                assert(!graph.get_is_reverse(h));
                assert(graph.get_is_reverse(graph.flip(h)));
                
                assert(graph.get_node_count() == 1);
                assert(graph.min_node_id() == graph.get_id(h));
                assert(graph.max_node_id() == graph.get_id(h));
                
                graph.follow_edges(h, true, [](const handle_t& prev) {
                    assert(false);
                    return true;
                });
                graph.follow_edges(h, false, [](const handle_t& next) {
                    assert(false);
                    return true;
                });
            }
            
            handle_t h2 = graph.create_handle("CT", 1);
            
            // DeletableHandleGraph has correct structure after creating a node at the beginning of ID space
            {
                
                assert(graph.get_sequence(h2) == "CT");
                assert(graph.get_sequence(graph.flip(h2)) == "AG");
                assert(graph.get_base(h2, 1) == 'T');
                assert(graph.get_base(graph.flip(h2), 0) == 'A');
                assert(graph.get_subsequence(h2, 1, 10) == "T");
                assert(graph.get_subsequence(graph.flip(h2), 0, 2) == "AG");
                assert(graph.get_length(h2) == 2);
                assert(graph.has_node(graph.get_id(h2)));
                assert(!graph.has_node(max(graph.get_id(h), graph.get_id(h2)) + 1));
                
                assert(graph.get_handle(graph.get_id(h2)) == h2);
                
                assert(graph.get_node_count() == 2);
                assert(graph.min_node_id() == graph.get_id(h2));
                assert(graph.max_node_id() == graph.get_id(h));
                
                graph.follow_edges(h2, true, [](const handle_t& prev) {
                    assert(false);
                    return true;
                });
                graph.follow_edges(h2, false, [](const handle_t& next) {
                    assert(false);
                    return true;
                });
            }
            
            // creating and accessing a node at the end of ID space
            
            handle_t h3 = graph.create_handle("GAC", 4);
            
            // DeletableHandleGraph has correct structure after creating a node at the end of ID space
            {
                assert(graph.get_sequence(h3) == "GAC");
                assert(graph.get_sequence(graph.flip(h3)) == "GTC");
                assert(graph.get_base(h3, 1) == 'A');
                assert(graph.get_base(graph.flip(h3), 0) == 'G');
                assert(graph.get_subsequence(h3, 1, 1) == "A");
                assert(graph.get_subsequence(graph.flip(h3), 0, 5) == "GTC");
                assert(graph.get_length(h3) == 3);
                
                assert(graph.get_handle(graph.get_id(h3)) == h3);
                
                assert(graph.get_node_count() == 3);
                assert(graph.min_node_id() == graph.get_id(h2));
                assert(graph.max_node_id() == graph.get_id(h3));
                
                graph.follow_edges(h3, true, [](const handle_t& prev) {
                    assert(false);
                    return true;
                });
                graph.follow_edges(h3, false, [](const handle_t& next) {
                    assert(false);
                    return true;
                });
            }
            
            
            // creating and accessing in the middle of ID space
            
            handle_t h4 = graph.create_handle("T", 3);
            
            // DeletableHandleGraph has correct structure after creating a node in the middle of ID space
            {
                assert(graph.get_sequence(h4) == "T");
                assert(graph.get_sequence(graph.flip(h4)) == "A");
                assert(graph.get_length(h4) == 1);
                
                assert(graph.get_handle(graph.get_id(h4)) == h4);
                
                assert(graph.get_node_count() == 4);
                assert(graph.min_node_id() == graph.get_id(h2));
                assert(graph.max_node_id() == graph.get_id(h3));
                
                graph.follow_edges(h4, true, [](const handle_t& prev) {
                    assert(false);
                    return true;
                });
                graph.follow_edges(h4, false, [](const handle_t& next) {
                    assert(false);
                    return true;
                });
            }
            
            graph.create_edge(h, h2);
            
            bool found1 = false, found2 = false, found3 = false, found4 = false;
            int count1 = 0, count2 = 0, count3 = 0, count4 = 0;
            
            // DeletableHandleGraph has correct structure after creating an edge
            {
                graph.follow_edges(h, false, [&](const handle_t& next) {
                    if (next == h2) {
                        found1 = true;
                    }
                    count1++;
                    return true;
                });
                graph.follow_edges(h2, true, [&](const handle_t& prev) {
                    if (prev == h) {
                        found2 = true;
                    }
                    count2++;
                    return true;
                });
                graph.follow_edges(graph.flip(h), true, [&](const handle_t& prev) {
                    if (prev == graph.flip(h2)) {
                        found3 = true;
                    }
                    count3++;
                    return true;
                });
                graph.follow_edges(graph.flip(h2), false, [&](const handle_t& next) {
                    if (next == graph.flip(h)) {
                        found4 = true;
                    }
                    count4++;
                    return true;
                });
                assert(count1 == 1);
                assert(count2 == 1);
                assert(count3 == 1);
                assert(count4 == 1);
                assert(found1);
                assert(found2);
                assert(found3);
                assert(found4);
                
                count1 = count2 = count3 = count4 = 0;
                found1 = found2 = found3 = found4 = false;
            }
            
            graph.create_edge(h, graph.flip(h3));
            
            bool found5 = false, found6 = false, found7 = false, found8 = false;
            int count5 = 0, count6 = 0;
            
            // DeletableHandleGraph has correct structure after creating an edge with a traversal
            {
                
                graph.follow_edges(h, false, [&](const handle_t& next) {
                    if (next == h2) {
                        found1 = true;
                    }
                    else if (next == graph.flip(h3)) {
                        found2 = true;
                    }
                    count1++;
                    return true;
                });
                graph.follow_edges(graph.flip(h), true, [&](const handle_t& prev) {
                    if (prev == graph.flip(h2)) {
                        found3 = true;
                    }
                    else if (prev == h3) {
                        found4 = true;
                    }
                    count2++;
                    return true;
                });
                graph.follow_edges(h2, true, [&](const handle_t& prev) {
                    if (prev == h) {
                        found5 = true;
                    }
                    count3++;
                    return true;
                });
                graph.follow_edges(graph.flip(h2), false, [&](const handle_t& next) {
                    if (next == graph.flip(h)) {
                        found6 = true;
                    }
                    count4++;
                    return true;
                });
                graph.follow_edges(graph.flip(h3), true, [&](const handle_t& prev) {
                    if (prev == h) {
                        found7 = true;
                    }
                    count5++;
                    return true;
                });
                graph.follow_edges(h3, false, [&](const handle_t& next) {
                    if (next == graph.flip(h)) {
                        found8 = true;
                    }
                    count6++;
                    return true;
                });
                assert(count1 == 2);
                assert(count2 == 2);
                assert(count3 == 1);
                assert(count4 == 1);
                assert(count5 == 1);
                assert(count6 == 1);
                assert(found1);
                assert(found2);
                assert(found3);
                assert(found4);
                assert(found5);
                assert(found6);
                assert(found7);
                assert(found8);
                
                count1 = count2 = count3 = count4 = count5 = count6 = 0;
                found1 = found2 = found3 = found4 = found5 = found6 = found7 = found8 = false;
            }
            
            graph.create_edge(h4, graph.flip(h4));
            
            // DeletableHandleGraph has correct structure after creating a reversing self-loop
            {
                graph.follow_edges(h4, false, [&](const handle_t& next) {
                    if (next == graph.flip(h4)) {
                        found1 = true;
                    }
                    count1++;
                    return true;
                });
                graph.follow_edges(graph.flip(h4), true, [&](const handle_t& prev) {
                    if (prev == h4) {
                        found2 = true;
                    }
                    count2++;
                    return true;
                });
                assert(count1 == 1);
                assert(count2 == 1);
                assert(found1);
                assert(found2);
                
                count1 = count2 = 0;
                found1 = found2 = false;
            }
            
            graph.create_edge(h, graph.flip(h4));
            graph.create_edge(graph.flip(h3), h4);
            
            graph.destroy_edge(h, graph.flip(h4));
            graph.destroy_edge(graph.flip(h3), h4);
            
            // DeletableHandleGraph has correct structure after creating and deleting edges
            {
                graph.follow_edges(h, false, [&](const handle_t& next) {
                    if (next == h2) {
                        found1 = true;
                    }
                    else if (next == graph.flip(h3)) {
                        found2 = true;
                    }
                    count1++;
                    return true;
                });
                graph.follow_edges(graph.flip(h), true, [&](const handle_t& prev) {
                    if (prev == graph.flip(h2)) {
                        found3 = true;
                    }
                    else if (prev == h3) {
                        found4 = true;
                    }
                    count2++;
                    return true;
                });
                graph.follow_edges(h2, true, [&](const handle_t& prev) {
                    if (prev == h) {
                        found5 = true;
                    }
                    count3++;
                    return true;
                });
                graph.follow_edges(graph.flip(h2), false, [&](const handle_t& next) {
                    if (next == graph.flip(h)) {
                        found6 = true;
                    }
                    count4++;
                    return true;
                });
                graph.follow_edges(graph.flip(h3), true, [&](const handle_t& prev) {
                    if (prev == h) {
                        found7 = true;
                    }
                    count5++;
                    return true;
                });
                graph.follow_edges(h3, false, [&](const handle_t& next) {
                    if (next == graph.flip(h)) {
                        found8 = true;
                    }
                    count6++;
                    return true;
                });
                assert(count1 == 2);
                assert(count2 == 2);
                assert(count3 == 1);
                assert(count4 == 1);
                assert(count5 == 1);
                assert(count6 == 1);
                assert(found1);
                assert(found2);
                assert(found3);
                assert(found4);
                assert(found5);
                assert(found6);
                assert(found7);
                assert(found8);
                
                count1 = count2 = count3 = count4 = count5 = count6 = 0;
                found1 = found2 = found3 = found4 = found5 = found6 = found7 = found8 = false;
                
                graph.follow_edges(h4, false, [&](const handle_t& next) {
                    if (next == graph.flip(h4)) {
                        found1 = true;
                    }
                    count1++;
                    return true;
                });
                graph.follow_edges(graph.flip(h4), true, [&](const handle_t& prev) {
                    if (prev == h4) {
                        found2 = true;
                    }
                    count2++;
                    return true;
                });
                assert(count1 == 1);
                assert(count2 == 1);
                assert(found1);
                assert(found2);
                
                count1 = count2 = 0;
                found1 = found2 = false;
            }
            
            handle_t h5 = graph.create_handle("GGACC");
            
            // make some edges to ensure that deleting is difficult
            graph.create_edge(h, h5);
            graph.create_edge(h5, h);
            graph.create_edge(graph.flip(h5), h2);
            graph.create_edge(h3, graph.flip(h5));
            graph.create_edge(h3, h5);
            graph.create_edge(h5, h4);
            
            graph.destroy_handle(h5);
            
            // DeletableHandleGraph has correct structure after creating and deleting a node
            {
                
                graph.follow_edges(h, false, [&](const handle_t& next) {
                    if (next == h2) {
                        found1 = true;
                    }
                    else if (next == graph.flip(h3)) {
                        found2 = true;
                    }
                    count1++;
                    return true;
                });
                graph.follow_edges(graph.flip(h), true, [&](const handle_t& prev) {
                    if (prev == graph.flip(h2)) {
                        found3 = true;
                    }
                    else if (prev == h3) {
                        found4 = true;
                    }
                    count2++;
                    return true;
                });
                graph.follow_edges(h2, true, [&](const handle_t& prev) {
                    if (prev == h) {
                        found5 = true;
                    }
                    count3++;
                    return true;
                });
                graph.follow_edges(graph.flip(h2), false, [&](const handle_t& next) {
                    if (next == graph.flip(h)) {
                        found6 = true;
                    }
                    count4++;
                    return true;
                });
                graph.follow_edges(graph.flip(h3), true, [&](const handle_t& prev) {
                    if (prev == h) {
                        found7 = true;
                    }
                    count5++;
                    return true;
                });
                graph.follow_edges(h3, false, [&](const handle_t& next) {
                    if (next == graph.flip(h)) {
                        found8 = true;
                    }
                    count6++;
                    return true;
                });
                assert(count1 == 2);
                assert(count2 == 2);
                assert(count3 == 1);
                assert(count4 == 1);
                assert(count5 == 1);
                assert(count6 == 1);
                assert(found1);
                assert(found2);
                assert(found3);
                assert(found4);
                assert(found5);
                assert(found6);
                assert(found7);
                assert(found8);
                
                count1 = count2 = count3 = count4 = count5 = count6 = 0;
                found1 = found2 = found3 = found4 = found5 = found6 = found7 = found8 = false;
                
                graph.follow_edges(h4, false, [&](const handle_t& next) {
                    if (next == graph.flip(h4)) {
                        found1 = true;
                    }
                    count1++;
                    return true;
                });
                graph.follow_edges(graph.flip(h4), true, [&](const handle_t& prev) {
                    if (prev == h4) {
                        found2 = true;
                    }
                    count2++;
                    return true;
                });
                assert(count1 == 1);
                assert(count2 == 1);
                assert(found1);
                assert(found2);
                
                count1 = count2 = 0;
                found1 = found2 = false;
            }
            
            // DeletableHandleGraph has correct structure after swapping nodes
            {
                
                graph.follow_edges(h, false, [&](const handle_t& next) {
                    if (next == h2) {
                        found1 = true;
                    }
                    else if (next == graph.flip(h3)) {
                        found2 = true;
                    }
                    count1++;
                    return true;
                });
                graph.follow_edges(graph.flip(h), true, [&](const handle_t& prev) {
                    if (prev == graph.flip(h2)) {
                        found3 = true;
                    }
                    else if (prev == h3) {
                        found4 = true;
                    }
                    count2++;
                    return true;
                });
                graph.follow_edges(h2, true, [&](const handle_t& prev) {
                    if (prev == h) {
                        found5 = true;
                    }
                    count3++;
                    return true;
                });
                graph.follow_edges(graph.flip(h2), false, [&](const handle_t& next) {
                    if (next == graph.flip(h)) {
                        found6 = true;
                    }
                    count4++;
                    return true;
                });
                graph.follow_edges(graph.flip(h3), true, [&](const handle_t& prev) {
                    if (prev == h) {
                        found7 = true;
                    }
                    count5++;
                    return true;
                });
                graph.follow_edges(h3, false, [&](const handle_t& next) {
                    if (next == graph.flip(h)) {
                        found8 = true;
                    }
                    count6++;
                    return true;
                });
                assert(count1 == 2);
                assert(count2 == 2);
                assert(count3 == 1);
                assert(count4 == 1);
                assert(count5 == 1);
                assert(count6 == 1);
                assert(found1);
                assert(found2);
                assert(found3);
                assert(found4);
                assert(found5);
                assert(found6);
                assert(found7);
                assert(found8);
                
                count1 = count2 = count3 = count4 = count5 = count6 = 0;
                found1 = found2 = found3 = found4 = found5 = found6 = found7 = found8 = false;
                
                graph.follow_edges(h4, false, [&](const handle_t& next) {
                    if (next == graph.flip(h4)) {
                        found1 = true;
                    }
                    count1++;
                    return true;
                });
                graph.follow_edges(graph.flip(h4), true, [&](const handle_t& prev) {
                    if (prev == h4) {
                        found2 = true;
                    }
                    count2++;
                    return true;
                });
                assert(count1 == 1);
                assert(count2 == 1);
                assert(found1);
                assert(found2);
                
                count1 = count2 = 0;
                found1 = found2 = false;
            }
            
            // DeletableHandleGraph visits all nodes with for_each_handle
            {
                graph.for_each_handle([&](const handle_t& handle) {
                    if (handle == h) {
                        found1 = true;
                    }
                    else if (handle == h2) {
                        found2 = true;
                    }
                    else if (handle == h3) {
                        found3 = true;
                    }
                    else if (handle == h4) {
                        found4 = true;
                    }
                    else {
                        assert(false);
                    }
                    return true;
                });
                
                assert(found1);
                assert(found2);
                assert(found3);
                assert(found4);
                
                found1 = found2 = found3 = found4 = false;
            }
            
            // to make sure the sequence reverse complemented correctly
            int i = 0;
            auto check_rev_comp = [&](const std::string& seq1, const std::string& seq2) {
                i++;
                assert(seq1.size() == seq2.size());
                auto it = seq1.begin();
                auto rit = seq2.rbegin();
                for (; it != seq1.end(); it++) {
                    if (*it == 'A') {
                        assert(*rit == 'T');
                    }
                    else if (*it == 'C') {
                        assert(*rit == 'G');
                    }
                    else if (*it == 'G') {
                        assert(*rit == 'C');
                    }
                    else if (*it == 'T') {
                        assert(*rit == 'A');
                    }
                    else if (*it == 'N') {
                        assert(*rit == 'N');
                    }
                    else {
                        assert(false);
                    }
                    
                    rit++;
                }
            };
            
            
            int count7 = 0, count8 = 0;
            
            // DeletableHandleGraph correctly reverses a node
            {
                
                string seq1 = graph.get_sequence(h);
                h = graph.apply_orientation(graph.flip(h));
                
                // check the sequence
                string rev_seq1 = graph.get_sequence(h);
                check_rev_comp(seq1, rev_seq1);
                
                // check that the edges are what we expect
                
                graph.follow_edges(h, false, [&](const handle_t& next) {
                    count1++;
                    return true;
                });
                graph.follow_edges(h, true, [&](const handle_t& prev) {
                    if (prev == graph.flip(h2)) {
                        found1 = true;
                    }
                    else if (prev == h3) {
                        found2 = true;
                    }
                    count2++;
                    return true;
                });
                graph.follow_edges(graph.flip(h), true, [&](const handle_t& next) {
                    count3++;
                    return true;
                });
                graph.follow_edges(graph.flip(h), false, [&](const handle_t& prev) {
                    if (prev == h2) {
                        found3 = true;
                    }
                    else if (prev == graph.flip(h3)) {
                        found4 = true;
                    }
                    count4++;
                    return true;
                });
                graph.follow_edges(h2, true, [&](const handle_t& prev) {
                    if (prev == graph.flip(h)) {
                        found5 = true;
                    }
                    count5++;
                    return true;
                });
                graph.follow_edges(graph.flip(h2), false, [&](const handle_t& next) {
                    if (next == h) {
                        found6 = true;
                    }
                    count6++;
                    return true;
                });
                graph.follow_edges(graph.flip(h3), true, [&](const handle_t& prev) {
                    if (prev == graph.flip(h)) {
                        found7 = true;
                    }
                    count7++;
                    return true;
                });
                graph.follow_edges(h3, false, [&](const handle_t& next) {
                    if (next == h) {
                        found8 = true;
                    }
                    count8++;
                    return true;
                });
                assert(count1 == 0);
                assert(count2 == 2);
                assert(count3 == 0);
                assert(count4 == 2);
                assert(count5 == 1);
                assert(count6 == 1);
                assert(count7 == 1);
                assert(count8 == 1);
                assert(found1);
                assert(found2);
                assert(found3);
                assert(found4);
                assert(found5);
                assert(found6);
                assert(found7);
                assert(found8);
                
                count1 = count2 = count3 = count4 = count5 = count6 = count7 = count8 = 0;
                found1 = found2 = found3 = found4 = found5 = found6 = found7 = found8 = false;
                
                
                // and now switch it back to the same orientation and repeat the topology checks
                
                h = graph.apply_orientation(graph.flip(h));
                
                graph.follow_edges(h, false, [&](const handle_t& next) {
                    if (next == h2) {
                        found1 = true;
                    }
                    else if (next == graph.flip(h3)) {
                        found2 = true;
                    }
                    count1++;
                    return true;
                });
                graph.follow_edges(graph.flip(h), true, [&](const handle_t& prev) {
                    if (prev == graph.flip(h2)) {
                        found3 = true;
                    }
                    else if (prev == h3) {
                        found4 = true;
                    }
                    count2++;
                    return true;
                });
                graph.follow_edges(h2, true, [&](const handle_t& prev) {
                    if (prev == h) {
                        found5 = true;
                    }
                    count3++;
                    return true;
                });
                graph.follow_edges(graph.flip(h2), false, [&](const handle_t& next) {
                    if (next == graph.flip(h)) {
                        found6 = true;
                    }
                    count4++;
                    return true;
                });
                graph.follow_edges(graph.flip(h3), true, [&](const handle_t& prev) {
                    if (prev == h) {
                        found7 = true;
                    }
                    count5++;
                    return true;
                });
                graph.follow_edges(h3, false, [&](const handle_t& next) {
                    if (next == graph.flip(h)) {
                        found8 = true;
                    }
                    count6++;
                    return true;
                });
                assert(count1 == 2);
                assert(count2 == 2);
                assert(count3 == 1);
                assert(count4 == 1);
                assert(count5 == 1);
                assert(count6 == 1);
                assert(found1);
                assert(found2);
                assert(found3);
                assert(found4);
                assert(found5);
                assert(found6);
                assert(found7);
                assert(found8);
                
                count1 = count2 = count3 = count4 = count5 = count6 = 0;
                found1 = found2 = found3 = found4 = found5 = found6 = found7 = found8 = false;
                
                graph.follow_edges(h4, false, [&](const handle_t& next) {
                    if (next == graph.flip(h4)) {
                        found1 = true;
                    }
                    count1++;
                    return true;
                });
                graph.follow_edges(graph.flip(h4), true, [&](const handle_t& prev) {
                    if (prev == h4) {
                        found2 = true;
                    }
                    count2++;
                    return true;
                });
                assert(count1 == 1);
                assert(count2 == 1);
                assert(found1);
                assert(found2);
                
                count1 = count2 = 0;
                found1 = found2 = false;
            }
            
            vector<handle_t> parts = graph.divide_handle(h, vector<size_t>{1, 2});
            
            int count9 = 0, count10 = 0, count11 = 0, count12 = 0;
            bool found9 = false, found10 = false, found11 = false, found12 = false, found13 = false, found14 = false;
            
            // DeletableHandleGraph can correctly divide a node
            {
                
                assert(parts.size() == 3);
                
                assert(graph.get_sequence(parts[0]) == "A");
                assert(graph.get_length(parts[0]) == 1);
                assert(graph.get_sequence(parts[1]) == "T");
                assert(graph.get_length(parts[1]) == 1);
                assert(graph.get_sequence(parts[2]) == "G");
                assert(graph.get_length(parts[2]) == 1);
                
                
                graph.follow_edges(parts[0], false, [&](const handle_t& next) {
                    if (next == parts[1]) {
                        found1 = true;
                    }
                    count1++;
                    return true;
                });
                graph.follow_edges(parts[0], true, [&](const handle_t& prev) {
                    count2++;
                    return true;
                });
                graph.follow_edges(graph.flip(parts[0]), true, [&](const handle_t& prev) {
                    if (prev == graph.flip(parts[1])) {
                        found2 = true;
                    }
                    count3++;
                    return true;
                });
                graph.follow_edges(graph.flip(parts[0]), false, [&](const handle_t& next) {
                    count4++;
                    return true;
                });
                
                graph.follow_edges(parts[1], false, [&](const handle_t& next) {
                    if (next == parts[2]) {
                        found3 = true;
                    }
                    count5++;
                    return true;
                });
                graph.follow_edges(parts[1], true, [&](const handle_t& prev) {
                    if (prev == parts[0]) {
                        found4 = true;
                    }
                    count6++;
                    return true;
                });
                graph.follow_edges(graph.flip(parts[1]), true, [&](const handle_t& prev) {
                    if (prev == graph.flip(parts[2])) {
                        found5 = true;
                    }
                    count7++;
                    return true;
                });
                graph.follow_edges(graph.flip(parts[1]), false, [&](const handle_t& next) {
                    if (next == graph.flip(parts[0])) {
                        found6 = true;
                    }
                    count8++;
                    return true;
                });
                
                graph.follow_edges(parts[2], false, [&](const handle_t& next) {
                    if (next == h2) {
                        found7 = true;
                    }
                    else if (next == graph.flip(h3)) {
                        found8 = true;
                    }
                    count9++;
                    return true;
                });
                graph.follow_edges(parts[2], true, [&](const handle_t& prev) {
                    if (prev == parts[1]) {
                        found9 = true;
                    }
                    count10++;
                    return true;
                });
                graph.follow_edges(graph.flip(parts[2]), true, [&](const handle_t& prev) {
                    if (prev == graph.flip(h2)) {
                        found10 = true;
                    }
                    else if (prev == h3) {
                        found11 = true;
                    }
                    count11++;
                    return true;
                });
                graph.follow_edges(graph.flip(parts[2]), false, [&](const handle_t& next) {
                    if (next == graph.flip(parts[1])) {
                        found12 = true;
                    }
                    count12++;
                    return true;
                });
                graph.follow_edges(graph.flip(h3), true, [&](const handle_t& prev) {
                    if (prev == parts[2]) {
                        found13 = true;
                    }
                    return true;
                });
                graph.follow_edges(h2, true, [&](const handle_t& prev) {
                    if (prev == parts[2]) {
                        found14 = true;
                    }
                    return true;
                });
                
                assert(count1 == 1);
                assert(count2 == 0);
                assert(count3 == 1);
                assert(count4 == 0);
                assert(count5 == 1);
                assert(count6 == 1);
                assert(count7 == 1);
                assert(count8 == 1);
                assert(count9 == 2);
                assert(count10 == 1);
                assert(count11 == 2);
                assert(count12 == 1);
                assert(found1);
                assert(found2);
                assert(found3);
                assert(found4);
                assert(found5);
                assert(found6);
                assert(found7);
                assert(found8);
                assert(found9);
                assert(found10);
                assert(found11);
                assert(found12);
                assert(found13);
                assert(found14);
                
                count1 = count2 = count3 = count4 = count5 = count6 = count7 = count8 = count9 = count10 = count11 = count12 = 0;
                found1 = found2 = found3 = found4 = found5 = found6 = found7 = found8 = found9 = found10 = found11 = found12 = false;
            }
            
            vector<handle_t> rev_parts = graph.divide_handle(graph.flip(h3), vector<size_t>{1});
            
            // DeletableHandleGraph can correctly divide a node on the reverse strand
            {
                
                assert(graph.get_sequence(rev_parts[0]) == "G");
                assert(graph.get_length(rev_parts[0]) == 1);
                assert(graph.get_is_reverse(rev_parts[0]));
                assert(graph.get_sequence(rev_parts[1]) == "TC");
                assert(graph.get_length(rev_parts[1]) == 2);
                assert(graph.get_is_reverse(rev_parts[1]));
                
                graph.follow_edges(rev_parts[0], false, [&](const handle_t& next) {
                    if (next == rev_parts[1]) {
                        found1 = true;
                    }
                    count1++;
                    return true;
                });
                graph.follow_edges(rev_parts[1], true, [&](const handle_t& prev) {
                    if (prev == rev_parts[0]) {
                        found2 = true;
                    }
                    count2++;
                    return true;
                });
                graph.follow_edges(graph.flip(rev_parts[1]), false, [&](const handle_t& next) {
                    if (next == graph.flip(rev_parts[0])) {
                        found3 = true;
                    }
                    count3++;
                    return true;
                });
                graph.follow_edges(graph.flip(rev_parts[0]), true, [&](const handle_t& prev) {
                    if (prev == graph.flip(rev_parts[1])) {
                        found4 = true;
                    }
                    count4++;
                    return true;
                });
                graph.follow_edges(rev_parts[0], true, [&](const handle_t& prev) {
                    if (prev == parts[2]) {
                        found5 = true;
                    }
                    count5++;
                    return true;
                });
                graph.follow_edges(rev_parts[1], false, [&](const handle_t& next) {
                    count6++;
                    return true;
                });
                
                assert(count1 == 1);
                assert(count2 == 1);
                assert(count3 == 1);
                assert(count4 == 1);
                assert(count5 == 1);
                assert(count6 == 0);
                assert(found1);
                assert(found2);
                assert(found3);
                assert(found4);
                assert(found5);
            }
        }
    }
    
    // second batch of test involving self loops
    {
        PackedGraph pg;
        implementations.push_back(&pg);
        
        HashGraph hg;
        implementations.push_back(&hg);
        
        ODGI og;
        implementations.push_back(&og);
        
        for (DeletableHandleGraph* implementation : implementations) {
            
            DeletableHandleGraph& graph = *implementation;
            
            // initialize the graph
            
            handle_t h1 = graph.create_handle("A");
            handle_t h2 = graph.create_handle("C");
            
            graph.create_edge(h1, h2);
            graph.create_edge(graph.flip(h1), h2);
            
            // test for the right initial topology
            bool found1 = false, found2 = false, found3 = false, found4 = false, found5 = false, found6 = false;
            int count1 = 0, count2 = 0, count3 = 0, count4 = 0;
            
            graph.follow_edges(h1, false, [&](const handle_t& other) {
                if (other == h2) {
                    found1 = true;
                }
                count1++;
            });
            graph.follow_edges(h1, true, [&](const handle_t& other) {
                if (other == graph.flip(h2)) {
                    found2 = true;
                }
                count2++;
            });
            graph.follow_edges(h2, false, [&](const handle_t& other) {
                count3++;
            });
            graph.follow_edges(h2, true, [&](const handle_t& other) {
                if (other == h1) {
                    found3 = true;
                }
                else if (other == graph.flip(h1)) {
                    found4 = true;
                }
                count4++;
            });
            assert(found1);
            assert(found2);
            assert(found3);
            assert(found4);
            assert(count1 == 1);
            assert(count2 == 1);
            assert(count3 == 0);
            assert(count4 == 2);
            found1 = found2 = found3 = found4 = found5 = found6 = false;
            count1 = count2 = count3 = count4 = 0;
            
            // flip a node and check if the orientation is correct
            h1 = graph.apply_orientation(graph.flip(h1));
            
            graph.follow_edges(h1, false, [&](const handle_t& other) {
                if (other == h2) {
                    found1 = true;
                }
                count1++;
            });
            graph.follow_edges(h1, true, [&](const handle_t& other) {
                if (other == graph.flip(h2)) {
                    found2 = true;
                }
                count2++;
            });
            graph.follow_edges(h2, false, [&](const handle_t& other) {
                count3++;
            });
            graph.follow_edges(h2, true, [&](const handle_t& other) {
                if (other == h1) {
                    found3 = true;
                }
                else if (other == graph.flip(h1)) {
                    found4 = true;
                }
                count4++;
            });
            assert(found1);
            assert(found2);
            assert(found3);
            assert(found4);
            assert(count1 == 1);
            assert(count2 == 1);
            assert(count3 == 0);
            assert(count4 == 2);
            found1 = found2 = found3 = found4 = found5 = found6 = false;
            count1 = count2 = count3 = count4 = 0;
            
            // create a new edge
            
            graph.create_edge(h1, graph.flip(h2));
            
            // check the topology
            
            graph.follow_edges(h1, false, [&](const handle_t& other) {
                if (other == h2) {
                    found1 = true;
                }
                else if (other == graph.flip(h2)) {
                    found2 = true;
                }
                count1++;
            });
            graph.follow_edges(h1, true, [&](const handle_t& other) {
                if (other == graph.flip(h2)) {
                    found3 = true;
                }
                count2++;
            });
            graph.follow_edges(h2, false, [&](const handle_t& other) {
                if (other == graph.flip(h1)) {
                    found4 = true;
                }
                count3++;
            });
            graph.follow_edges(h2, true, [&](const handle_t& other) {
                if (other == h1) {
                    found5 = true;
                }
                else if (other == graph.flip(h1)) {
                    found6 = true;
                }
                count4++;
            });
            assert(found1);
            assert(found2);
            assert(found3);
            assert(found4);
            assert(found5);
            assert(found6);
            assert(count1 == 2);
            assert(count2 == 1);
            assert(count3 == 1);
            assert(count4 == 2);
            found1 = found2 = found3 = found4 = found5 = found6 = false;
            count1 = count2 = count3 = count4 = 0;
            
            // now another node and check to make sure that the edges are updated appropriately
            
            h2 = graph.apply_orientation(graph.flip(h2));
            
            graph.follow_edges(h1, false, [&](const handle_t& other) {
                if (other == h2) {
                    found1 = true;
                }
                else if (other == graph.flip(h2)) {
                    found2 = true;
                }
                count1++;
            });
            graph.follow_edges(h1, true, [&](const handle_t& other) {
                if (other == h2) {
                    found3 = true;
                }
                count2++;
            });
            graph.follow_edges(h2, false, [&](const handle_t& other) {
                if (other == h1) {
                    found4 = true;
                }
                else if (other == graph.flip(h1)) {
                    found5 = true;
                }
                count3++;
            });
            graph.follow_edges(h2, true, [&](const handle_t& other) {
                if (other == h1) {
                    found6 = true;
                }
                count4++;
            });
            assert(found1);
            assert(found2);
            assert(found3);
            assert(found4);
            assert(found5);
            assert(found6);
            assert(count1 == 2);
            assert(count2 == 1);
            assert(count3 == 2);
            assert(count4 == 1);
        }
    }
    
    cerr << "DeletableHandleGraph tests successful!" << endl;
}

void test_mutable_path_handle_graphs() {
    
    vector<MutablePathDeletableHandleGraph*> implementations;
    
    PackedGraph pg;
    implementations.push_back(&pg);
    
    HashGraph hg;
    implementations.push_back(&hg);
    
    ODGI og;
    implementations.push_back(&og);
    
    for (MutablePathDeletableHandleGraph* implementation : implementations) {
        
        MutablePathDeletableHandleGraph& graph = *implementation;
        
        auto check_path = [&](const path_handle_t& p, const vector<handle_t>& steps) {
            
            assert(graph.get_step_count(p) == steps.size());
            
            step_handle_t step = graph.path_begin(p);
            for (int i = 0; i < steps.size(); i++) {
                
                assert(graph.get_path_handle_of_step(step) == p);
                assert(graph.get_handle_of_step(step) == steps[i]);
                
                if (graph.get_is_circular(p)) {
                    assert(graph.has_next_step(step));
                    assert(graph.has_previous_step(step));
                }
                else {
                    assert(graph.has_next_step(step) == i + 1 < steps.size());
                    assert(graph.has_previous_step(step) == i > 0);
                }
                
                step = graph.get_next_step(step);
            }
            
            if (graph.get_is_circular(p) && !graph.is_empty(p)) {
                assert(step == graph.path_begin(p));
            }
            else {
                assert(step == graph.path_end(p));
            }
            
            step = graph.path_back(p);
            
            for (int i = steps.size() - 1; i >= 0; i--) {
                
                assert(graph.get_path_handle_of_step(step) == p);
                assert(graph.get_handle_of_step(step) == steps[i]);
                
                if (graph.get_is_circular(p)) {
                    assert(graph.has_next_step(step));
                    assert(graph.has_previous_step(step));
                }
                else {
                    assert(graph.has_next_step(step) == i + 1 < steps.size());
                    assert(graph.has_previous_step(step) == i > 0);
                }
                
                step = graph.get_previous_step(step);
            }
            
            if (graph.get_is_circular(p) && !graph.is_empty(p)) {
                assert(step == graph.path_back(p));
            }
            else {
                assert(step == graph.path_front_end(p));
            }
        };
        
        auto check_flips = [&](const path_handle_t& p, const vector<handle_t>& steps) {
            auto flipped = steps;
            for (size_t i = 0; i < steps.size(); i++) {
                graph.apply_orientation(graph.flip(graph.forward(flipped[i])));
                flipped[i] = graph.flip(flipped[i]);
                check_path(p, flipped);
                
                graph.apply_orientation(graph.flip(graph.forward(flipped[i])));
                flipped[i] = graph.flip(flipped[i]);
                check_path(p, flipped);
            }
        };
        
        handle_t h1 = graph.create_handle("AC");
        handle_t h2 = graph.create_handle("CAGTGA");
        handle_t h3 = graph.create_handle("GT");
        
        graph.create_edge(h1, h2);
        graph.create_edge(h2, h3);
        graph.create_edge(h1, graph.flip(h2));
        graph.create_edge(graph.flip(h2), h3);
        
        assert(!graph.has_path("1"));
        assert(graph.get_path_count() == 0);
        
        path_handle_t p1 = graph.create_path_handle("1");
        
        assert(graph.has_path("1"));
        assert(graph.get_path_count() == 1);
        assert(graph.get_path_handle("1") == p1);
        assert(graph.get_path_name(p1) == "1");
        assert(graph.get_step_count(p1) == 0);
        assert(graph.is_empty(p1));
        
        graph.append_step(p1, h1);
        
        assert(graph.get_step_count(p1) == 1);
        assert(!graph.is_empty(p1));
        
        graph.append_step(p1, h2);
        graph.append_step(p1, h3);
        
        assert(graph.get_step_count(p1) == 3);
        
        // graph can traverse a path
        check_path(p1, {h1, h2, h3});
        
        // graph preserves paths when reversing nodes
        check_flips(p1, {h1, h2, h3});
        
        // make a circular path
        path_handle_t p2 = graph.create_path_handle("2", true);
        assert(graph.get_path_count() == 2);
        
        graph.append_step(p2, h1);
        graph.append_step(p2, graph.flip(h2));
        graph.append_step(p2, h3);
        
        check_path(p2, {h1, graph.flip(h2), h3});
        
        // graph can query steps of a node on paths
        
        bool found1 = false, found2 = false;
        vector<step_handle_t> steps = graph.steps_of_handle(h1);
        for (auto& step : steps) {
            if (graph.get_path_handle_of_step(step) == p1 &&
                graph.get_handle_of_step(step) == h1) {
                found1 = true;
            }
            else if (graph.get_path_handle_of_step(step) == p2 &&
                     graph.get_handle_of_step(step) == h1) {
                found2 = true;
            }
            else {
                assert(false);
            }
        }
        assert(found1);
        assert(found2);
        found1 = found2 = false;
        
        steps = graph.steps_of_handle(h1, true);
        for (auto& step : steps) {
            if (graph.get_path_handle_of_step(step) == p1 &&
                graph.get_handle_of_step(step) == h1) {
                found1 = true;
            }
            else if (graph.get_path_handle_of_step(step) == p2 &&
                     graph.get_handle_of_step(step) == h1) {
                found2 = true;
            }
            else {
                assert(false);
            }
        }
        assert(found1);
        assert(found2);
        found1 = found2 = false;
        
        steps = graph.steps_of_handle(graph.flip(h1), true);
        for (auto& step : steps) {
            assert(false);
        }
        
        steps = graph.steps_of_handle(h2, true);
        for (auto& step : steps) {
            if (graph.get_path_handle_of_step(step) == p1 &&
                graph.get_handle_of_step(step) == h2) {
                found1 = true;
            }
            else {
                assert(false);
            }
        }
        steps = graph.steps_of_handle(graph.flip(h2), true);
        for (auto& step : steps) {
            if (graph.get_path_handle_of_step(step) == p2 &&
                graph.get_handle_of_step(step) == graph.flip(h2)) {
                found2 = true;
            }
            else {
                assert(false);
            }
        }
        assert(found1);
        assert(found2);
        found1 = found2 = false;
        
        vector<handle_t> segments = graph.divide_handle(h2, {size_t(2), size_t(4)});
        
        // graph preserves paths when dividing nodes
        
        check_path(p1, {h1, segments[0], segments[1], segments[2], h3});
        check_path(p2, {h1, graph.flip(segments[2]), graph.flip(segments[1]), graph.flip(segments[0]), h3});
        
        path_handle_t p3 = graph.create_path_handle("3");
        graph.append_step(p3, h1);
        graph.append_step(p3, segments[0]);
        
        assert(graph.has_path("3"));
        assert(graph.get_path_count() == 3);
        
        // graph can toggle circularity
        
        graph.for_each_path_handle([&](const path_handle_t& p) {
            
            vector<handle_t> steps;
            
            for (handle_t h : graph.scan_path(p)) {
                steps.push_back(h);
            }
            
            bool starting_circularity = graph.get_is_circular(p);
            
            // make every transition occur
            for (bool circularity : {true, true, false, false, true}) {
                graph.set_circularity(p, circularity);
                assert(graph.get_is_circular(p) == circularity);
                check_path(p, steps);
            }
            
            graph.set_circularity(p, starting_circularity);
        });
        
        // graph can destroy paths
        
        graph.destroy_path(p3);
        
        assert(!graph.has_path("3"));
        assert(graph.get_path_count() == 2);
        
        bool found3 = false;
        
        graph.for_each_path_handle([&](const path_handle_t& p) {
            if (graph.get_path_name(p) == "1") {
                found1 = true;
            }
            else if (graph.get_path_name(p) == "2") {
                found2 = true;
            }
            else if (graph.get_path_name(p) == "3") {
                found3 = true;
            }
            else {
                assert(false);
            }
        });
        
        assert(found1);
        assert(found2);
        assert(!found3);
        
        // check flips to see if membership records are still functional
        check_flips(p1, {h1, segments[0], segments[1], segments[2], h3});
        check_flips(p2, {h1, graph.flip(segments[2]), graph.flip(segments[1]), graph.flip(segments[0]), h3});
        
        graph.destroy_path(p1);
        
        assert(!graph.has_path("1"));
        assert(graph.get_path_count() == 1);
        
        found1 = found2 = found3 = false;
        
        graph.for_each_path_handle([&](const path_handle_t& p) {
            if (graph.get_path_name(p) == "1") {
                found1 = true;
            }
            else if (graph.get_path_name(p) == "2") {
                found2 = true;
            }
            else if (graph.get_path_name(p) == "3") {
                found3 = true;
            }
            else {
                assert(false);
            }
        });
        
        assert(!found1);
        assert(found2);
        assert(!found3);
        
        // check flips to see if membership records are still functional
        check_flips(p2, {h1, graph.flip(segments[2]), graph.flip(segments[1]), graph.flip(segments[0]), h3});
        
        // make a path to rewrite
        path_handle_t p4 = graph.create_path_handle("4");
        graph.prepend_step(p4, h3);
        graph.prepend_step(p4, segments[2]);
        graph.prepend_step(p4, segments[1]);
        graph.prepend_step(p4, segments[0]);
        graph.prepend_step(p4, h1);
        
        check_flips(p4, {h1, segments[0], segments[1], segments[2], h3});
        
        auto check_rewritten_segment = [&](const pair<step_handle_t, step_handle_t>& new_segment,
                                           const vector<handle_t>& steps) {
            int i = 0;
            for (auto step = new_segment.first; step != new_segment.second; step = graph.get_next_step(step)) {
                assert(graph.get_handle_of_step(step) == steps[i]);
                i++;
            }
            assert(i == steps.size());
        };
        
        // rewrite the middle portion of a path
        
        step_handle_t s1 = graph.get_next_step(graph.path_begin(p4));
        step_handle_t s2 = graph.get_next_step(graph.get_next_step(graph.get_next_step(s1)));
        
        auto new_segment = graph.rewrite_segment(s1, s2, {graph.flip(segments[2]), graph.flip(segments[1]), graph.flip(segments[0])});
        
        check_flips(p4, {h1, graph.flip(segments[2]), graph.flip(segments[1]), graph.flip(segments[0]), h3});
        check_rewritten_segment(new_segment, {graph.flip(segments[2]), graph.flip(segments[1]), graph.flip(segments[0])});
        
        // rewrite around the end of a circular path to delete
        
        graph.create_edge(h3, h1);
        graph.create_edge(segments[2], segments[0]);
        graph.set_circularity(p4, true);
        
        s1 = graph.get_previous_step(graph.path_begin(p4));
        s2 = graph.get_next_step(graph.path_begin(p4));
        
        new_segment = graph.rewrite_segment(s1, s2, vector<handle_t>());
        
        check_flips(p4, {graph.flip(segments[2]), graph.flip(segments[1]), graph.flip(segments[0])});
        check_rewritten_segment(new_segment, vector<handle_t>());
        
        // add into an empty slot
        
        new_segment = graph.rewrite_segment(new_segment.first, new_segment.second, {graph.flip(h1), graph.flip(h3)});
        
        check_flips(p4, {graph.flip(h1), graph.flip(h3), graph.flip(segments[2]), graph.flip(segments[1]), graph.flip(segments[0])});
        check_rewritten_segment(new_segment, {graph.flip(h1), graph.flip(h3)});
        
    }
    
    cerr << "MutablePathDeletableHandleGraph tests successful!" << endl;
}

int main(void) {
    
    test_deletable_handle_graphs();
    test_mutable_path_handle_graphs();
    
}
