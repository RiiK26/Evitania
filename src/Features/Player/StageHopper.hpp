#pragma once
#include <string>

namespace Features
{
  namespace StageHopper
  {
    void Initialize();
    void Uninitialize();

    void HopToStage(const std::string& stageName);
    void HopOnce();
    void Update();

    std::string GetCurrentStage();
    void        SetCurrentStage(const std::string& stageName);
    bool        IsLoopActive();
    bool        IsValidStage(const std::string& stageName);
  }  // namespace StageHopper
}  // namespace Features
