#pragma once

#include "Action.hpp"
#include "Prerequisites.hpp"

namespace Actions
{
    class Attack : public Action
    {
        friend class ActionPool;

    private:
        /// \brief asdasd
        enum struct ActionMsgType: uint8_t
        {
            PL_TARGET, 
            PL_ATTACK_ROLL_INFO,
            DM_ATTACK_ROLL_MISS,
            DM_ATTACK_ROLL_HIT,
            DM_ATTACK_ROLL_CRIT,
            PL_HIT_ROLL_INFO,
            PL_CRIT_ROLL_INFO
        };

    public:
        /// \brief Initialize an attack action.
        virtual void Execute();

        /// \brief Handles the various network messages.
        virtual void HandleActionInfo(uint8_t messageType, const std::string& message) override;

    private:
        void AttackRollPromptFinished(States::GameState* gs);
        void AttackRollInfoReceived(const std::string& message);
        void AttackRollMissed();
        void AttackRollHit();
        void HitRollPromptFinished(States::GameState* gs);
        void HitRollInfoReceived(const std::string& message);

        /// \brief Clones an attack action.
        virtual Action* Clone(Core::ObjectID target);

        /// \brief Private constructor, fills requirements list.
        Attack();
    };
}