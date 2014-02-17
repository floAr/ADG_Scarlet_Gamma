#include "UseJumpPoint.hpp"
#include "Game.hpp"
#include "core\World.hpp"
#include "Constants.hpp"
#include "WalkTo.hpp"

namespace Actions {


	UseJumpPoint::UseJumpPoint() : Action( STR_ACT_USEJUMPPOINT, Duration::MOVE_ACTION, -1 )
	{
		m_finished = false;
		m_targetRequirements.push_back( std::pair<std::string, bool>(STR_PROP_JUMPPOINT, true) );
	}

	void UseJumpPoint::Execute()
	{
		// Perform a WalkTo action. This looks creepy but avoids much redundant code.
		WalkTo::Perform(m_executor, m_target);
	}

	void UseJumpPoint::Update()
	{
		// Are we close enough to the target?
		Core::Object* jumpPointObj = g_Game->GetWorld()->GetObject( m_target );
		Core::Object* execObj = g_Game->GetWorld()->GetObject( m_executor );

		if( sfUtils::LengthSq(jumpPointObj->GetPosition() - execObj->GetPosition()) < 0.01f )
		{
			Core::Property& jp = jumpPointObj->GetProperty( STR_PROP_JUMPPOINT );

			// Remove object from old map and reinsert at new position
			Core::ObjectID targetID = (Core::ObjectID)atoi(jp.Value().c_str());
			Core::Object* targetObj = g_Game->GetWorld()->GetObject( targetID );
			Core::Map* map = g_Game->GetWorld()->GetMap(execObj->GetParentMap());
			map->SetObjectPosition( execObj, targetObj->GetPosition() );
			execObj->ResetTarget();

			// This action is finished
			m_finished = true;
		}

		// Not ready yet
	}

	Action* UseJumpPoint::Clone( Core::ObjectID _executor, Core::ObjectID _target )
	{
		UseJumpPoint* result = new UseJumpPoint();

		// Set all required values
		result->m_id = m_id;
		result->m_priority = m_priority;
		result->m_cursor = m_cursor;
		result->m_executor = _executor;
		result->m_target = _target;

		return result;
	}


}