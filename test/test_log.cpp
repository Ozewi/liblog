/**
 * @package   liblog: A small C++ logging library.
 * @brief     Tests
 * @author    José Luis Sánchez Arroyo
 * @section   License
 * Copyright (c) 2011-2025 José Luis Sánchez Arroyo
 * This software is distributed under the terms of the BSD 3-Clause License and comes WITHOUT ANY WARRANTY.
 * Please read the file LICENSE for further details.
 */

#include "log.h"
#include <iostream>
#include <cstdint>

void write_some_log()
{
    Logger(Log::Debug) << "This is a debug entry." << Log::end;
    Logger(Log::Info) << "An Info entry here." << Log::end;
    Logger(Log::Warning) << "Warning of something." << Log::end;
    Logger(Log::Error) << "This is an error." << Log::end;
}

void test_levels()
{
    for (auto level = Log::MinValue; level <= Log::MaxValue; level = static_cast<Log::Verbosity>(static_cast<int>(level) + 1))
    {
        std::cout << "Setting reporting level to '" << Log::GetLevelName(level) << "'\n";
        Log::SetReportingLevel(level);
        write_some_log();
        std::cout << std::endl;
    }
}

void test_bad_level()
{
    try
    {
        Log::SetReportingLevel(static_cast<Log::Verbosity>(99));
    }
    catch(const std::exception& e)
    {
        std::cout << "Exception catched: " << e.what() << std::endl;
    }
    Log::SetReportingLevel(Log::Debug);
}

void test_memdump()
{
    uint8_t buffer[] = { 3,20,21,146,101,53,137,121,50,56,70 };
    Logger(Log::Debug) << "PI value is: " << Log::MemDump(buffer, sizeof(buffer)) << Log::end;
}

void test_timestamp()
{
    Log::SetTimestampFormat(Log::BootTime_HiRes);
    write_some_log();
}

void showTitle(const std::string& title)
{
    std::cout << "----------\n---Testing " << title << "---\n";
}


int main()
{
    showTitle("levels");
    test_levels();
    showTitle("bad level");
    test_bad_level();
    showTitle("memdump");
    test_memdump();
    showTitle("timestamp");
    test_timestamp();
}
