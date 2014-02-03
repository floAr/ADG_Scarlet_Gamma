#include "CombatMessages.hpp"
#include "Message.hpp"
#include "Messenger.hpp"
#include "Game.hpp"
#include "states/CommonState.hpp"
#include "states/MasterState.hpp"
#include "states/PlayerState.hpp"
#include <cassert>

using namespace Network;

size_t Network::HandleCombatMessage(const uint8_t* _data, size_t _size, uint8_t _sender)
{
    assert(_size >= sizeof(CombatMsgType));

    const CombatMsgType* header = reinterpret_cast<const CombatMsgType*>(_data);
    size_t readSize = sizeof(CombatMsgType);

    switch(*header)
    {
    case CombatMsgType::DM_COMBAT_BEGIN:
        assert(Messenger::IsServer() == false);
        readSize += _size; // read 'em all
        g_Game->GetCommonState()->BeginCombat();
        break;
    case CombatMsgType::DM_COMBAT_END:
        assert(Messenger::IsServer() == false);
        readSize += _size; // read 'em all
        break;
    }

    return readSize;
}

////////////////////////////////////////////////////////////////////////////////
// CombatMsg

void CombatMsg::Send(uint8_t sender)
{
    // Write headers
    Jo::Files::MemFile data;
    data.Write(&MessageHeader(Target::COMBAT, 0), sizeof(MessageHeader));
    data.Write(&m_purpose, sizeof(CombatMsgType));

    // Write data
    WriteData(data);
    Messenger::Send(data.GetBuffer(), (size_t)data.GetSize(), Network::Messenger::GetSocket(sender));
}