#pragma once

#include "Prerequisites.hpp"
#include <memory>
#include <vector>

namespace Actions
{
    class ActionPool
    {
    public:
        /// \brief Get the only instance of the ActionPool object
        static ActionPool& Instance()
        {
            if (m_Instance == nullptr)
                m_Instance = new ActionPool();
            return *m_Instance;
        }

        /// \brief Returns all actions which are allowed to be executed on a given object.
        /// \param [in] _executor A list of objects which try to do things
        /// \param [in] object  Object to be examined
        /// \returns Vector of Action pointers for which the actions are applicable
        std::vector<Core::ActionID> GetAllowedActions(std::vector<Core::Object*> _executors, Core::Object& object);

        /// \brief Get the name of an action by ID.
        /// \param [in] id  ID of the action to start, \see GetAllowedActions
        /// \returns  Name of the action specified by ID
        const std::string& GetActionName(Core::ActionID id);

        /// \brief Get a local action that is currently in progress.
        /// \returns  Pointer to action, may be 0
        Core::ActionID GetLocalAction();

        /// \brief Get a client action that is currently in progress.
        /// \param [in] index  Optional: specify an index (i.e. player ID when you
        ///   are the server)
        /// \returns  Pointer to action, may be 0
        Core::ActionID GetClientAction(int index = 0);

        /// \brief Start an action on the local machine by ID.
        /// \param [in] id     ID of the action to start, \see GetAllowedActions
        void StartLocalAction(Core::ActionID _id, Core::ObjectID _executor,
            Core::ObjectID _target);

        /// \brief Start an action that a remote machine requested.
        /// \param [in] id     ID of the action to start, \see GetAllowedActions
        /// \param [in] index  Specify an index (i.e. player ID)
        void StartClientAction(Core::ActionID _id, Core::ObjectID _executor,
            Core::ObjectID _target, uint8_t _index);

        /// \brief Ends a current action at the specified index
        void EndLocalAction();

        /// \brief End an action that a remote machine requested.
        /// \param [in] index  Specify an index (i.e. player ID)
        void EndClientAction(uint8_t index);

        /// \brief Pass action message to the current action to be handled.
        void HandleActionInfo(uint8_t sender, uint8_t messageType, const std::string& message);

    private:
        /// \brief Private constructor prohibits more than one instance
        /// Adds all the actions in the game to the list.
        /// Implementing an action? Register it here!
        ActionPool();

        /// \brief Prevent copy construction
        ActionPool(const ActionPool&);

        /// \brief Prevent assignment
        ActionPool& operator=(const ActionPool&);

        /// \brief Prevent destruction
        ~ActionPool();

        /// \brief The only instance of the ActionPool
        static ActionPool* m_Instance;

        /// \brief List of all actions in the game
        std::vector<Action*> m_Actions;

        /// \brief Array of current client action(s).
        Action* m_ClientActions[24];

        /// \brief Current local action.
        Action* m_LocalAction;
    };
}