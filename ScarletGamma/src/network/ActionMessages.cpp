#include "ActionMessages.hpp"
#include "Message.hpp"
#include "Messenger.hpp"
#include <assert.h>

using namespace Network;

size_t Network::HandleActionMessage(Core::ActionID _action, const uint8_t* _data, size_t _size)
{
    assert(_size > sizeof(ActionMsgType));

    const ActionMsgType* header = reinterpret_cast<const ActionMsgType*>(_data);
    size_t readSize = sizeof(ActionMsgType);

    switch(*header)
    {
    case ActionMsgType::BEGIN_ACTION:
        readSize += MsgBeginAction::Receive(_action, _data + readSize, _size - readSize);
        break;
    case ActionMsgType::END_ACTION:
        //readSize += MsgEndAction::Receive(_action, _data + readSize, _size - readSize);
        break;
    }

    return readSize;
}

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

MsgBeginAction::MsgBeginAction(Core::ActionID _action)
    : ActionMsg(ActionMsgType::BEGIN_ACTION, _action)
{
}

void MsgBeginAction::WriteData(Jo::Files::MemFile& _output) const
{
    // Write nothing, ActionMsgType and ActionID are sufficient
}

size_t MsgBeginAction::Receive(Core::ActionID _action, const uint8_t* _data, size_t _size)
{
    Jo::Files::MemFile file(_data, _size);

    // TODO: Bring the action!

    return (size_t)file.GetCursor();
}