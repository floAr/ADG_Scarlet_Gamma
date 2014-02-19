#include "actions/Action.hpp"
#include "gamerules/Combat.hpp"

using namespace Actions;

bool Action::CanUse(Core::ObjectList& _executors, Core::Object& _object)
{
    // If in combat and action has a duration
    if ( g_Game->GetCommonState()->InCombat() && m_duration != Duration::NO_ACTION )
    {
        // action is only allowed in my round, break if I don't own any of the executors
        for( int i=0; i<_executors.Size(); ++i )
            if ( !g_Game->GetCommonState()->OwnsObject(_executors[i]) )
                return false;

        // action is only allowed if I didn't spend the required action already
        GameRules::Combat* combat = g_Game->GetCommonState()->GetCombat();
        if (!combat->CanUse(m_duration))
            return false;
    }

    // Get all requirements for target and loop through them
    const std::vector<std::pair<std::string, bool>>& requirements = m_targetRequirements;
    for (auto req = requirements.begin(); req != requirements.end(); ++req)
        if (_object.HasProperty((*req).first) != (*req).second)
            return false;

    // Get all requirements for executor and loop through them
    const std::vector<std::pair<std::string, bool>>& sourceRequirements = m_sourceRequirements;
    for (auto req = sourceRequirements.begin(); req != sourceRequirements.end(); ++req)
	{
        // Test all possible executors
		for( int i=0; i<_executors.Size(); ++i )
			if( g_Game->GetWorld()->GetObject(_executors[i])->HasProperty((*req).first) != (*req).second )
				return false;
	}

    return true;
}