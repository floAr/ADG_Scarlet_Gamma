#pragma once

#include "Prerequisites.hpp"
#include <memory>
#include <vector>
#include <queue>
#include <tuple>

namespace Actions
{
    class ActionPool
    {
    public:
        /// \brief Get the only instance of the ActionPool object
        static ActionPool& Instance()
        {
            if (m_instance == nullptr)
                m_instance = new ActionPool();
            return *m_instance;
        }

        /// \brief Returns all actions which are allowed to be executed on a given object.
        /// \param [in] _executor A list of objects which try to do things
        /// \param [in] object  Object to be examined
        /// \returns Vector of Action pointers for which the actions are applicable
        std::vector<Core::ActionID> GetAllowedActions(Core::ObjectList& _executors, Core::Object& _object);

        /// \brief Get the name of an action by ID.
        /// \param [in] id  ID of the action to start, \see GetAllowedActions
        /// \returns  Name of the action specified by ID
        const std::string& GetActionName(Core::ActionID _id);

        /// \brief Check whether a the default action might have changed.
        /// \param [in] _executors  Executors that are used for the default action check.
        /// \param [in] _object     Target that is used for the default action check.
        void UpdateDefaultAction(Core::ObjectList& _executors, Core::Object* _object);

        /// \brief Starts the current default action on the target.
        /// \detail Be sure to call UpdateDefaultAction right before using this function!
        /// \return True if an action was started, false otherwise (maybe we wanna handle
        ///     that as selection or something?)
        bool StartDefaultAction(Core::ObjectID _executor, Core::ObjectID _target);

        /// \brief Checks whether this action can ever be used as the default
        ///     left-click action. If not, it is only accessible using the
        ///     right-click menu.
        /// \param [in] id  The action to check.
        bool CanBeDefaultAction(Core::ActionID _id);

        /// \brief Get a local action that is currently in progress.
        /// \returns  Pointer to action, may be 0
        Core::ActionID GetLocalAction();

        /// \brief Get a client action that is currently in progress.
        /// \param [in] index  Optional: specify an index (i.e. player ID when you
        ///   are the server)
        /// \returns  Pointer to action, may be 0
        Core::ActionID GetClientAction(int _index = 0);

        /// \brief Start an action on the local machine by ID.
        /// \param [in] id     ID of the action to start, \see GetAllowedActions
        void StartLocalAction(Core::ActionID _id, Core::ObjectID _executor,
            Core::ObjectID _target);

        /// \brief Start an action that a remote machine requested.
        /// \param [in] id     ID of the action to start, \see GetAllowedActions
        /// \param [in] index  Specify an index (i.e. player ID)
        void StartClientAction(Core::ActionID _id, Core::ObjectID _executor,
            Core::ObjectID _target, uint8_t _index);

		/// \brief Call Update() for each active action.
		/// \details Some actions are active for some time and require
		///		continual check of the current task to be ready.
		///		The local action is updated before the client actions
		void UpdateExecution();

        /// \brief Ends the current local action gracefully.
        void EndLocalAction();

        /// \brief End an action that a remote machine requested.
        /// \param [in] index  Specify an index (i.e. player ID)
        void EndClientAction(uint8_t _index);

        /// \brief Pass action message to the current action to be handled.
        void HandleActionInfo(uint8_t _sender, uint8_t _messageType, const std::string& _message);

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
        static ActionPool* m_instance;

        /// \brief List of all actions in the game
        std::vector<Action*> m_actions;

        /// \brief Array of current client action(s).
        Action* m_clientActions[24];

        /// \brief Current local action.
        Action* m_localAction;

        /// \brief Current default action that will be executed on left click.
        Action* m_currentDefaultAction;

        /// \brief Last object that was used for the default action check.
        Core::ObjectID m_lastDefaultActionTarget;

        /// \brief Typedef for information about a local action.
        typedef std::tuple<Core::ActionID, Core::ObjectID, Core::ObjectID> LocalActionInfo;

        /// \brief Queue for new local actions to be started when the current one is finished.
        /// \details Save the ActionID, the executor's ObjectID and the target's ObjectID in 
        ///     this particular order.
        std::queue<LocalActionInfo> m_localActionQueue;
    };
}