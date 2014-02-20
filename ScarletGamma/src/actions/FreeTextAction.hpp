#pragma once

#include "actions/Action.hpp"

namespace Actions
{
    class FreeTextAction : public Action
    {
    private:
        /// \brief Messages that can be sent/received during an attack action
        enum struct FreeTextMsgType: uint8_t
        {
            PL_FREETEXT, ///< sends the free text that was entered
        };

    public:
        /// \brief Initializes the base class
        FreeTextAction();

    protected:
        static std::string m_valueCache; ///< remembers the last entered value

        /// \brief Sends the user input to the DM.
        void FreeTextPromptFinished(const std::string& _result);

        /// \brief Server-side: handles player's requests.
        void HandleActionInfo(uint8_t _messageType, const std::string& _message,
            uint8_t _sender) override;

        /// \brief Do whatever the action does at startup.
        virtual void Execute() override;
        
        /// \brief Can only be used by players.
        virtual bool CanUse(Core::ObjectList& _executors, Core::Object& _object) override;

        /// \brief Create a copy of the action. 
        virtual Action* Clone(Core::ObjectID _executor, Core::ObjectID _target) override;
    };
}