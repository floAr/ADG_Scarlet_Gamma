#include "actions/ActionPool.hpp"
#include "core/Object.hpp"
#include "actions/Attack.hpp"
#include "actions/WalkTo.hpp"
#include "network/Messenger.hpp"
#include <assert.h>
#include "SetJumpPoint.hpp"
#include "UseJumpPoint.hpp"

using namespace Actions;

// Initialize Singleton pointer
ActionPool* ActionPool::m_Instance = nullptr;

ActionPool::ActionPool()
{
    // Adding actions
    m_Actions.push_back(new Attack());
    m_Actions.push_back(new WalkTo());
	m_Actions.push_back(new SetJumpPoint());
	m_Actions.push_back(new UseJumpPoint());
    //------------------------------//
    // TODO: add more actions here! //
    //------------------------------//

    // Set action IDs
    for (unsigned int id = 0; id < m_Actions.size(); ++id)
    {
        m_Actions.at(id)->m_id = id;
    }
}

std::vector<Core::ActionID> ActionPool::GetAllowedActions(std::vector<Core::Object*> _executors, Core::Object& object)
{
    // Create empty result vector
    std::vector<Core::ActionID> result;

    // Check all available actions
    for (std::vector<Action*>::iterator action = m_Actions.begin(); action != m_Actions.end(); ++action)
    {
        // Remember whether all properties were found, initially true
        bool allowed = true;

        // Get all requirements for target and loop through them
        const std::vector<std::string>& requirements = (*action)->GetTargetRequirements();
        for (auto req = requirements.begin(); req != requirements.end(); ++req)
        {
            if (object.HasProperty(*req) == false)
            {
                // Missing a property, remember that and get out of the requirements loop
                allowed = false;
                break;
            }
        }

		// Get all requirements for executor and loop through them
		const std::vector<std::string>& sourceRequirements = (*action)->GetSourceRequirements();
		for (auto req = sourceRequirements.begin(); req != sourceRequirements.end() && allowed; ++req)
		{
			// Test all possible executors
			for( size_t i=0; i<_executors.size(); ++i )
			if( !_executors[i]->HasProperty(*req) )
			{
				// Missing a property, remember that and get out of the requirements loop
				allowed = false;
				break;
			}
		}

        // Did the object have all properties?
        if (allowed)
        {
            result.push_back((*action)->m_id);
        }
    }

    return result;
}

const std::string& ActionPool::GetActionName(Core::ActionID id)
{
    Action* unknown = m_Actions.at(id);
    return unknown->GetName();
}

Core::ActionID ActionPool::GetLocalAction()
{
    if (m_LocalAction != 0)
        return m_LocalAction->m_id;
    else
        return -1;
}

void ActionPool::StartLocalAction(Core::ActionID _id, Core::ObjectID _executor,
                                  Core::ObjectID _target)
{
    Action* toCopy = m_Actions.at(_id);

    // Create a copy of the action and return it
    if (toCopy)
    {
        Action* newAction = toCopy->Clone(_executor, _target);
        m_LocalAction = newAction;
        newAction->Execute();
    }
}


void ActionPool::StartClientAction(Core::ActionID id, Core::ObjectID _executor,
                                   Core::ObjectID target, uint8_t index)
{
    Action* toCopy = m_Actions.at(id);

    // Create a copy of the action and return it
    if (toCopy)
    {
        Action* newAction = toCopy->Clone(_executor, target);
        m_ClientActions[index] = newAction;
    }
}

void ActionPool::EndLocalAction()
{
    delete m_LocalAction;
    m_LocalAction = 0;
}

void ActionPool::EndClientAction(uint8_t index)
{
    delete m_ClientActions[index];
    m_ClientActions[index] = 0;
}

Core::ActionID ActionPool::GetClientAction(int index)
{
    return m_ClientActions[index]->m_id;
}

void ActionPool::HandleActionInfo(uint8_t sender, uint8_t messageType, const std::string& message)
{
    if (Network::Messenger::IsServer())
    {
        // Handle Client's message
        assert(m_ClientActions[sender] && "Received MsgActionInfo while no client action was started.");
        m_ClientActions[sender]->HandleActionInfo(messageType, message, sender);
    }
    else
    {
        // Handle server's response
        assert(m_LocalAction && "Received MsgActionInfo while no local action was started.");
        m_LocalAction->HandleActionInfoResponse(messageType, message);
    }
}