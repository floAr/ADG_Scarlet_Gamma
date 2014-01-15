#include "actions/ActionPool.hpp"
#include "core/Object.hpp"
#include "actions/Attack.hpp"

using namespace Actions;

ActionPool::ActionPool()
{
    // Adding actions
    m_Actions.push_back(new Attack());

    //------------------------------//
    // TODO: add more actions here! //
    //------------------------------//
}

std::vector<Action*> ActionPool::GetAllowedActions(Core::Object& object)
{
    // Create empty result vector
    std::vector<Action*> result;

    // Check all avaibale actions
    for (std::vector<Action*>::iterator action = m_Actions.begin(); action != m_Actions.end(); ++action)
    {
        // Remember wethere all properties were found, initially true
        bool allowed = true;

        // Get all requirements for the action and loop through them
        std::vector<std::string> requirements = (*action)->GetRequirements();
        for (std::vector<std::string>::iterator req = requirements.begin(); req != requirements.end(); ++req)
        {
            if (object.HasProperty(*req) == false)
            {
                // Missing a property, remember that and get out of the requirements loop
                allowed = false;
                break;
            }
        }

        // Did the object have all properties?
        if (allowed)
        {
            result.push_back(*action);
        }
    }

    return result;
}