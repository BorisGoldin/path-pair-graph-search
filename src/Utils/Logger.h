#ifndef UTILS_LOGGER_H
#define UTILS_LOGGER_H

#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <chrono>
#include "Definitions.h"

#if (DEBUG >= 1)
#define LOG_START_SEARCH(logger, source, target, message)   ((logger).start_search((source), (target), (message)));
#define LOG_EVENT(logger, message)                          ((logger).log_event((message)));
#define LOG_FINISH_SEARCH(logger, message)                  ((logger).finish_search((message)));
#else
#define LOG_START_SEARCH(logger, source, target, message)   ;
#define LOG_EVENT(logger, message)                          ;
#define LOG_FINISH_SEARCH(logger, message)                  ;
#endif

using Clock     = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

class Logger;
using LoggerPtr = std::shared_ptr<Logger>;

class Logger
{
private:
    TimePoint       run_start_time;
    long int        total_run_duration_ms           = 0;
    long int        run_time_since_previous_log_ms  = 0;
    std::ofstream   log_filestream;

public:
    Logger(std::string filename);
    ~Logger();
    void start_search(Idx source, Idx target, std::string search_info_json);
    void log_event(std::string message);
    void finish_search(std::string finish_info_json);
};

#endif // UTILS_LOGGER_H