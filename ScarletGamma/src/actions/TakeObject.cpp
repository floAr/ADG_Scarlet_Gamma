#include "TakeObject.hpp"
#include "Game.hpp"
#include "core/World.hpp"
#include "Constants.hpp"
#include "actions/ActionPool.hpp"
#include "core/Object.hpp"

namespace Actions {


TakeObject::TakeObject() : Action(STR_ACT_TAKEOBJECT, Duration::FREE_ACTION, 0)
{
	m_targetRequirements.push_back( std::pair<std::string, bool>(STR_PROP_ITEM, true) );
	m_sourceRequirements.push_back( std::pair<std::string, bool>(STR_PROP_INVENTORY, true) );
}

void TakeObject::Execute()
{
	Core::Object* executor = g_Game->GetWorld()->GetObject(m_executor);
	Core::Object* target = g_Game->GetWorld()->GetObject(m_target);

	// Take the target object away from the map
	g_Game->GetWorld()->GetMap(target->GetParentMap())->Remove( m_target );
	// Add to executors inventory
	executor->GetProperty( STR_PROP_INVENTORY ).AddObject( target );
}

Action* TakeObject::Clone( Core::ObjectID _executor, Core::ObjectID _target )
{
	TakeObject* result = new TakeObject();

	// Set all required values
	result->m_id = m_id;
	result->m_priority = m_priority;
	result->m_cursor = m_cursor;
	result->m_executor = _executor;
	result->m_target = _target;

	return result;
}


}