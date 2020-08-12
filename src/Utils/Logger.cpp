#include <string>
#include <fstream>
#include <chrono>
#include "Logger.h"

std::string insert_tabs_on_newline(std::string str, size_t tabs_amount) {
    std::string replace = "\n";
    while (tabs_amount-- != 0) {
        replace += "\t";
    }

    size_t pos = 0;
    while ((pos = str.find("\n", pos)) != std::string::npos) {
        str.replace(pos, 1, replace);
        pos += replace.length();
    }
    return str;
}

Logger::Logger(std::string filename): log_filestream(filename.c_str(), std::ofstream::out | std::ofstream::app) {
    this->run_start_time = Clock::now();
};

Logger::~Logger() {
    this->log_filestream.close();
};

// Log file is in the format:
//
// Mon Aug  3 15:33:25 2020
// Source: 3, Target: 4,  message
// 4 | 4 | this is message 1
// 5 | 1 | this is message 2

void Logger::start_search(Idx source, Idx target, std::string search_info_json) {
    std::time_t start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    this->run_start_time = Clock::now();
    this->total_run_duration_ms = 0;
    this->run_time_since_previous_log_ms = 0;
    std::string string_start_time = std::ctime(&start_time);
    string_start_time.pop_back();

    this->log_filestream << "{\n"
                         <<     "\t\"start_time\": \"" << string_start_time << "\",\n"
                         <<     "\t\"source\": " << source << ",\n"
                         <<     "\t\"target\": " << target << ",\n"
                         <<     "\t\"search_info\": " << insert_tabs_on_newline(search_info_json,2) << ",\n"
                         <<     "\t\"events\": [" << std::endl;
};

void Logger::log_event(std::string message) {
    this->run_time_since_previous_log_ms = this->total_run_duration_ms;
    this->total_run_duration_ms = 
        std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - this->run_start_time).count();
    this->run_time_since_previous_log_ms =
        this->total_run_duration_ms - this->run_time_since_previous_log_ms;

    this->log_filestream <<     "\t\t{"
                         <<     "\t\t\t\"time_elapsed\": " << this->total_run_duration_ms << ",\n"
                         <<     "\t\t\t\"time_since_last_event\": " << this->run_time_since_previous_log_ms << ",\n"
                         <<     "\t\t\t\"event_data\": \"" << message << "\"\n"
                         <<     "\t\t}," << std::endl;
};

void Logger::finish_search(std::string finish_info_json) {
    long int total_runtime_ms = 
        std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - this->run_start_time).count();
    this->log_filestream <<     "\t],\n"
                         <<     "\t\"finish_info\": " << insert_tabs_on_newline(finish_info_json,2) << ",\n"
                         <<     "\t\"total_runtime\": " << total_runtime_ms << "\n"
                         << "}\n" << std::endl;
}