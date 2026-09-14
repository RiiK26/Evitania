#pragma once
#include <mutex>
#include "imgui.h"

namespace Menu
{
  class Logger
  {
  public:
    static void Clear();
    static void Log(const char* fmt, ...);
    static void Draw();

  private:
    static ImGuiTextBuffer Buf;
    static ImGuiTextFilter Filter;
    static ImVector<int>   LineOffsets;
    static bool            AutoScroll;
    static std::mutex      LogMutex;
    static bool            Initialized;
  };
}  // namespace Menu
