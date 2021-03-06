#pragma once

#include "Action.hpp"
#include "Prerequisites.hpp"

namespace Actions
{
    class Attack : public Action
    {
        friend class ActionPool;

    private:
        /// \brief Messages that can be sent/received during an attack action
        enum struct AttackMsgType: uint8_t
        {
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
		void AttackRollPromptFinished(const std::string& _result);
        bool EvaluateAttackRoll(int _roll);
        void AttackRollInfoReceived(const std::string& _message);
        void AttackRollInfoLocal(const std::string& _message);
		void PushAttackRollDMPrompt(int _result, void (Attack::* _callback)(const std::string&));
		void AttackRollDMPromptFinished(const std::string& _result);
		void AttackRollDMPromptFinishedLocal(const std::string& _result);
        void SendAttackRollHit();
        void SendAttackRollMissed();
        void AttackRollHit();
        void AttackRollMissed();
		void HitRollPromptFinished(const std::string& _result);
        void HitRollInfoReceived(const std::string& _message);
        void HitRollInfoLocal(const std::string& _message);
		void PushHitRollDMPrompt(int _result, void (Attack::* _callback)(const std::string&));
		void HitRollDMPromptFinished(const std::string& _result);
		void HitRollDMPromptFinishedLocal(const std::string& _result);
        void BroadcastMissMessage(const std::string& _dice, int _result);
        void BroadcastHitMessage(const std::string& _dice, int _result);
        void BroadcastDamageMessage(const std::string& _dice, int _result);

        /// \brief Action can be used only in combat, so we need to overwrite CanUse.
        virtual bool CanUse(Core::ObjectList& _executors, Core::Object& _object) override;

        /// \brief Clones an attack action.
        virtual Action* Clone(Core::ObjectID _executor, Core::ObjectID target);

        /// \brief Private constructor, fills requirements list.
        Attack();

        /// \brief Saves the attack roll entered by the player
        std::string m_attackRoll;

        /// \brief Saves the hit roll entered by the player
        std::string m_hitRoll;

        /// \brief Saves the sender's socket ID
        uint8_t m_sender;

		static std::unordered_map<Core::ObjectID, std::string> m_attackRollCache;
		static std::unordered_map<Core::ObjectID, std::string> m_hitRollCache;
    };
}