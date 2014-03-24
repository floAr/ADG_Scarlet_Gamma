#include "Combat.hpp"
#include "Game.hpp"
#include "States/StateMachine.hpp"
#include "States/CommonState.hpp"
#include "states/PromptState.hpp"
#include "network/CombatMessages.hpp"
#include "Constants.hpp"
#include "core/World.hpp"
#include "utils/Exception.hpp"
#include <jofilelib.hpp>
#include "states/PlayerState.hpp"
#include "network/Messenger.hpp"

using namespace GameRules;

GameRules::Combat::Combat() : m_currentObject(-1), m_diagonalCounter(false),
	m_fiveFootStepRemaining(false), m_moveActionRemaining(false),
	m_moveActionStepsLeft(0.0f), m_standardActionRemaining(false)
{
	UpdateCombatantPanel();
}

void Combat::AddParticipantWithInitiative(Core::ObjectID _object, int8_t _position)
{
    // Use user-defined index: negative values count backwards from the end
    // of the list
    uint8_t size;
    if (_position < 0)
        size = m_participants.size() + _position;
    else
        size = _position;

    // Iterate to position
    auto it = m_participants.begin();
    for (uint8_t i = 0; i < size && it != m_participants.end(); ++i)
		++it;

    // Insert object at position
    m_participants.insert( it, _object );

	// Update combatant panel
	UpdateCombatantPanel();
}

void GameRules::Combat::RemoveParticipant( Core::ObjectID _object )
{
	m_participants.remove(_object);
	UpdateCombatantPanel();
}

void GameRules::Combat::PushInitiativePrompt(Core::ObjectID _object)
{
    // Push initiative prompt state
    States::PromptState* prompt = dynamic_cast<States::PromptState*>(
        g_Game->GetStateMachine()->PushGameState(States::GST_PROMPT));
    Core::Object* object = g_Game->GetWorld()->GetObject(_object);
    prompt->SetText("Initiativewurf für " + object->GetName() + " eingeben:\n");

    // Set the default value for the roll
    if (object->HasProperty(STR_PROP_INITIATIVE_MOD))
        prompt->SetDefaultValue("1W20 + '" + STR_PROP_INITIATIVE_MOD + "'");
	else if (object->HasProperty(STR_PROP_DEXTERITY_MOD))
		prompt->SetDefaultValue("1W20 + '" + STR_PROP_DEXTERITY_MOD + "'");
	else
        prompt->SetDefaultValue("1W20");

    // Add callback function
	prompt->AddButton("OK", std::bind(&Combat::InitiativeRollPromptFinished, this,
		std::placeholders::_1, object->ID()), sf::Keyboard::Return, object);
	prompt->AddButton("Abbrechen", std::bind(&Combat::RemoveParticipant, this, object->ID()),
		sf::Keyboard::Escape);
}

void GameRules::Combat::InitiativeRollPromptFinished( const std::string& _result, Core::ObjectID _object )
{
    // Send initiative roll string to server
    Jo::Files::MemFile data;
    data.Write(&_object, sizeof(_object));
    data.Write(_result.c_str(), _result.length());
    Network::CombatMsg(Network::CombatMsgType::PL_COMBAT_INITIATIVE).Send(&data);
}

void GameRules::Combat::SetTurn( Core::ObjectID _object )
{
    // Try to find the object
    Core::Object* object = g_Game->GetWorld()->GetObject(_object);
    if (!object)
    {
        std::cerr << "Tried to call SetTurn with an object that couldn't be found. ObjectID is " << _object << "\n";
        return;
    }

    m_currentObject = _object;
    g_Game->AppendToChatLog( Network::ChatMsg(object->GetName() + " ist am Zug.", object->GetColorOpaque()) );

    // Reset combat round values
    m_diagonalCounter = false;
    m_fiveFootStepRemaining = true;
    m_standardActionRemaining = true;
    m_moveActionRemaining = true;
    
    // Set default speed of 9m and try to overwrite it
    m_moveActionStepsLeft = 9.0f;
    try
    {
        // Convert value to float, may raise an exception
        m_moveActionStepsLeft = std::stof( object->GetProperty(STR_PROP_SPEED).Value().c_str() );
    }
    catch (Exception::NoSuchProperty)
    {
        // No such property? Write it to the chat for master
        if (Network::Messenger::IsServer())
            g_Game->AppendToChatLog( Network::ChatMsg(STR_PROP_SPEED + " von " + object->GetName() +
            " nicht gefunden. Setze 9 Meter.", sf::Color::Red) );
    }
    catch (...)
    {
        // False property? Write it to the chat for master
        if (Network::Messenger::IsServer())
            g_Game->AppendToChatLog( Network::ChatMsg(STR_PROP_SPEED + " von " + object->GetName() +
            " ist fehlerhaft: " + object->GetProperty(STR_PROP_SPEED).Value() + ". Setze 9 Meter.", sf::Color::Red) );
    }

    // Set view to object if I am a player
    States::PlayerState* player = dynamic_cast<States::PlayerState*>(g_Game->GetCommonState());
    if (player)
        player->FocusObject(object);

	// Update panel
    UpdateCombatantPanel();
	UpdateCombatantPanelTurn();
}

