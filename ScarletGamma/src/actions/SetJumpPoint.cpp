#include "SetJumpPoint.hpp"
#include "Game.hpp"
#include "core\World.hpp"
#include "Constants.hpp"

namespace Actions {


SetJumpPoint::SetJumpPoint() : Action(STR_ACT_SETJUMPPOINT, ActionType::FREE_ACTION, -1)
{
	m_sourceRequirements.push_back( STR_PROP_JUMPPOINT );
}

void SetJumpPoint::Execute()
{
	// The executor has a property which should be set
	Core::Object* jumpPointObj = g_Game->GetWorld()->GetObject( m_executor );
	Core::Property& jp = jumpPointObj->GetProperty( STR_PROP_JUMPPOINT );
	// The target id is the new jump target
	jp.SetValue( std::to_string(m_target) );
}

Action* SetJumpPoint::Clone( Core::ObjectID _executor, Core::ObjectID _target )
{
	SetJumpPoint* result = new SetJumpPoint();

	// Set all required values
	result->m_id = m_id;
	result->m_priority = m_priority;
	result->m_cursor = m_cursor;
	result->m_executor = _executor;
	result->m_target = _target;

	return result;
}


}