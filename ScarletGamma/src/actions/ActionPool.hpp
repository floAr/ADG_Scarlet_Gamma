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
        ActionPool& Instance()
        {
            if (!m_Instance)
                m_Instance = new ActionPool();
            return *m_Instance;
        }

        /// \brief Returns all actions which are allowed to be executed on a given object.
        /// \param [in] object  Object to be examined
        /// \returns Vector of unique Action pointers for which the actions are applicable
        std::vector<Action*> GetAllowedActions(Core::Object& object);
                
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
    };
}