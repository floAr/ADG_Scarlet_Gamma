#include "UseJumpPoint.hpp"
#include "Game.hpp"
#include "core\World.hpp"
#include "Constants.hpp"

namespace Actions {


	UseJumpPoint::UseJumpPoint() : Action( STR_ACT_USEJUMPPOINT, ActionType::MOVE_ACTION )
	{
		m_targetRequirements.push_back( STR_PROP_JUMPPOINT );
	}

	void UseJumpPoint::Execute()
	{
		// The executor has a property which should be set
		Core::Object* jumpPointObj = g_Game->GetWorld()->GetObject( m_target );
		Core::Property& jp = jumpPointObj->GetProperty( STR_PROP_JUMPPOINT );
		
		// Remove object from old map
		Core::Object* execObj = g_Game->GetWorld()->GetObject( m_executor );
		int oldLayer = atoi(execObj->GetProperty( STR_PROP_LAYER ).Value().c_str());
		g_Game->GetWorld()->GetMap(execObj->GetParentMap())->Remove( m_executor );

		// Reinsert at new position
		Core::ObjectID targetID = (Core::ObjectID)atoi(jp.Value().c_str());
		Core::Object* targetObj = g_Game->GetWorld()->GetObject( targetID );
		sf::Vector2i targetPos = sfUtils::Round( targetObj->GetPosition() );
		g_Game->GetWorld()->GetMap(targetObj->GetParentMap())->Add(
			m_executor, targetPos.x, targetPos.y, oldLayer );

		// Setting positions of active objects confuses them
		if( execObj->HasProperty(STR_PROP_TARGET) )
		{
			Core::Property& prop = execObj->GetProperty(STR_PROP_TARGET);
			prop.SetValue( sfUtils::to_string(targetObj->GetPosition()) );
			execObj->Remove(STR_PROP_PATH);
		}
	}

	Action* UseJumpPoint::Clone( Core::ObjectID _excecutor, Core::ObjectID _target )
	{
		UseJumpPoint* jp = new UseJumpPoint();
		// Copy the automated id from this
		jp->m_id = m_id;
		// Set the relevant information
		jp->m_executor = _excecutor;
		jp->m_target = _target;

		return jp;
	}


}