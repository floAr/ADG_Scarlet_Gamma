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
        /// \param [in] object  Object to be examined
        /// \returns Vector of Action pointers for which the actions are applicable
        std::vector<Core::ActionID> GetAllowedActions(Core::Object& object);

        /// \brief Get the name of an action by ID.
        /// \param [in] id  ID of the action to start, \see GetAllowedActions
        /// \returns  Name of the action specified by ID
        const std::string& GetActionName(Core::ActionID id);

        /// \brief Get an action that is currently in progress.
        /// \param [in] index  Optional: specify an index (i.e. player ID when you
        ///   are the server)
        /// \returns  Pointer to action, may be 0
        Core::ActionID GetCurrentAction(int index = 0);

        /// \brief Start an action on the local machine by ID.
        /// \param [in] id     ID of the action to start, \see GetAllowedActions
        void StartAction(Core::ActionID id);

        /// \brief Start an action that a remote machine requested.
        /// \param [in] id     ID of the action to start, \see GetAllowedActions
        /// \param [in] index  Optional: specify an index (i.e. player ID)
        void StartClientAction(Core::ActionID id, uint8_t index);

        /// \brief Ends a current action at the specified index
        /// \param [in] index  Optional: specify an index (i.e. player ID when you
        ///   are the server)
        void EndAction(int index = 0);

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

        /// \brief Array of current action(s).
        /// \detail Players really only needs the first element, while the GM needs
        ///   at most one for each player.
        Action* m_CurrentActions[24];
    };
}