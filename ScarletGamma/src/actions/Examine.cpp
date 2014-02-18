#include "Examine.hpp"
#include "Game.hpp"
#include "Constants.hpp"
#include "actions/ActionPool.hpp"
#include "states/PlayerState.hpp"
#include "network/Messenger.hpp"

namespace Actions {


Examine::Examine() : Action(STR_ACT_EXAMINE, Duration::STANDARD_ACTION, 1)
{
	m_sourceRequirements.push_back( std::pair<std::string, bool>(STR_PROP_PLAYER, true) );
}

void Examine::Execute()
{
	// The master can examine everything by default...
	States::PlayerState* state = dynamic_cast<States::PlayerState*>(g_Game->GetCommonState());
	if( state )
	{
		state->ExamineObject(m_target);
	} else
		Network::ChatMsg("Aktion als Spielleiter nicht möglich.", sf::Color::Red, true).Send();
}

Action* Examine::Clone( Core::ObjectID _executor, Core::ObjectID _target )
{
	Examine* result = new Examine(*this);

	// Set all required values
	result->m_executor = _executor;
	result->m_target = _target;

	return result;
}

bool Examine::CanUse( Core::ObjectList& _executors, Core::Object& _object )
{
	return !Network::Messenger::IsServer() && Action::CanUse(_executors, _object);
}


}