#include "actions/ActionPool.hpp"
#include "core/Object.hpp"
#include "actions/Attack.hpp"

using namespace Actions;

// Initialize Singleton pointer
ActionPool* ActionPool::m_Instance = nullptr;

ActionPool::ActionPool()
{
    // Adding actions
    m_Actions.push_back(new Attack());
    //------------------------------//
    // TODO: add more actions here! //
    //------------------------------//

    // Set action IDs
    for (unsigned int id = 0; id < m_Actions.size(); ++id)
    {
        m_Actions.at(id)->SetID(id);
    }
}

std::vector<Core::ActionID> ActionPool::GetAllowedActions(Core::Object& object)
{
    // Create empty result vector
    std::vector<Core::ActionID> result;

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
            result.push_back((*action)->GetID());
        }
    }

    return result;
}

const std::string ActionPool::GetActionName(Core::ActionID id)
{
    Action* unknown = m_Actions.at(id);
    return unknown->GetName();
}

Action* ActionPool::StartAction(Core::ActionID id, int index)
{
    Action* toCopy = m_Actions.at(id);

    // Create a copy of the action and return it
    if (toCopy)
    {
        Action* newAction = toCopy->Clone();
        newAction->Execute();
        return newAction;
    }
    
    // Action not found
    return 0;
}

Action* ActionPool::GetCurrentAction(int index)
{
    return m_CurrentActions[index];
}