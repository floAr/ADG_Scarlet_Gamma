#include "WalkTo.hpp"
#include "Constants.hpp"
#include "../core/Object.hpp"
#include "network/ActionMessages.hpp"

using namespace Actions;

WalkTo::WalkTo() : Action(STR_ACT_WALKTO)
{
    // Set requirements
	m_requirements.push_back(STR_PROP_X);
    m_requirements.push_back(STR_PROP_Y);
}

void WalkTo::Execute()
{
    //TODO: Actual do something
}

Action* WalkTo::Clone(Core::ObjectID _executor, Core::ObjectID _target)
{
    WalkTo* result = new WalkTo();

    // TODO: copy all values

    return dynamic_cast<Action*>(result);
}