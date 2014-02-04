#include "WalkTo.hpp"
#include "Constants.hpp"
#include "core/Object.hpp"
#include "network/ActionMessages.hpp"
#include "Game.hpp"
#include "core/World.hpp"
#include "core/PredefinedProperties.hpp"
#include "core/Map.hpp"
#include "actions/ActionPool.hpp"

using namespace Actions;

WalkTo::WalkTo() : Action(STR_ACT_WALKTO, ActionType::MOVE_ACTION)
{
    // Set requirements
    m_targetRequirements.push_back(STR_PROP_X);
    m_targetRequirements.push_back(STR_PROP_Y);
}

void WalkTo::Execute()
{
    Core::Object* executor = g_Game->GetWorld()->GetObject(m_executor);
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
    executor->AppendToPath( m_target );

    // Reevaluate object to get it updated
    g_Game->GetWorld()->GetMap(executor->GetParentMap())->ReevaluateActiveObject(m_executor);

    // Action is done
    Actions::ActionPool::Instance().EndLocalAction();
}

Action* WalkTo::Clone(Core::ObjectID _executor, Core::ObjectID _target)
{
    WalkTo* result = new WalkTo();

    result->m_executor = _executor; // set executor
    result->m_target = _target; // set target
    result->m_id = m_id; // copy ID

    return dynamic_cast<Action*>(result);
}