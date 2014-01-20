#include "ActionMessages.hpp"
#include "Message.hpp"
#include "Messenger.hpp"
#include "Actions/ActionPool.hpp"
#include <assert.h>
#include <iostream>

using namespace Network;

size_t Network::HandleActionMessage(Core::ActionID _action, const uint8_t* _data,
                                    size_t _size, uint8_t _sender)
{
    assert(_size >= sizeof(ActionMsgType));

    // TODO: use _sender id

    const ActionMsgType* header = reinterpret_cast<const ActionMsgType*>(_data);
    size_t readSize = sizeof(ActionMsgType);

    switch(*header)
    {
    case ActionMsgType::ACTION_BEGIN:
        // Start a new action
        readSize += MsgActionBegin::Receive(_action, _sender, _data + readSize, _size - readSize);
        break;
    case ActionMsgType::ACTION_END:
        // End the current action
        readSize += MsgActionEnd::Receive(_action, _sender, _data + readSize, _size - readSize);
        break;
    case ActionMsgType::ACTION_INFO:
        // Notify the action about something
        readSize += MsgActionInfo::Receive(_action, _sender, _data + readSize, _size - readSize);
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
    data.Write(&MessageHeader(Target::ACTION, m_action), sizeof(MessageHeader));
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

size_t MsgActionBegin::Receive(Core::ActionID _action, uint8_t _sender,
                               const uint8_t* _data, size_t _size)
{
    Jo::Files::MemFile file(_data, _size);

    assert(Messenger::IsServer());
    Actions::ActionPool::Instance().StartClientAction(_action, _sender);

    return (size_t)file.GetCursor();
}


////////////////////////////////////////////////////////////////////////////////
// MsgActionEnd

MsgActionEnd::MsgActionEnd(Core::ActionID _action)
    : ActionMsg(ActionMsgType::ACTION_END, _action)
{
}

size_t MsgActionEnd::Receive(Core::ActionID _action, uint8_t _sender,
                             const uint8_t* _data, size_t _size)
{
    Jo::Files::MemFile file(_data, _size);

    Core::ActionID currentAction = Actions::ActionPool::Instance().GetCurrentAction(_sender);

    if (currentAction == _action)
    {
        // End the current action
        Actions::ActionPool::Instance().EndAction(_sender);
    }
    else
    {
        // Log error
        std::cerr << "Received request to end action '" <<  Actions::ActionPool::Instance().GetActionName(_action)
            << "' from " <<  std::to_string(_sender) << ", but the current action is "
            << Actions::ActionPool::Instance().GetActionName(currentAction) << "!" << '\n';
    }

    return (size_t)file.GetCursor();
}


////////////////////////////////////////////////////////////////////////////////
// MsgActionInfo

MsgActionInfo::MsgActionInfo(Core::ActionID _action)
    : ActionMsg(ActionMsgType::ACTION_INFO, _action)
{
}

void MsgActionInfo::WriteData(Jo::Files::MemFile& _output) const
{
    // TODO: Write message data
}

size_t MsgActionInfo::Receive(Core::ActionID _action, uint8_t _sender,
                             const uint8_t* _data, size_t _size)
{
    Jo::Files::MemFile file(_data, _size);

    // TODO: Send info to action!

    return (size_t)file.GetCursor();
}