#include "UseSwitch.hpp"
#include "Game.hpp"
#include "core/World.hpp"
#include "Constants.hpp"
#include "actions/ActionPool.hpp"

namespace Actions {


UseSwitch::UseSwitch() : Action(STR_ACT_USESWITCH, Duration::FREE_ACTION, 1)
{
	m_targetRequirements.push_back( std::pair<std::string, bool>(STR_PROP_SWITCH, true) );
}

void UseSwitch::Execute()
{
	Core::Object* executor = g_Game->GetWorld()->GetObject(m_executor);
	Core::Object* target = g_Game->GetWorld()->GetObject(m_target);
	auto& prop = target->GetProperty(STR_PROP_SWITCH);
	if( prop.Value() == STR_OFF ) prop.SetValue( STR_ON );
	else prop.SetValue( STR_OFF );
	if( executor->HasProperty(STR_PROP_PLAYER) )
		Network::ChatMsg(executor->GetName() + " hat den Schalter '" + target->GetName() + "' betätigt.",
		executor->GetColorOpaque(), true).Send();
}

Action* UseSwitch::Clone( Core::ObjectID _executor, Core::ObjectID _target )
{
	UseSwitch* result = new UseSwitch();

	// Set all required values
	result->m_id = m_id;
	result->m_priority = m_priority;
	result->m_cursor = m_cursor;
	result->m_executor = _executor;
	result->m_target = _target;

	return result;
}


}