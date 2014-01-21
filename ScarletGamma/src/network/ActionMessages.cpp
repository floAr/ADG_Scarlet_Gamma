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

MsgActionBegin::MsgActionBegin(Core::ActionID _action, Core::ObjectID target)
    : ActionMsg(ActionMsgType::ACTION_BEGIN, _action), m_target(target)
{
}

void MsgActionBegin::WriteData(Jo::Files::MemFile& _output) const
{
    // Serialize
    _output.Write( &m_target, sizeof(Core::ObjectID) );
}

size_t MsgActionBegin::Receive(Core::ActionID _action, uint8_t _sender,
                               const uint8_t* _data, size_t _size)
{
    assert(Messenger::IsServer() && "Client got MsgActionBegin");

    // Deserialize
    Core::ObjectID target = *_data;

    std::cerr << "Player " <<  std::to_string(_sender) << " starting action '"
        <<  Actions::ActionPool::Instance().GetActionName(_action) << "' on target "
        << std::to_string(target) << '\n';
    Actions::ActionPool::Instance().StartClientAction(_action, target, _sender);

    return sizeof(Core::ObjectID);
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
    Core::ActionID currentAction = Actions::ActionPool::Instance().GetClientAction(_sender);

    if (currentAction == _action)
    {
        // End the current action
        Actions::ActionPool::Instance().EndClientAction(_sender);
        std::cerr << "Player " <<  std::to_string(_sender) << " ending action '"
            <<  Actions::ActionPool::Instance().GetActionName(_action) << "'" << '\n';
    }
    else
    {
        // Log error
        std::cerr << "Received request to end action '" <<  Actions::ActionPool::Instance().GetActionName(_action)
            << "' from " <<  std::to_string(_sender) << ", but the current action is "
            << Actions::ActionPool::Instance().GetActionName(currentAction) << "!" << '\n';
    }

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
// MsgActionInfo

MsgActionInfo::MsgActionInfo(Core::ActionID _action, uint8_t messageType, const std::string& message)
    : ActionMsg(ActionMsgType::ACTION_INFO, _action), m_messageType(messageType), m_message(message)
{
}

void MsgActionInfo::WriteData(Jo::Files::MemFile& _output) const
{
    // Serialize
    _output.Write( &m_messageType, sizeof(uint8_t) );
    _output.Write( m_message.c_str(), m_message.length());
}

size_t MsgActionInfo::Receive(Core::ActionID _action, uint8_t _sender,
                             const uint8_t* _data, size_t _size)
{
    Jo::Files::MemFile file(_data, _size);

    // Deserialize
    uint8_t messageType = *_data;
    std::string message((const char*) _data + sizeof(uint8_t), _size - sizeof(uint8_t));
    std::cout << "Received data " << message << " of type " << messageType << " from " << _sender << '\n';
    
    Actions::ActionPool::Instance().HandleActionInfo(_sender, messageType, message);

    return _size;
}