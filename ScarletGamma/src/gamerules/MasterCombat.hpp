#pragma once

#include "Combat.hpp"
#include <unordered_map>

namespace GameRules
{
    class MasterCombat : public Combat
    {
    public:
        void ReceivedInitiative(Core::ObjectID _object, std::string& _initiative);

    protected:
        /// \brief Keeps the initiative for each combat participant.
        std::unordered_map<Core::ObjectID, int> m_initiatives;
    };
}
