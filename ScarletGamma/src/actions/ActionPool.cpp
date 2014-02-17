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
#include "UseSwitch.hpp"
#include "TakeObject.hpp"
#include "Game.hpp"
#include "states/CommonState.hpp"
#include "gamerules/Combat.hpp"
#include "Examine.hpp"

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
	m_actions.push_back(new UseSwitch());
	m_actions.push_back(new TakeObject());
	m_actions.push_back(new Examine());
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

std::vector<Core::ActionID> ActionPool::GetAllowedActions(Core::ObjectList& _executors, Core::Object& _object)
{
    // Create empty result vector
    std::vector<Core::ActionID> result;

    // Check all available actions
    for (std::vector<Action*>::iterator action = m_actions.begin(); action != m_actions.end(); ++action)
    {
        if ((*action)->CanUse(_executors, _object))
            result.push_back((*action)->m_id);
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
    m_localActionQueue.push(LocalActionInfo(_id, _executor, _target));
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

		if (g_Game->GetCommonState()->InCombat() && g_Game->GetCommonState()->GetCombat()->GetTurn() == _executor)
		{
			if (m_clientActions[index]->m_duration == Duration::STANDARD_ACTION)
				g_Game->GetCommonState()->GetCombat()->UseStandardAction();
			else if (m_clientActions[index]->m_duration == Duration::MOVE_ACTION)
				g_Game->GetCommonState()->GetCombat()->UseMoveAction();
		}
    }
}


void Actions::ActionPool::EndLocalAction()
{
    if (m_localAction)
        m_localAction->m_finished = true;
}

void ActionPool::EndClientAction(uint8_t _index)
{
    if (m_clientActions[_index])
        m_clientActions[_index]->m_finished = true;
}

void ActionPool::UpdateExecution()
{
	if( m_localAction )
	{
		m_localAction->Update();
		if( m_localAction->m_finished)
		{
			delete m_localAction;
			m_localAction = 0;
		}
	}

    // Push local action if none is active
    if (!m_localAction && m_localActionQueue.size() > 0)
    {
        // Get the new local action from the queue
        LocalActionInfo newAction = m_localActionQueue.front();
        m_localActionQueue.pop();
        Action* toCopy = m_actions.at(std::get<0>(newAction));

        // Create a copy of the action and return it
        if (toCopy)
        {
            // The old action is stopped here
            //delete m_localAction; if (!m_localAction.... on top -> there is nothing

            // Create a copy of the new action
            m_localAction = toCopy->Clone(std::get<1>(newAction), std::get<2>(newAction));

			if( g_Game->GetCommonState()->InCombat() )
			{
				if (m_localAction->m_duration == Duration::STANDARD_ACTION)
					g_Game->GetCommonState()->GetCombat()->UseStandardAction();
				else if (m_localAction->m_duration == Duration::MOVE_ACTION)
					g_Game->GetCommonState()->GetCombat()->UseMoveAction();
			}

            m_localAction->Execute();
        }
    }

	for( int i=0; i<24; ++i )
	{
		if( m_clientActions[i] && m_clientActions[i]->m_finished )
		{
			delete m_clientActions[i];
			m_clientActions[i] = 0;
		}
	}
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
