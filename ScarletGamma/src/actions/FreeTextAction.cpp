#include "actions/FreeTextAction.hpp"
#include "Game.hpp"
#include "states/StateMachine.hpp"
#include "states/PromptState.hpp"
#include "network/Messenger.hpp"
#include "network/ActionMessages.hpp"
#include "states/MasterState.hpp"

using namespace Actions;

std::string FreeTextAction::m_valueCache = STR_EMPTY;

FreeTextAction::FreeTextAction() : Action(STR_ACT_FREETEXT, Duration::NO_ACTION, -1)
{
    m_finished = false;
}

void FreeTextAction::Execute()
{
    States::PromptState* prompt = static_cast<States::PromptState*>(g_Game->
        GetStateMachine()->PushGameState(States::GST_PROMPT) );
    prompt->SetText("Ziel: " + g_Game->GetWorld()->GetObject(m_target)->GetName() +
        ". Was möchtest du tun?\n");
    prompt->SetTextInputRequired(true);
    prompt->SetDefaultValue(m_valueCache);
    prompt->AddButton("OK", std::bind(&FreeTextAction::FreeTextPromptFinished, this,
        std::placeholders::_1), sf::Keyboard::Return);
    prompt->AddButton("Abbrechen", [](const std::string&) -> void {}, sf::Keyboard::Escape);
}

Action* FreeTextAction::Clone(Core::ObjectID _executor, Core::ObjectID _target)
{
    FreeTextAction* result = new FreeTextAction(*this);

    // Set all required values
    result->m_id = m_id;
    result->m_priority = m_priority;
    result->m_cursor = m_cursor;
    result->m_executor = _executor;
    result->m_target = _target;

    return result;
}

void Actions::FreeTextAction::FreeTextPromptFinished(const std::string& _result)
{
    // We assume we are a player, because the DM must not execute this action.
    // Tell the DM that we want to start a Free Text action
    Network::MsgActionBegin(this->m_id, m_target).Send();

    // Send him the free text that the player entered
    Network::MsgActionInfo(this->m_id, static_cast<uint8_t>(FreeTextMsgType::PL_FREETEXT), _result).Send();

    // That's pretty much it, end the action
    Network::MsgActionEnd(this->m_id).Send();
    m_finished = true;
}

bool Actions::FreeTextAction::CanUse(Core::ObjectList& _executors, Core::Object& _object)
{
    // Prevent execution if I am the server
    return Network::Messenger::IsServer() == false && Action::CanUse(_executors, _object);
}

void FreeTextAction::HandleActionInfo(uint8_t _messageType, const std::string& _message, uint8_t _sender)
{
    // I am the DM. This function handles player's requests and information.
    assert(Network::Messenger::IsServer());

    switch (static_cast<FreeTextMsgType>(_messageType))
    {
    case FreeTextMsgType::PL_FREETEXT: {
        // Player sent free text, first get the required objects
        Core::Object* executor = g_Game->GetWorld()->GetObject(m_executor);
        Core::Object* target = g_Game->GetWorld()->GetObject(m_target);

        // Get the master state
        States::MasterState* master = dynamic_cast<States::MasterState*>(g_Game->GetCommonState());
        assert(master);

        // Construct a helpful prompt for the DM
        States::PromptState* prompt = static_cast<States::PromptState*>(g_Game->
            GetStateMachine()->PushGameState(States::GST_PROMPT) );
        prompt->SetText("Spieler " + executor->GetName() + " möchte folgende Aktion auf " +
            target->GetName() + " starten:\n\"" + _message + "\"\n");
        prompt->SetTextInputRequired(false);
        prompt->AddButton("Zu " + target->GetName(), std::bind(&States::MasterState::GoTo, master, target));
        prompt->AddButton("Zu " + executor->GetName(), std::bind(&States::MasterState::GoTo, master, executor));
        prompt->AddButton("Schließen", [](const std::string&)->void {}, sf::Keyboard::Escape);

        } break;
    default:
        std::cerr << "FreeTextAction::HandleActionInfoResponse hat eine unbekannte Nachricht erhalten. " <<
            "ID: " << (int) _messageType << ", String: " << _messageType << '\n';
        break;
    }
}
