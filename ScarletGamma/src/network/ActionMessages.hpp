#pragma once

#include <cstdint>
#include "Prerequisites.hpp"
#include <jofilelib.hpp>

namespace Network
{
    enum struct ActionMsgType: uint8_t
    {
        ACTION_BEGIN,  //< Begin an action, can be sent both ways
        ACTION_END,    //< End an action, can be sent both ways
        ACTION_INFO    //< Information about an action, to be handled by the Action object
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
    size_t HandleActionMessage(Core::ActionID _action, const uint8_t* _data, size_t _size, uint8_t _sender);

    ////////////////////////////////////////////////////////////////////////////

    /// \brief Send this message to start an action.
    class MsgActionBegin: public ActionMsg
    {
    public:
        MsgActionBegin(Core::ActionID id, Core::ObjectID target);

        /// \see HandleActionMessage
        static size_t Receive(Core::ActionID _action, uint8_t _sender, const uint8_t* _data, size_t _size);
    protected:
        const Core::ObjectID m_target;
        virtual void WriteData(Jo::Files::MemFile& _output) const override;
    };

    ////////////////////////////////////////////////////////////////////////////

    /// \brief Send this message to end an action.
    class MsgActionEnd: public ActionMsg
    {
    public:
        MsgActionEnd(Core::ActionID id);

        /// \see HandleActionMessage
        static size_t Receive(Core::ActionID _action, uint8_t _sender, const uint8_t* _data, size_t _size);
    protected:
        virtual void WriteData(Jo::Files::MemFile& _output) const override {};
    };

    ////////////////////////////////////////////////////////////////////////////

    /// \brief Send this message to end an action.
    class MsgActionInfo: public ActionMsg
    {
    public:
        MsgActionInfo(Core::ActionID id, uint8_t messageType, const std::string& message);

        /// \see HandleActionMessage
        static size_t Receive(Core::ActionID _action, uint8_t _sender, const uint8_t* _data, size_t _size);
    protected:
        const uint8_t m_messageType;
        const std::string m_message;
        virtual void WriteData(Jo::Files::MemFile& _output) const override;
    };
}