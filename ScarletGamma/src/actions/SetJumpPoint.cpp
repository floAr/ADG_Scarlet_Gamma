#include "SetJumpPoint.hpp"
#include "Game.hpp"
#include "core\World.hpp"
#include "Constants.hpp"

namespace Actions {


SetJumpPoint::SetJumpPoint() : Action(STR_ACT_SETJUMPPOINT)
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

Action* SetJumpPoint::Clone( Core::ObjectID _excecutor, Core::ObjectID _target )
{
	SetJumpPoint* jp = new SetJumpPoint();
	// Copy the automated id from this
	jp->m_id = m_id;
	// Set the relevant information
	jp->m_executor = _excecutor;
	jp->m_target = _target;

	return jp;
}


}