bool GameRules::Combat::CanUse( Actions::Duration _duration ) const
{
    switch (_duration)
    {
    // You can always use NO_ACTION, FREE_ACTION and SWIFT_ACTION
    // TODO: only one swift action per round, but YAGNI for the prototype
    case Actions::Duration::NO_ACTION:
    case Actions::Duration::FREE_ACTION:
    case Actions::Duration::SWIFT_ACTION:
        return true;

    case Actions::Duration::FULL_ACTION:
        return m_moveActionRemaining && m_standardActionRemaining;

    case Actions::Duration::STANDARD_ACTION:
        return m_standardActionRemaining;

    case Actions::Duration::MOVE_ACTION:
        return m_moveActionRemaining;
    }

    // Unhandled, let's just use it
    return true;
}

float GameRules::Combat::GetRemainingSteps() const
{
    return m_moveActionStepsLeft;
}

void GameRules::Combat::EndTurn()
{
    // Send message to the GM
    if ( g_Game->GetCommonState()->OwnsObject(m_currentObject) )
        Network::CombatMsg(Network::CombatMsgType::PL_COMBAT_END_TURN).Send();
}

void GameRules::Combat::UseStandardAction()
{
	// This one's easy ;)
	m_standardActionRemaining = false;

	// If we started to move before, we can't move afterwards!
	if (!m_moveActionRemaining)
		m_moveActionStepsLeft = 0;
}

bool GameRules::Combat::UseMoveAction( float _distance, bool _diagonal )
{
	// Move-corresponding actions without a distance (e.g. load crossbow) prevent another
	// move action and set the steps to 0. Real move action (e.g. walk) don't check whether
	// the move action is available, but whether there are steps left. They disable the
	// move action, so no other move action can be started, and decrement the steps left.
	// In addition, they disable the 5-foot-step.
	if (_distance == 0)
	{
		m_moveActionRemaining = false;
		m_moveActionStepsLeft = 0;
		return true;
	}
	else
	{
		if (_diagonal)
		{
			// Double diagonal distance every second time
			if (m_diagonalCounter)
				_distance *= 2.0f;

			// Flip counter
			m_diagonalCounter = !m_diagonalCounter;
		}

		// If I did more than a 5-foot-step, or my step was already used, move action is used
		if (_distance > 1.5f || !m_fiveFootStepRemaining)
			m_moveActionRemaining = false;

		// First step voids the 5-foot-step in any case
		if (m_fiveFootStepRemaining)
			m_fiveFootStepRemaining = false;

		if (m_moveActionStepsLeft - _distance >= 0)
		{
			// Decrement the available distance
			m_moveActionStepsLeft -= _distance;
			return true;
		}
		else
		{
			// Move is not possible!
			return false;
		}
	}

#ifdef _DEBUG
	std::cout << m_moveActionRemaining << ' ' << m_fiveFootStepRemaining << ' '
		<< m_diagonalCounter << ' ' << m_moveActionStepsLeft << '\n';
#endif
}

Core::ObjectID GameRules::Combat::GetTurn() const
{
	return m_currentObject;
}

bool GameRules::Combat::HasParticipant( Core::ObjectID _object ) const
{
	return std::find(m_participants.begin(), m_participants.end(), _object) != m_participants.end();
}

bool GameRules::Combat::HasStarted() const
{
	return m_currentObject != -1;
}

void GameRules::Combat::UpdateCombatantPanel()
{
	// Update list of combattants
	g_Game->GetCommonState()->GetCombatantPanel()->UpdateCombatants(m_participants);

	// Set current participant
	UpdateCombatantPanelTurn();
}

void GameRules::Combat::UpdateCombatantPanelTurn()
{
	// No current object: return -1
	int index = -1;

	if ( HasStarted() )
	{
		index = std::distance( m_participants.begin(), std::find( m_participants.begin(),
			m_participants.end(), m_currentObject ) );
	}

	g_Game->GetCommonState()->GetCombatantPanel()->SetTurn(index);
}
