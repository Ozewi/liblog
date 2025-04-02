/**
 * @package   liblog: A small C++ logging library.
 * @brief     Main header file.
 * @author    José Luis Sánchez Arroyo
 * @section   License
 * Copyright (c) 2011-2025 José Luis Sánchez Arroyo
 * This software is distributed under the terms of the BSD 3-Clause License and comes WITHOUT ANY WARRANTY.
 * Please read the file LICENSE for further details.
 */

#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>
#include <fstream>
#include <string>

/**
 * @brief     General log macro. Use it as an output stream.
 * @details   According to the current verbosity level, a true stream or a null stream is returned.
 */
#define Logger(level) Log::GetStream(level, __FILE__, __LINE__, __FUNCTION__)

namespace Log {

/** ----------------------------------------------------
 * @brief     Auxilliary type definitions.
 * ------ */

enum Verbosity                                          /** @brief  Verbosity levels. */
{
    Error,                                              //!< Unrecoverable error. The application should stop after printing the log.
    Warning,                                            //!< Anomalous condition, recoverable.
    Info,                                               //!< Informative remark. Not an error.
    Debug,                                              //!< Debugging info.
    MinValue = Error,                                   //!< Min and max values.
    MaxValue = Debug
};

enum TimeFormat                                         /** @brief  Time formats in timestamps. */
{
    BootTime          = 0x0001,                         //!< Time since system bootup (CLOCK_BOOTTIME), in seconds and milliseconds (10^-3 s).
    BootTime_HiRes    = 0x1001,                         //!< Time since system bootup (CLOCK_BOOTTIME), in seconds and microseconds (10^-6 s).
    TimeOfDay         = 0x0002,                         //!< System clock daytime (CLOCK_REALTIME), date and time up to the second.
    TimeOfDay_HiRes   = 0x1002,                         //!< System clock daytime (CLOCK_REALTIME), date and time up to the millisecond.

    HiRes             = 0x1000                          //!< Helper to know if a time format implies high resolution clock or not.
};

enum OpenMode                                           /** @brief  Open modes. @see ToFile */
{
    Append,                                             //!< Open file for append. New traces are added at the end of the file.
    Truncate                                            //!< Truncate file on open.
};

/** ----------------------------------------------------
 * @brief     Class MemDump: Dump memory buffers to log
 * ------ */
class MemDump
{
public:
    /**
     * @brief   Constructor.
     */
    MemDump (
        const void* p_buffer,                           //!< Pointer to the data to serialize
        size_t p_length                                 //!< Length of data to show
    ) : buffer_(reinterpret_cast<const unsigned char*>(p_buffer)), length_(p_length) {};

private:
    const unsigned char* buffer_;                       //!< Pointer to the data block
    size_t length_;                                     //!< Size of the data block

    /**
     * @brief   Injection operator.
     */
    friend
    std::ostream&                                       //!< Log output stream
    operator << (
        std::ostream& os,                               //!< Log output stream
        const MemDump& md                               //!< Object to serialize
    );

    /**
     * @brief   Dumping function.
     */
    std::ostream&                                       //!< Log output stream
    doDump(
        std::ostream& os                                //!< Log output stream
    ) const;
};

/** ----------------------------------------------------
 * @brief     Log management functions
 * ------ */

/**
 * @brief     Set the output log file.
 * @details   Use this function to redirect log output to a file.
 *            The output file is created if it didn't existed.
 * @throws    std::ios_base::failure if the file can't be opened / created.
 */
void
ToFile (
    const std::string& fname,                           //!< Name of the new output file.
    OpenMode mode = Log::Append                         //!< Open mode. @see OpenMode
);

/**
 * @brief     Set the minimum reporting level of a trace for being reported.
 * @throws    std::invalid_argument if the argument is out of range.
 */
void
SetReportingLevel (
    Verbosity p_level                                   //!< New reporting level.
);

/**
 * @brief     Get the current reporting level.
 */
Verbosity                                               /** @return Current reporting level. */
GetReportingLevel ();

/**
 * @brief     Get the name of a given reporting level.
 * @throws    std::invalid_argument if the argument is out of range.
 */
const char*                                             /** @return Level name */
GetLevelName(
    Verbosity level                                     //!< Level to 'translate'
);

/**
 * @brief     Set whether a timestamp should be added to each trace or not.
 */
void
ShowTimestamp (
    bool show                                           //!< true: Add timestamps.\n false: Don't add.
);

/**
 * @brief     Set the print format of the timestamp.
 */
void
SetTimestampFormat (
    TimeFormat format                                   //!< Timestamp format code.
);

/**
 * @brief     Set serialization mode.
 */
void
SetSerialization (
    bool mode                                           //!< true: Serialize output.\n false: Don't serialize.
);

/**
 * @brief     Get the output stream.
 * @details   Context is added before providing the stream.
 * @note      Don't use this function directly; instead, use the Logger() macro.
 */
std::ostream&                                           /** @return Log output stream */
GetStream (
    Verbosity p_level,                                  //!< Severity level of the trace.
    const char* p_file,                                 //!< Source file, provided by the __FILE__ predefined macro.
    int         p_line,                                 //!< Line of the source file, provided by the __LINE__ predefined macro.
    const char* p_func                                  //!< Function or function member, provided by the __FUNCTION__ variable.
);

/**
 * @brief     Finish a trace.
 * @details   A newline character is added at the end; then, the output is flushed.
 */
std::ostream&                                           /** @brief Log output stream. */
end (
    std::ostream& os                                    //!< Log output stream.
);

/**
 * @brief     Finish a trace and exit the program.
 */
[[noreturn]] std::ostream&
terminate (
    std::ostream& os
);

} // namespace Log

#endif // _LOG_H_
