#include <iostream>
#include <memory>

#include "ShortestPathHeuristic.h"
#include "../Utils/Definitions.h"
#include "../Utils/IOUtils.h"
#include "../Utils/Logger.h"
#include "../BiCriteria/BOAStar.h"
#include "../BiCriteria/PPA.h"

const std::string resource_path = "src/Example/Resources/";

void NYMap_Single(size_t source, size_t target, double eps, LoggerPtr logger) {
    std::cout << "-----Start NY Map Single Example: SRC=" << source << " DEST=" << target << " EPS=" << eps << "-----" << std::endl;

    // Load files
    size_t graph_size;
    std::vector<Edge> edges;
    if (load_gr_files(resource_path+"USA-road-d.NY.gr", resource_path+"USA-road-t.NY.gr", edges, graph_size) == false) {
        std::cout << "Failed to load gr files" << std::endl;
        return;
    }

    // Build graphs
    AdjacencyMatrix graph(graph_size, edges);
    AdjacencyMatrix inv_graph(graph_size, edges, true);

    // Compute heuristic
    std::cout << "Start Computing Heuristic" << std::endl;
    ShortestPathHeuristic sp_heuristic(target, graph_size, inv_graph);
    std::cout << "Finish Computing Heuristic" << std::endl;

    using std::placeholders::_1;
    Heuristic heuristic = std::bind( &ShortestPathHeuristic::operator(), sp_heuristic, _1);

    // Compute BOAStar
    std::cout << "Start Computing BOAStar" << std::endl;
    SolutionSet boa_solutions;
    BOAStar boa_star(graph, {eps,eps}, logger);
    boa_star(source, target, heuristic, boa_solutions);
    std::cout << "Finish Computing BOAStar" << std::endl;

    // Compute PPA
    std::cout << "Start Computing PPA" << std::endl;
    SolutionSet ppa_solutions;
    PPA ppa(graph, {eps,eps}, logger);
    ppa(source, target, heuristic, ppa_solutions);
    std::cout << "Finish Computing PPA" << std::endl;

    std::cout << "-----End NY Map Single Example-----" << std::endl;
}

void NYMap_Queries(double eps, LoggerPtr logger) {
    std::cout << "-----Start NY Map Queries Example: EPS=" << eps << "-----" << std::endl;

    // Load files
    size_t graph_size;
    std::vector<Edge> edges;
    if (load_gr_files(resource_path+"USA-road-d.NY.gr", resource_path+"USA-road-t.NY.gr", edges, graph_size) == false) {
        std::cout << "Failed to load gr files" << std::endl;
        return;
    }

    std::vector<std::pair<size_t, size_t>> queries;
    if (load_queries(resource_path+"USA-road-NY-queries", queries) == false) {
        std::cout << "Failed to load queries file" << std::endl;
        return;
    }

    // Build graphs
    AdjacencyMatrix graph(graph_size, edges);
    AdjacencyMatrix inv_graph(graph_size, edges, true);

    size_t query_count = 0;
    for (auto iter = queries.begin(); iter != queries.end(); ++iter) {
        std::cout << "Started Query: " << ++query_count << std::endl;
        size_t source = iter->first;
        size_t target = iter->second;

        ShortestPathHeuristic sp_heuristic(target, graph_size, inv_graph);

        using std::placeholders::_1;
        Heuristic heuristic = std::bind( &ShortestPathHeuristic::operator(), sp_heuristic, _1);

        SolutionSet boa_solutions;
        BOAStar boa_star(graph, {eps,eps}, logger);
        boa_star(source, target, heuristic, boa_solutions);

        SolutionSet ppa_solutions;
        PPA ppa(graph, {eps,eps}, logger);
        ppa(source, target, heuristic, ppa_solutions);
    }

    std::cout << "-----End NY Map Queries Example-----" << std::endl;
}

int main(void) {
    LoggerPtr logger = std::make_shared<Logger>("log_queries_long.txt");
    // Easy - C_BOA gets around 20ms
    size_t easy_source = 9899;
    size_t easy_target = 7857;

    // Hard - C_BOA gets around 2k ms
    // size_t hard_source = 180834;
    // size_t hard_target = 83150;

    // NYMap_Single(easy_source, easy_target, 0, logger);
    // NYMap_Single(easy_source, easy_target, 0.1, logger);

    NYMap_Queries(0.1, logger);
    NYMap_Queries(0.05, logger);
    NYMap_Queries(0.025, logger);
    NYMap_Queries(0.01, logger);
    NYMap_Queries(0, logger);

    return 0;
}

