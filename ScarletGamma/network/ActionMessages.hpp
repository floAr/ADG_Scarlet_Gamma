#pragma once

#include <cstdint>
#include "Prerequisites.hpp"
#include <jofilelib.hpp>

namespace Network
{
    enum struct ActionMsgType: uint8_t
    {
        // TODO
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
        ActionMsg(ActionMsgType _purpose, Core::ActionID) : m_purpose(_purpose) {}

        /// \brief Write data of a specific message to the packet.
        virtual void WriteData( Jo::Files::MemFile& _output ) const = 0;
    };
}