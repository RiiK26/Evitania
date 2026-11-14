/*
 * MIT License
 *
 * Copyright © 2026 - Present RiiK
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#ifdef _DEBUG

  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #include <windows.h>

  #include <cstdarg>
  #include <cstdio>
  #include <ctime>
  #include <mutex>

namespace Utils::Logger
{
  inline std::mutex& GetMutex()
  {
    static std::mutex mtx;
    return mtx;
  }

  inline FILE*& GetFile()
  {
    static FILE* fp = nullptr;
    return fp;
  }

  inline void Log(const char* level, const char* fmt, ...)
  {
    std::lock_guard<std::mutex> lock(GetMutex());

    FILE*& fp = GetFile();
    if (!fp) {
      fp = std::fopen("EvitaniaDebug.log", "a");
    }

    char    msg[1024];
    va_list args;
    va_start(args, fmt);
    std::vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    std::time_t now = std::time(nullptr);
    std::tm     tmNow{};
    localtime_s(&tmNow, &now);

    char line[1152];
    std::snprintf(
      line, sizeof(line), "[%02d:%02d:%02d][%s] %s\n", tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec, level, msg
    );

    OutputDebugStringA(line);
    if (fp) {
      std::fputs(line, fp);
      std::fflush(fp);
    }
  }
}  // namespace Utils::Logger

  #define LOG_DEBUG(fmt, ...) ::Utils::Logger::Log("DEBUG", fmt, ##__VA_ARGS__)
  #define LOG_INFO(fmt, ...)  ::Utils::Logger::Log("INFO", fmt, ##__VA_ARGS__)
  #define LOG_WARN(fmt, ...)  ::Utils::Logger::Log("WARN", fmt, ##__VA_ARGS__)
  #define LOG_ERROR(fmt, ...) ::Utils::Logger::Log("ERROR", fmt, ##__VA_ARGS__)

#else

  #define LOG_DEBUG(fmt, ...) ((void) 0)
  #define LOG_INFO(fmt, ...)  ((void) 0)
  #define LOG_WARN(fmt, ...)  ((void) 0)
  #define LOG_ERROR(fmt, ...) ((void) 0)

#endif
