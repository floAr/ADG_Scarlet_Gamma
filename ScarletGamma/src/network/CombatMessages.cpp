#include "CombatMessages.hpp"
#include "Message.hpp"
#include "Messenger.hpp"
#include "Game.hpp"
#include "states/CommonState.hpp"
#include "states/MasterState.hpp"
#include "states/PlayerState.hpp"
#include "gamerules/Combat.hpp"
#include "gamerules/MasterCombat.hpp"
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
        g_Game->GetCommonState()->BeginCombat();
        break;

    case CombatMsgType::PL_COMBAT_INITIATIVE:
        {
            // Get master combat
            GameRules::MasterCombat* mc = static_cast<GameRules::MasterCombat*>(
                g_Game->GetCommonState()->GetCombat());
            assert(mc);

            // Read object ID
            Core::ObjectID object = *reinterpret_cast<const Core::ObjectID*>( _data + readSize );
            readSize += sizeof(Core::ObjectID);

            // Read initiative as string
            std::string initiative = std::string(*reinterpret_cast<const char*>( _data + readSize ),
                _size - readSize);
            readSize += sizeof(initiative.length());

            // Process information
            mc->ReceivedInitiative(object, initiative);
        } break;

    case CombatMsgType::DM_COMBAT_ADD_PARTICIPANT:
        {
            // Read object ID
            Core::ObjectID object = *reinterpret_cast<const Core::ObjectID*>( _data + readSize );
            readSize += sizeof(Core::ObjectID);

            // Try to read index, otherwise use -1 (back of list)
            int8_t index = -1;
            if (_size > readSize)
            {
                index = *reinterpret_cast<const int8_t*>( _data + readSize );
                readSize += sizeof(int8_t);
            }

            // Add participant to combat
            g_Game->GetCommonState()->GetCombat()->AddParticipant(object, index);
        } break;

    case CombatMsgType::DM_COMBAT_END:
        g_Game->GetCommonState()->EndCombat();
        break;

    }

    return readSize;
}

////////////////////////////////////////////////////////////////////////////////
// CombatMsg

void CombatMsg::Send(Jo::Files::MemFile* _suffix, uint8_t _socket)
{
    // Write headers
    Jo::Files::MemFile data;
    data.Write(&MessageHeader(Target::COMBAT, 0), sizeof(MessageHeader));
    data.Write(&m_purpose, sizeof(CombatMsgType));

    // Write user data
    if (_suffix)
        data.Write(_suffix->GetBuffer(), _suffix->GetSize());

    Messenger::Send(data.GetBuffer(), (size_t)data.GetSize(), Network::Messenger::GetSocket(_socket));
}
