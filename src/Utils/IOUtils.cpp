#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "IOUtils.h"

void string_split(std::string string, std::string delimiter, std::vector<std::string> &results)
{
    size_t first_delimiter;

    while ((first_delimiter = string.find_first_of(delimiter)) != string.npos) {
        if (first_delimiter > 0) {
            results.push_back(string.substr(0, first_delimiter));
        }
        string = string.substr(first_delimiter + 1);
    }

    if (string.length() > 0) {
        results.push_back(string);
    }
}

bool load_gr_files(std::string gr_file1, std::string gr_file2, std::string gr_file3, std::vector<Edge> &edges_out, size_t &graph_size) {
    size_t          max_node_num = 0;
    std::ifstream   file1(gr_file1.c_str());
    std::ifstream   file2(gr_file2.c_str());
    std::ifstream   file3(gr_file3.c_str());

    if ((file1.is_open() == false) || (file2.is_open() == false) || (file3.is_open() == false)) {
        return false;
    }

    std::string line1, line2, line3;
    while ((file1.eof() == false) && (file2.eof() == false) && (file3.eof() == false)) {
        std::getline(file1, line1);
        std::getline(file2, line2);
        std::getline(file3, line3);

        if ((line1 == "") || (line2 == "") || (line3 == "")) {
            break;
        }

        std::vector<std::string> decomposed_line1, decomposed_line2, decomposed_line3;
        string_split(line1, " ", decomposed_line1);
        string_split(line2, " ", decomposed_line2);
        string_split(line3, " ", decomposed_line3);

        std::string type = decomposed_line1[0];
        if ((std::strcmp(type.c_str(),"c") == 0) || (std::strcmp(type.c_str(),"p") == 0)) {
            continue; //comment or problem
        }

        if ((decomposed_line1[0] != decomposed_line2[0]) ||
            (decomposed_line1[1] != decomposed_line2[1]) ||
            (decomposed_line1[2] != decomposed_line2[2]) ||
            (decomposed_line1[0] != decomposed_line3[0]) ||
            (decomposed_line1[1] != decomposed_line3[1]) ||
            (decomposed_line1[2] != decomposed_line3[2])) {
            // return false;
            throw std::runtime_error("Asymmetric gr files");
        }

        if (std::strcmp(type.c_str(),"a") == 0) { //arc
            Edge e(std::stoul(decomposed_line1[1]),
                   std::stoul(decomposed_line1[2]),
                   {std::stoul(decomposed_line1[3]), std::stoul(decomposed_line2[3]), std::stoul(decomposed_line3[3])});
                   // {std::stod(decomposed_line1[3]), std::stod(decomposed_line2[3]), std::stod(decomposed_line3[3])});
            edges_out.push_back(e);
            max_node_num = std::max({max_node_num, e.source, e.target});
        }
    }
    graph_size = max_node_num;
    return true;
}

bool load_gr_files(std::string gr_file1, std::string gr_file2, std::vector<Edge> &edges_out, size_t &graph_size) {
    size_t          max_node_num = 0;
    std::ifstream   file1(gr_file1.c_str());
    std::ifstream   file2(gr_file2.c_str());

    if ((file1.is_open() == false) || (file2.is_open() == false)) {
        return false;
    }

    std::string line1, line2;
    while ((file1.eof() == false) && (file2.eof() == false)) {
        std::getline(file1, line1);
        std::getline(file2, line2);

        if ((line1 == "") || (line2 == "")) {
            break;
        }

        std::vector<std::string> decomposed_line1, decomposed_line2;
        string_split(line1, " ", decomposed_line1);
        string_split(line2, " ", decomposed_line2);

        std::string type = decomposed_line1[0];
        if ((std::strcmp(type.c_str(),"c") == 0) || (std::strcmp(type.c_str(),"p") == 0)) {
            continue; //comment or problem
        }

        if ((decomposed_line1[0] != decomposed_line2[0]) ||
            (decomposed_line1[1] != decomposed_line2[1]) ||
            (decomposed_line1[2] != decomposed_line2[2])) {
            return false;
        }

        if (std::strcmp(type.c_str(),"a") == 0) { //arc
            Edge e(std::stoul(decomposed_line1[1]),
                   std::stoul(decomposed_line1[2]),
                   {std::stoul(decomposed_line1[3]), std::stoul(decomposed_line2[3]), 0});
                   // {std::stod(decomposed_line1[3]), std::stod(decomposed_line2[3]), 0});
            edges_out.push_back(e);
            max_node_num = std::max({max_node_num, e.source, e.target});
        }
    }
    graph_size = max_node_num;
    return true;
}

bool load_txt_file(std::string txt_file, std::vector<Edge> &edges_out, size_t &graph_size) {
    bool            first_line = true;
    size_t          max_node_num = 0;
    std::ifstream   file(txt_file.c_str());

    if (file.is_open() == false) {
        return false;
    }

    std::string line;
    while (file.eof() == false) {
        std::getline(file, line);

        if (line == "") {
            break;
        }

        std::vector<std::string> decomposed_line;
        string_split(line, " ", decomposed_line);

        if (first_line) {
            first_line = false;
            continue;
        }
        Edge e(std::stoul(decomposed_line[0]),
               std::stoul(decomposed_line[1]),
               {std::stoul(decomposed_line[2]), std::stoul(decomposed_line[3]), 0});
               // {std::stod(decomposed_line[2]), std::stod(decomposed_line[3]), 0});
        edges_out.push_back(e);
        max_node_num = std::max({max_node_num, e.source, e.target});
    }
    graph_size = max_node_num;
    return true;
}

bool load_queries(std::string query_file, std::vector<std::pair<size_t, size_t>> &queries_out) {
    std::ifstream   file(query_file.c_str());

    if (file.is_open() == false) {
        return false;
    }

    std::string line;
    while (file.eof() == false) {
        std::getline(file, line);

        if (line == "") {
            break;
        }

        std::vector<std::string> decomposed_line;
        string_split(line, ",", decomposed_line);

        if (std::strcmp(decomposed_line[0].c_str(),"#") == 0) {
            continue;
        }

        std::pair<size_t, size_t> query = {std::stoul(decomposed_line[0]), std::stoul(decomposed_line[1])};
        queries_out.push_back(query);
    }
    return true;
}