#include "actions/ActionPool.hpp"
#include "core/Object.hpp"
#include "actions/Attack.hpp"
#include "actions/WalkTo.hpp"
#include "network/Messenger.hpp"
#include <assert.h>
#include "SetJumpPoint.hpp"
#include "UseJumpPoint.hpp"
#include "core/World.hpp"
#include <iostream>

using namespace Actions;

// Initialize Singleton pointer
ActionPool* ActionPool::m_instance = nullptr;

ActionPool::ActionPool() : 
	m_localAction(nullptr)
{
    // Adding actions
    m_actions.push_back(new Attack());
    m_actions.push_back(new WalkTo());
	m_actions.push_back(new SetJumpPoint());
	m_actions.push_back(new UseJumpPoint());
    //------------------------------//
    // TODO: add more actions here! //
    //------------------------------//

    // Set action IDs
    for (unsigned int id = 0; id < m_actions.size(); ++id)
    {
        m_actions.at(id)->m_id = id;
    }

	for( int i=0; i<24; ++i )
		m_clientActions[i] = nullptr;
}

std::vector<Core::ActionID> ActionPool::GetAllowedActions(Core::ObjectList& _executors, Core::Object& object)
{
    // Create empty result vector
    std::vector<Core::ActionID> result;

    // Check all available actions
    for (std::vector<Action*>::iterator action = m_actions.begin(); action != m_actions.end(); ++action)
    {
        // Remember whether all properties were found, initially true
        bool allowed = true;

        // Get all requirements for target and loop through them
        const std::vector<std::pair<std::string, bool>>& requirements = (*action)->GetTargetRequirements();
        for (auto req = requirements.begin(); req != requirements.end(); ++req)
        {
            if (object.HasProperty((*req).first) != (*req).second)
            {
                // Missing a property, remember that and get out of the requirements loop
                allowed = false;
                break;
            }
        }

		// Get all requirements for executor and loop through them
		const std::vector<std::pair<std::string, bool>>& sourceRequirements = (*action)->GetSourceRequirements();
		for (auto req = sourceRequirements.begin(); req != sourceRequirements.end() && allowed; ++req)
		{
			// Test all possible executors
			for( int i=0; i<_executors.Size(); ++i )
			if( g_Game->GetWorld()->GetObject(_executors[i])->HasProperty((*req).first) != (*req).second )
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

    // Sort actions by property, descending
    std::sort(result.begin(), result.end(),
        [](int x, int y) -> bool
        {
            auto& actions = ActionPool::Instance().m_actions;
            return actions[x]->m_priority > actions[y]->m_priority;
        } );

    return result;
}

void ActionPool::UpdateDefaultAction(Core::ObjectList& _executors, Core::Object* _object)
{
    // Void default action if we have no target
    if (_object == 0 || _executors.Size() == 0)
    {
        m_currentDefaultAction = 0;
        m_lastDefaultActionTarget = -1;
        g_Game->SetMouseCursor(Game::MC_DEFAULT);
        return;
    }

    // Don't recheck on the same target, and let's just keep our fingers crossed
    // that the executors didn't change :)
    if (_object->ID() == m_lastDefaultActionTarget)
        return;

    // Remember last target and get sorted list of allowed actions
    m_lastDefaultActionTarget = _object->ID();
    auto actions = GetAllowedActions(_executors, *_object);

    // Check whether we got a valid default action
    if (actions.size() == 0 || CanBeDefaultAction(actions.front()) == false)
    {
        m_currentDefaultAction = 0;
        g_Game->SetMouseCursor(Game::MC_DEFAULT);
    }
    else
    {
        m_currentDefaultAction = m_actions[actions.front()];
        g_Game->SetMouseCursor(m_currentDefaultAction->m_cursor);
    }
}

bool Actions::ActionPool::StartDefaultAction( Core::ObjectID _executor, Core::ObjectID _target )
{
    if (m_currentDefaultAction == 0)
        return false;

    StartLocalAction(m_currentDefaultAction->m_id, _executor, _target);
    return true;
}


const std::string& ActionPool::GetActionName(Core::ActionID id)
{
    Action* unknown = m_actions.at(id);
    return unknown->GetName();
}

bool ActionPool::CanBeDefaultAction(Core::ActionID _id)
{
    return m_actions[_id]->m_priority > 0;
}

Core::ActionID ActionPool::GetLocalAction()
{
    if (m_localAction != 0)
        return m_localAction->m_id;
    else
        return -1;
}

void ActionPool::StartLocalAction(Core::ActionID _id, Core::ObjectID _executor,
                                  Core::ObjectID _target)
{
    Action* toCopy = m_actions.at(_id);

    // Create a copy of the action and return it
    if (toCopy)
    {
		// The old action is stopped here
		EndLocalAction();

        Action* newAction = toCopy->Clone(_executor, _target);
        m_localAction = newAction;
        newAction->Execute();
    }
}

void ActionPool::StartClientAction(Core::ActionID id, Core::ObjectID _executor,
                                   Core::ObjectID target, uint8_t index)
{
    Action* toCopy = m_actions.at(id);

    // Create a copy of the action and return it
    if (toCopy)
    {
        Action* newAction = toCopy->Clone(_executor, target);
        m_clientActions[index] = newAction;
    }
}

void ActionPool::UpdateExecution()
{
	if( m_localAction )
	{
		if( m_localAction->Update() )
			EndLocalAction();
	}

	for( int i=0; i<24; ++i )
	{
		if( m_clientActions[i] )
		{
			if( m_clientActions[i]->Update() )
				EndClientAction(i);
		}
	}
}

void ActionPool::EndLocalAction()
{
    delete m_localAction;
    m_localAction = 0;
}

void ActionPool::EndClientAction(uint8_t index)
{
    delete m_clientActions[index];
    m_clientActions[index] = 0;
}

Core::ActionID ActionPool::GetClientAction(int index)
{
    return m_clientActions[index]->m_id;
}

void ActionPool::HandleActionInfo(uint8_t sender, uint8_t messageType, const std::string& message)
{
    if (Network::Messenger::IsServer())
    {
        // Handle Client's message
        assert(m_clientActions[sender] && "Received MsgActionInfo while no client action was started.");
        m_clientActions[sender]->HandleActionInfo(messageType, message, sender);
    }
    else
    {
        // Handle server's response
        assert(m_localAction && "Received MsgActionInfo while no local action was started.");
        m_localAction->HandleActionInfoResponse(messageType, message);
    }
}
