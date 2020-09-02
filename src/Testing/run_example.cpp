#include <iostream>
#include <memory>
#include "ShortestPathHeuristic.h"
#include "../Utils/Definitions.h"
#include "../Utils/IOUtils.h"
#include "../Utils/Logger.h"
#include "../SearchUtils/PathPair.h"
#include "../SearchUtils/SearchNode.h"
#include "../BiCriteria/BOAStar.h"
#include "../BiCriteria/PPA.h"
#include "../TriCriteria/TriAStar.h"
#include "../TriCriteria/TPPA.h"

const std::string resource_path = "src/Testing/Resources/";

void simple_test_case(LoggerPtr logger, EpsType eps) {
    std::cout << "-----Start Simple Test Case EPS=" << eps << "-----" << std::endl;

    std::vector<Edge> edges = {
        {.source=1, .target=2, .cost={1,1,0}},
        {.source=1, .target=3, .cost={1,1,0}},
        {.source=3, .target=5, .cost={1,2,0}},
        {.source=2, .target=5, .cost={2,2,0}},
        {.source=2, .target=4, .cost={1,1,0}},
        {.source=4, .target=6, .cost={2,1,0}},
        {.source=5, .target=6, .cost={1,1,0}},
    };

    AdjacencyMatrix graph(6, edges);
    AdjacencyMatrix inv_graph(6, edges, true);

    std::cout << "Graph:" << std::endl;
    std::cout << graph << std::endl;

    std::cout << "Inverse Graph:" << std::endl;
    std::cout << inv_graph << std::endl;

    ShortestPathHeuristic heuristic(inv_graph, 6);
    std::cout << "Heuristic:" << std::endl;
    std::cout << 1 << ": " << heuristic(1) << std::endl;
    std::cout << 2 << ": " << heuristic(2) << std::endl;
    std::cout << 3 << ": " << heuristic(3) << std::endl;
    std::cout << 4 << ": " << heuristic(4) << std::endl;
    std::cout << 5 << ": " << heuristic(5) << std::endl;
    std::cout << 6 << ": " << heuristic(6) << std::endl;

    using std::placeholders::_1;
    Heuristic heur = std::bind( &ShortestPathHeuristic::operator(), heuristic, _1);


    SearchNode::SolutionsSet boa_solutions;
    BOAStar boa_star(graph, {eps,eps}, logger);
    boa_star(1, 6, boa_solutions, heur);

    std::cout << "\nBOAStar Solutions:" << std::endl;
    for (auto iter = boa_solutions.begin(); iter != boa_solutions.end(); ++iter) {
        std::cout << **iter << std::endl;
    }


    SearchNode::SolutionsSet ppa_solutions;
    PPA ppa(graph, {eps,eps}, logger);
    ppa(1, 6, ppa_solutions, heur);

    std::cout << "\nPPA Solutions:" << std::endl;
    for (auto iter = ppa_solutions.begin(); iter != ppa_solutions.end(); ++iter) {
        std::cout << **iter << std::endl;
    }

    std::cout << "-----End Simple Test Case-----" << std::endl;
}



void NY_single_test_case(LoggerPtr logger, EpsType eps) {
    std::cout << "-----Start NY Single Test Case EPS=" << eps << "-----" << std::endl;

    // Easy - C_BOA gets around 20ms
    // Idx source = 9899;
    // Idx target = 7857;

    // Hard - C_BOA gets around 2k ms
    Idx source = 180834;
    Idx target = 83150;

    size_t graph_size;
    std::vector<Edge> edges;
    if (load_gr_files(resource_path+"USA-road-d.NY.gr", resource_path+"USA-road-t.NY.gr", edges, graph_size) == false) {
        std::cout << "Failed to load gr files" << std::endl;
        return;
    }

    AdjacencyMatrix graph(graph_size, edges);
    AdjacencyMatrix inv_graph(graph_size, edges, true);

    std::cout << "Start Computing Heuristic" << std::endl;
    ShortestPathHeuristic sp_heuristic(inv_graph, target);
    std::cout << "Finish Computing Heuristic" << std::endl;

    using std::placeholders::_1;
    Heuristic heuristic = std::bind( &ShortestPathHeuristic::operator(), sp_heuristic, _1);

    std::cout << "Start Computing BOAStar" << std::endl;
    SearchNode::SolutionsSet boa_solutions;
    BOAStar boa_star(graph, {eps,eps}, logger);
    boa_star(source, target, boa_solutions, heuristic);
    std::cout << "Finish Computing BOAStar" << std::endl;

    std::cout << "Start Computing PPA" << std::endl;
    SearchNode::SolutionsSet ppa_solutions;
    PPA ppa(graph, {eps,eps}, logger);
    ppa(source, target, ppa_solutions, heuristic);
    std::cout << "Finish Computing PPA" << std::endl;

    std::cout << "-----End NY Single Test Case-----" << std::endl;
}



