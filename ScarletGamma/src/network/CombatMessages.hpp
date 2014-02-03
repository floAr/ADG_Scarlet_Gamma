#pragma once

#include <cstdint>
#include "Prerequisites.hpp"
#include <jofilelib.hpp>

namespace Network
{
    enum struct CombatMsgType: uint8_t
    {
        DM_COMBAT_BEGIN,         ///< DM begins combat, void
        DM_COMBAT_INITIATIVE,    ///< DM requests initiative roll, void
        PL_COMBAT_INITIATIVE,    ///< Player answer initiative request, int
        /* ... */
        DM_COMBAT_END,           ///< DM ends combat, void
    };

    /// \brief A base class for all combat related messages.
    class CombatMsg
    {
    public:
        /// \brief Writes the headers and the specific data to a packet and
        ///		sends it.
        void Send(uint8_t socket = 0);

    private:
        CombatMsgType m_purpose;

    protected:
        CombatMsg(CombatMsgType _purpose)
            : m_purpose(_purpose) {}

        /// \brief Write data of a specific message to the packet.
        virtual void WriteData(Jo::Files::MemFile& _output) const = 0;
    };

    /// \brief Handle a message with target Master or Player.
    /// \details The function is called after the MessageHeader header is
    ///		decoded. So data starts with MsgActionHeader.
    ///	\return Number of bytes which were read: could be smaller than _size.
    size_t HandleCombatMessage(const uint8_t* _data, size_t _size, uint8_t _sender);
}