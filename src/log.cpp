/**
 * @package   liblog: A small C++ logging library.
 * @brief     Main file.
 * @author    José Luis Sánchez Arroyo
 * @section   License
 * Copyright (c) 2011-2025 José Luis Sánchez Arroyo
 * This software is distributed under the terms of the BSD 3-Clause License and comes WITHOUT ANY WARRANTY.
 * Please read the file LICENSE for further details.
 */

#include "log.h"
#include <iostream>
#include <iomanip>
#include <mutex>
#include <time.h>               // clock_gettime
#include <string.h>             // strrchr

namespace Log {

/** ----------------------------------------------------
 * @brief     Variables local to the module
 * ------ */

static std::ostream* outLog_ = &std::cerr;              // Output stream. Default: std::cerr alias stderr.
static Verbosity reportingLevel_ = Error;               // Reporting level. Default: Error.
static bool show_timestamp_ = true;                     // Add timestamp to logs. Default: true.
static TimeFormat timestamp_format_ = TimeOfDay;        // Timestamp format. Default: time of day.
static bool serialize_output_ = false;                  // Serialize output with a mutex acquired in GetStream() and released in end(). Default: false.
static std::timed_mutex out_mutex_;                     // The mutex for serializing.
static constexpr unsigned MUTEX_TIMEOUT_ = 5;           // Timeout for acquiring the serialization mutex, in seconds.

static const char* logNames[] = { "ERROR", "Warn", "Info", "Debug" };

/** ----------------------------------------------------
 * @brief     Class nullstream: An output stream that writes to nowhere.
 * ------ */
struct nullstream : std::ostream
{
    nullstream() : std::ios(0), std::ostream(0) {};
};

/**
 * @brief     Overloaded "inject" friend operator for class nullstream that outputs nothing.
 */
template <typename T>
std::ostream& operator << (nullstream& os, T)
{
    return os;
};

/** ----------------------------------------------------
 * @brief     Class MemDump: Dump memory buffers to log
 * ------ */

/**
 * @brief     Injection operator
 */
std::ostream& operator << (std::ostream& os, const MemDump& md)
{
    return md.doDump(os);
}

/**
 * @brief     Dumping function
 */
std::ostream& MemDump::doDump(std::ostream& os) const
{
    std::ofstream save;                                 // Save previous state
    save.copyfmt(os);
    os << std::hex << std::right << std::setfill('0');

    for (size_t ix = 0; ix < length_; ++ix)
        os << ' ' << std::setw(2) << static_cast<unsigned>(buffer_[ix]);
    os.copyfmt(save);                                   // Restore previous state
    return os;
}

/** ----------------------------------------------------
 * @brief     Public functions of the module
 * ------ */

/**
 * @brief     Set the output log file.
 */
void ToFile(const std::string& file, OpenMode mode)
{
    auto openmode = (mode == Log::Append)? std::ios::app : std::ios::trunc;
    std::ofstream* ofs = new std::ofstream(file, std::ios::out | openmode);
    ofs->exceptions(std::ios_base::badbit | std::ios_base::badbit);   // If there was an error opening the file, an exception should be thrown here.

    if (outLog_ != &std::cerr && outLog_ != &std::cout)   // Delete the previous stream only if it wasn't a standard one.
        delete outLog_;
    outLog_ = ofs;
}

/**
 * @brief     Set the minimum reporting level of a trace for being reported.
 */
void SetReportingLevel(Verbosity p_level)
{
    if (p_level < MinValue || p_level > MaxValue)
        throw std::invalid_argument("Invalid verbosity level");
    const char* oldLevel = logNames[reportingLevel_];
    reportingLevel_ = p_level;
    Logger(Info) << "Log level changed from " << oldLevel << " to " << logNames[p_level] << end;
}

/**
 * @brief     Gets the current reporting level.
 */
Verbosity GetReportingLevel()
{
    return reportingLevel_;
}

/**
 * @brief     Get the name of a given reporting level.
 */
const char* GetLevelName(Verbosity level)
{
    if (level < MinValue || level > MaxValue)
        throw std::invalid_argument("Invalid verbosity level");
    return logNames[level];
}

/**
 * @brief     Sets whether a timestamp should be added to each trace or not.
 */
void ShowTimestamp(bool show)
{
    show_timestamp_ = show;
}

/**
 * @brief     Sets the print format of the timestamp.
 */
void SetTimestampFormat(TimeFormat format)
{
    timestamp_format_ = format;
}

/**
 * @brief     Sets serialization mode.
 */
void SetSerialization(bool mode)
{
    serialize_output_ = mode;
    if (!mode)
        out_mutex_.unlock();
}

/**
 * @brief     Get the output stream.
 */
std::ostream& GetStream(Verbosity p_level, const char* p_file, int p_line, const char* p_func)
{
    static nullstream cnull;

    if (p_level > reportingLevel_)
        return cnull;

    const char* fname = strrchr(p_file, '/');           // Remove the path from the filename
    if (fname == nullptr)
        fname = p_file;
    else
        fname++;                                        // including the slash

    if (show_timestamp_)
    {
        clockid_t clk = (timestamp_format_ & BootTime)? CLOCK_BOOTTIME : CLOCK_REALTIME;
        timespec now;
        clock_gettime(clk, &now);

        if (serialize_output_)
        {
            if (out_mutex_.try_lock_for(std::chrono::seconds(MUTEX_TIMEOUT_)) == false)
            {
                *outLog_ << "--- WARNING --- Timeout waiting for log serialization mutex. Disabling serialization...";
                serialize_output_ = false;
                out_mutex_.unlock();
            }
        }
        if (timestamp_format_ & BootTime)
            *outLog_ << std::setfill('0') << std::setw(7) << now.tv_sec % 10000000 << "." << std::setw((timestamp_format_ & HiRes)? 6 : 3) << now.tv_nsec / ((timestamp_format_ & HiRes)? 1000 : 1000000);
        else if (timestamp_format_ & TimeOfDay)
        {
            auto timeinfo = gmtime(&now.tv_sec);
            *outLog_ << std::put_time(timeinfo, "%d.%m.%Y %T");
            if (timestamp_format_ & HiRes)
                *outLog_ << "." << std::setfill('0') <<  std::setw(3) << now.tv_nsec / 1000000;
        }
        *outLog_ << " | ";
    }

    *outLog_ << std::left << std::setw(5) << logNames[p_level] << " | " << fname << ":" << p_line << " (" << p_func << ") | ";
    return *outLog_;
}

/**
 * @brief     Finish a trace.
 */
std::ostream& end(std::ostream& os)
{
    os << std::endl << std::flush;
    if (serialize_output_)
        out_mutex_.unlock();
    return os;
}

/**
 * @brief     Finish a trace and exit the program.
 */
[[noreturn]] std::ostream& terminate(std::ostream& os)
{
    os << std::endl << std::flush;
    if (serialize_output_)
        out_mutex_.unlock();
    exit(-1);
}

} // namespace Log
