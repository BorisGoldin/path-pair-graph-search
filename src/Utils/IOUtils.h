#ifndef UTILS_IO_UTILS_H
#define UTILS_IO_UTILS_H

#include <string>
#include <vector>
#include "Definitions.h"

bool load_gr_files(std::string gr_file1, std::string gr_file2, std::vector<Edge> &edges, size_t &graph_size);
bool load_gr_files(std::string gr_file1, std::string gr_file2, std::string gr_file3, std::vector<Edge> &edges, size_t &graph_size);
bool load_txt_file(std::string txt_file, std::vector<Edge> &edges_out, size_t &graph_size);
bool load_queries(std::string query_file, std::vector<std::pair<Idx, Idx>> &queries_out);

#endif //UTILS_IO_UTILS_H
