#pragma once

#include <cstdint>
#include "Prerequisites.hpp"
#include <jofilelib.hpp>

namespace Network
{
    enum struct CombatMsgType: uint8_t
    {
        DM_COMBAT_BEGIN,               ///< DM begins combat
        PL_COMBAT_INITIATIVE,          ///< Player provides initiative roll
        DM_COMBAT_ADD_PARTICIPANT,     ///< DM tells player about participants, in order of initiative
        /* ... */
        DM_COMBAT_END,                 ///< DM ends combat, void
    };

    /// \brief A base class for all combat related messages.
    class CombatMsg
    {
    public:
        /// \brief Writes the headers and the specific data to a packet and
        ///     sends it.
        /// \param [in] _suffix  Data to append to the packet, may be omitted
        /// \param [in] _socket  Socket to send the packet to, may be omitted
        void Send(Jo::Files::MemFile* _suffix = 0, uint8_t _socket = 0);

    private:
        CombatMsgType m_purpose;

    protected:
        CombatMsg(CombatMsgType _purpose)
            : m_purpose(_purpose) {}
    };

    /// \brief Handle a message with target Master or Player.
    /// \details The function is called after the MessageHeader header is
    ///		decoded. So data starts with MsgActionHeader.
    ///	\return Number of bytes which were read: could be smaller than _size.
    size_t HandleCombatMessage(const uint8_t* _data, size_t _size, uint8_t _sender);
}
