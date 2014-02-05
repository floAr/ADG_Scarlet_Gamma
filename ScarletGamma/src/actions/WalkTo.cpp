#include "WalkTo.hpp"
#include "Constants.hpp"
#include "core/Object.hpp"
#include "network/ActionMessages.hpp"
#include "Game.hpp"
#include "core/World.hpp"
#include "core/PredefinedProperties.hpp"
#include "core/Map.hpp"

using namespace Actions;

WalkTo::WalkTo() : Action(STR_ACT_WALKTO, ActionType::MOVE_ACTION, 100, Game::MC_WALK)
{
    // Set requirements
    m_targetRequirements.push_back(std::pair<std::string, bool>(STR_PROP_X, true));
    m_targetRequirements.push_back(std::pair<std::string, bool>(STR_PROP_Y, true));
    m_targetRequirements.push_back(std::pair<std::string, bool>(STR_PROP_OBSTACLE, false));
}

void WalkTo::Execute()
{
	Perform(m_executor, m_target);
}

void WalkTo::Perform(Core::ObjectID _executor, Core::ObjectID _target)
{
    Core::Object* executor = g_Game->GetWorld()->GetObject(_executor);
    assert(executor);

    if (!executor->HasProperty(STR_PROP_TARGET))
        executor->Add(Core::PROPERTY::TARGET);

    if (!executor->HasProperty(STR_PROP_PATH))
        executor->Add(Core::PROPERTY::PATH);

    // Clear the target
    executor->GetProperty(STR_PROP_TARGET).SetValue(STR_EMPTY);
    Core::Property& path = executor->GetProperty(STR_PROP_PATH);
    path.ClearObjects();
    path.SetValue(STR_FALSE);
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