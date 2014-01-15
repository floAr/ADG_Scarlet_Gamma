#pragma once

#include <cstdint>
#include "Prerequisites.hpp"
#include <jofilelib.hpp>

namespace Network
{
    enum struct ActionMsgType: uint8_t
    {
        BEGIN_ACTION,  //< Begin an action, can be sent both ways
        END_ACTION     //< End an action, can be sent both ways
    };

    /// \brief A base class for all action related messages.
    class ActionMsg
    {
    public:
        /// \brief Writes the headers and the specific data to a packet and
        ///		sends it.
        void Send();

    private:
        ActionMsgType m_purpose;
        Core::ActionID m_action;

    protected:
        ActionMsg(ActionMsgType _purpose, Core::ActionID _action)
            : m_purpose(_purpose), m_action(_action) {}

        /// \brief Write data of a specific message to the packet.
        virtual void WriteData(Jo::Files::MemFile& _output) const = 0;
    };

    /// \brief Handle a message with target Master or Player.
    /// \details The function is called after the MessageHeader header is
    ///		decoded. So data starts with MsgActionHeader.
    ///	\return Number of bytes which were read: could be smaller than _size.
    size_t HandleActionMessage(Core::ActionID _action, const uint8_t* _data, size_t _size);


    /// \brief Send this message to start an action.
    class MsgBeginAction: public ActionMsg
    {
    public:
        MsgBeginAction(Core::ActionID id);

        /// \see HandleActionMessage
        static size_t Receive(Core::ActionID _action, const uint8_t* _data, size_t _size);
    protected:
        virtual void WriteData(Jo::Files::MemFile& _output) const override;
    };
}