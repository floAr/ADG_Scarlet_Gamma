#pragma once

#include "Prerequisites.hpp"
#include <cstdint>
#include <list>

namespace GameRules
{
    class Combat
    {
    public:
        /// \brief Adds a participant to the combat.
        /// \detail By default, this function adds a participant to the end of
        ///    the round. You can add a participant counting from the front
        ///    using positive values (0 = before first) or from the back of the
        ///    list using negative values (-1 = after last, -2 = before last...)
        void AddParticipantWithInitiative(Core::ObjectID _object, int8_t _position = -1);

        void PushInitiativePrompt(Core::ObjectID _object);

    protected:
        /// \brief Map of participants, sorted by their order of initiative
        std::list<Core::ObjectID> m_participants;

        /// \brief The object whose move it currently is.
        Core::ObjectID m_currentObject;
    };
}
