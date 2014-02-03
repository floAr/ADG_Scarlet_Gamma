#pragma once

#include "Combat.hpp"

namespace GameRules
{
    class MasterCombat : public Combat
    {
    public:
        void ReceivedInitiative(Core::ObjectID _object, std::string& _initiative);
    };
}
