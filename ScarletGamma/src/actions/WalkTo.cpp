#include "WalkTo.hpp"
#include "Constants.hpp"
#include "core/Object.hpp"
#include "network/ActionMessages.hpp"
#include "Game.hpp"
#include "states/CommonState.hpp"
#include "gamerules/Combat.hpp"
#include "core/World.hpp"
#include "core/PredefinedProperties.hpp"
#include "core/Map.hpp"
#include "network/Messenger.hpp"
#include "events/InputHandler.hpp"

using namespace Actions;

WalkTo::WalkTo() : Action(STR_ACT_WALKTO, Duration::FREE_ACTION, 100, Game::MC_WALK)
{
    // WalkTo can't be a Walk action, because it doesn't void the remaining steps. Sorry.

    // Set requirements
    m_targetRequirements.push_back(std::pair<std::string, bool>(STR_PROP_X, true));
    m_targetRequirements.push_back(std::pair<std::string, bool>(STR_PROP_Y, true));
    m_targetRequirements.push_back(std::pair<std::string, bool>(STR_PROP_OBSTACLE, false));
}

void WalkTo::Execute()
{
	Perform(m_executor, m_target, Network::Messenger::IsServer()
		&& Events::InputHandler::IsControlPressed());
}

void WalkTo::Perform(Core::ObjectID _executor, Core::ObjectID _target, bool _append)
{
    Core::Object* executor = g_Game->GetWorld()->GetObject(_executor);
    assert(executor);

    if (!executor->HasProperty(STR_PROP_TARGET))
        executor->Add(Core::PROPERTY::TARGET);

    if (!executor->HasProperty(STR_PROP_PATH))
        executor->Add(Core::PROPERTY::PATH);

    // Clear the target if not appending
	if (!_append)
	{
		executor->GetProperty(STR_PROP_TARGET).SetValue(STR_EMPTY);
		Core::Property& path = executor->GetProperty(STR_PROP_PATH);
		path.ClearObjects();
		path.SetValue(STR_FALSE);
	}

	// Append
    executor->AppendToPath( _target );

    // Reevaluate object to get it updated
    g_Game->GetWorld()->GetMap(executor->GetParentMap())->ReevaluateActiveObject(_executor);
}

Action* WalkTo::Clone(Core::ObjectID _executor, Core::ObjectID _target)
{
    WalkTo* result = new WalkTo();

    // Set all required values
    result->m_id = m_id;
    result->m_priority = m_priority;
    result->m_cursor = m_cursor;
    result->m_executor = _executor;
    result->m_target = _target;

    return dynamic_cast<Action*>(result);
}

bool Actions::WalkTo::CanUse( Core::ObjectList& _executors, Core::Object& _object )
{
	bool result = true;

	// In combat, we can walk as long as we have remaining steps.
	if (g_Game->GetCommonState()->InCombat())
		result = (g_Game->GetCommonState()->GetCombat()->GetRemainingSteps() > 0);

	return result && Action::CanUse(_executors, _object);
}
