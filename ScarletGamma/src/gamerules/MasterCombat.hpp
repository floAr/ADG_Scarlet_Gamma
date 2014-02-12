#pragma once

#include "Combat.hpp"
#include <unordered_map>

namespace GameRules
{
    class MasterCombat : public Combat
    {
    public:
        void AddParticipant(Core::ObjectID _object);
        void ReceivedInitiative(Core::ObjectID _object, std::string& _initiative);
        virtual void InitiativeRollPromptFinished( States::GameState* _ps, Core::ObjectID _object ) override;
        void SetTurn(Core::ObjectID _object);

    protected:
        /// \brief Keeps the initiative for each combat participant.
        std::unordered_map<Core::ObjectID, int> m_initiatives;
    };
}