void NY_queries_short_test_case(LoggerPtr logger, EpsType eps) {
    std::cout << "-----Start NY Queries-Short Test Case EPS=" << eps << "-----" << std::endl;

    std::vector<std::pair<Idx, Idx>> queries;
    if (load_queries(resource_path+"USA-road-NY-queries-short", queries) == false) {
        std::cout << "Failed to load queries file" << std::endl;
        return;
    }

    size_t graph_size;
    std::vector<Edge> edges;
    if (load_gr_files(resource_path+"USA-road-d.NY.gr", resource_path+"USA-road-t.NY.gr", edges, graph_size) == false) {
        std::cout << "Failed to load gr files" << std::endl;
        return;
    }

    AdjacencyMatrix graph(graph_size, edges);
    AdjacencyMatrix inv_graph(graph_size, edges, true);

    size_t count = 0;
    for (auto iter = queries.begin(); iter != queries.end(); ++iter) {
        std::cout << "Started Query: " << ++count << std::endl;
        Idx source = iter->first;
        Idx target = iter->second;

        ShortestPathHeuristic sp_heuristic(inv_graph, target);

        using std::placeholders::_1;
        Heuristic heuristic = std::bind( &ShortestPathHeuristic::operator(), sp_heuristic, _1);

        SearchNode::SolutionsSet boa_solutions;
        BOAStar boa_star(graph, {eps,eps}, logger);
        boa_star(source, target, boa_solutions, heuristic);

        SearchNode::SolutionsSet ppa_solutions;
        PPA ppa(graph, {eps,eps}, logger);
        ppa(source, target, ppa_solutions, heuristic);
    }

    std::cout << "-----End NY Queries-Short Test Case-----" << std::endl;
}




// void simple_3test_case(LoggerPtr logger, EpsType eps) {
//     std::cout << "-----Start Simple 3Test Case EPS=" << eps << "-----" << std::endl;

//     std::vector<Edge> edges = {
//         {.source=1, .target=2, .cost={1,1,1}},
//         {.source=1, .target=3, .cost={1,1,3}},
//         {.source=3, .target=5, .cost={1,2,2}},
//         {.source=2, .target=5, .cost={2,2,4}},
//         {.source=2, .target=4, .cost={1,1,0}},
//         {.source=4, .target=6, .cost={2,1,2}},
//         {.source=5, .target=6, .cost={1,1,1}},
//     };

//     AdjacencyMatrix graph(edges);
//     AdjacencyMatrix inv_graph(edges, true);

//     std::cout << "Graph:" << std::endl;
//     std::cout << graph << std::endl;

//     std::cout << "Inverse Graph:" << std::endl;
//     std::cout << inv_graph << std::endl;

//     ShortestPathHeuristic heuristic(inv_graph, 6);
//     std::cout << "Heuristic:" << std::endl;
//     std::cout << 1 << ": " << heuristic(1) << std::endl;
//     std::cout << 2 << ": " << heuristic(2) << std::endl;
//     std::cout << 3 << ": " << heuristic(3) << std::endl;
//     std::cout << 4 << ": " << heuristic(4) << std::endl;
//     std::cout << 5 << ": " << heuristic(5) << std::endl;
//     std::cout << 6 << ": " << heuristic(6) << std::endl;

//     using std::placeholders::_1;
//     Heuristic heur = std::bind( &ShortestPathHeuristic::operator(), heuristic, _1);

//     SearchNode::SolutionsSet ta_star_solutions;
//     TriAStar ta_star(graph, {eps,eps,eps}, logger);
//     ta_star(1, 6, ta_star_solutions, heur);

//     std::cout << "\nTriAStar Solutions:" << std::endl;
//     for (auto iter = ta_star_solutions.begin(); iter != ta_star_solutions.end(); ++iter) {
//         std::cout << **iter << std::endl;
//     }



//     SearchNode::SolutionsSet tppa_solutions;
//     TPPA tppa(graph, {eps,eps,eps}, logger);
//     tppa(1, 6, tppa_solutions, heur);

//     std::cout << "\nTPPA Solutions:" << std::endl;
//     for (auto iter = tppa_solutions.begin(); iter != tppa_solutions.end(); ++iter) {
//         std::cout << **iter << std::endl;
//     }

//     std::cout << "-----End Simple 3Test Case-----" << std::endl;
// }

// void NY_single_3test_case(LoggerPtr logger, EpsType eps) {
//     std::cout << "-----Start NY Single 3Test Case EPS=" << eps << "-----" << std::endl;

//     Idx source = 213;
//     Idx target = 522;

//     std::vector<Edge> edges;
//     if (load_gr_files(resource_path+"USA-road-d.NY-Small.gr",
//                       resource_path+"USA-road-t.NY-Small.gr",
//                       resource_path+"USA-road-w.NY-Small.gr", edges) == false) {
//         std::cout << "Failed to load gr files" << std::endl;
//         return;
//     }

//     AdjacencyMatrix graph(edges);
//     AdjacencyMatrix inv_graph(edges, true);

//     std::cout << "Start Computing Heuristic" << std::endl;
//     ShortestPathHeuristic sp_heuristic(inv_graph, target);
//     std::cout << "Finish Computing Heuristic" << std::endl;

//     using std::placeholders::_1;
//     Heuristic heuristic = std::bind( &ShortestPathHeuristic::operator(), sp_heuristic, _1);

//     std::cout << "Start Computing TriAStar" << std::endl;
//     SearchNode::SolutionsSet ta_star_solutions;
//     TriAStar ta_star(graph, {eps,eps, eps}, logger);
//     ta_star(source, target, ta_star_solutions, heuristic);
//     std::cout << "Finish Computing TriAStar" << std::endl;

//     // std::cout << "Start Computing TPPA" << std::endl;
//     // SearchNode::SolutionsSet tppa_solutions;
//     // TPPA tppa(graph, {eps,eps, eps}, logger);
//     // tppa(source, target, tppa_solutions, heuristic);
//     // std::cout << "Finish Computing TPPA" << std::endl;

//     std::cout << "-----End NY Single 3Test Case-----" << std::endl;
// }

int main(void) {
    LoggerPtr logger = std::make_shared<Logger>("log.txt");
    // simple_test_case(logger, 0);
    NY_single_test_case(logger, 0);
    // NY_queries_short_test_case(logger, 0.1);
    // NY_queries_short_test_case(logger, 0.015);

    return 0;
}

