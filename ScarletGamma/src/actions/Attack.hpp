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

        virtual void HandleActionInfo(uint8_t messageType, const std::string& message, uint8_t sender) override;
        virtual void HandleActionInfoResponse(uint8_t messageType, const std::string& message) override;

    private:
        void AttackRollPromptFinished(States::GameState* _gs);
        bool EvaluateAttackRoll(int _roll);
        void AttackRollInfoReceived(const std::string& _message, uint8_t _sender);
        void AttackRollInfoLocal(const std::string& _message);
        void SendAttackRollHit(uint8_t _sender);
        void SendAttackRollMissed(uint8_t _sender);
        void AttackRollHit();
        void AttackRollMissed();
        void HitRollPromptFinished(States::GameState* _gs);
        void HitRollInfoReceived(const std::string& _message, uint8_t _sender);
        void HitRollInfoLocal(const std::string& _message);
        void ApplyDamage();
        void BroadcastMissMessage(const std::string& _dice, int _result);
        void BroadcastHitMessage(const std::string& _dice, int _result);
        void BroadcastDamageMessage(const std::string& _dice, int _result);

        /// \brief Clones an attack action.
        virtual Action* Clone(Core::ObjectID _executor, Core::ObjectID target);

        /// \brief Private constructor, fills requirements list.
        Attack();
    };
}