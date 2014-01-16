#include "ActionMessages.hpp"
#include "Message.hpp"
#include "Messenger.hpp"
#include "Actions/ActionPool.hpp"
#include <assert.h>

using namespace Network;

size_t Network::HandleActionMessage(Core::ActionID _action, const uint8_t* _data, size_t _size)
{
    assert(_size > sizeof(ActionMsgType));

    const ActionMsgType* header = reinterpret_cast<const ActionMsgType*>(_data);
    size_t readSize = sizeof(ActionMsgType);

    switch(*header)
    {
    case ActionMsgType::ACTION_BEGIN:
        // Start a new action
        readSize += MsgActionBegin::Receive(_action, _data + readSize, _size - readSize);
        break;
    case ActionMsgType::ACTION_END:
        // End the current action
        //readSize += MsgEndAction::Receive(_action, _data + readSize, _size - readSize);
        break;
    case ActionMsgType::ACTION_INFO:
        // Notify the action about something
        //readSize += MsgEndAction::Receive(_action, _data + readSize, _size - readSize);
        break;
    }

    return readSize;
}


////////////////////////////////////////////////////////////////////////////////
// ActionMsg

void ActionMsg::Send()
{
    // Write headers
    Jo::Files::MemFile data;
    data.Write(&MessageHeader(Target::OBJECT, m_action), sizeof(MessageHeader));
    data.Write(&m_purpose, sizeof(ActionMsgType));
    // Write data
    WriteData(data);
    Messenger::Send(data.GetBuffer(), (size_t)data.GetSize());
}


////////////////////////////////////////////////////////////////////////////////
// MsgActionBegin

MsgActionBegin::MsgActionBegin(Core::ActionID _action)
    : ActionMsg(ActionMsgType::ACTION_BEGIN, _action)
{
}

void MsgActionBegin::WriteData(Jo::Files::MemFile& _output) const
{
    // Write nothing, ActionMsgType and ActionID are sufficient
}

size_t MsgActionBegin::Receive(Core::ActionID _action, const uint8_t* _data, size_t _size)
{
    Jo::Files::MemFile file(_data, _size);

    // TODO: If server, where to get the player ID from?!
    //Actions::ActionPool::Instance().RemoteStartAction(_action);
    // TODO: implement function to start remote Action on the server

    return (size_t)file.GetCursor();
}


////////////////////////////////////////////////////////////////////////////////
// MsgActionEnd

MsgActionEnd::MsgActionEnd(Core::ActionID _action)
    : ActionMsg(ActionMsgType::ACTION_END, _action)
{
}

void MsgActionEnd::WriteData(Jo::Files::MemFile& _output) const
{
    // Write nothing, ActionMsgType and ActionID are sufficient
}

size_t MsgActionEnd::Receive(Core::ActionID _action, const uint8_t* _data, size_t _size)
{
    Jo::Files::MemFile file(_data, _size);

    // TODO: End an action!

    return (size_t)file.GetCursor();
}


////////////////////////////////////////////////////////////////////////////////
// MsgActionInfo

// ...