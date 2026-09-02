#include "FreeCrafting.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace FreeCrafting
  {
    bool (*Orig_SpendableVisitor_Spend)(void* __this, void* currency, float amount, void* method_info);

    bool Hook_SpendableVisitor_Spend(void* __this, void* currency, float amount, void* method_info)
    {
      if (Menu::Config.bFreeCrafting)
        return true;
      return Orig_SpendableVisitor_Spend(__this, currency, amount, method_info);
    }

    void Initialize()
    {
      HOOK_METHOD("SpendableVisitor", "Spend", 2, Hook_SpendableVisitor_Spend, Orig_SpendableVisitor_Spend);
    }

    void Uninitialize() { }
  }  // namespace FreeCrafting
}  // namespace Features
