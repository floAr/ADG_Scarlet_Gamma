#include "WalkTo.hpp"
#include "Constants.hpp"
#include "../core/Object.hpp"
#include "network/ActionMessages.hpp"

using namespace Actions;

WalkTo::WalkTo() : Action(STR_ACT_WALKTO)
{
    // Set requirements
	m_Requirements.push_back(Core::Object::PROP_X);
    m_Requirements.push_back(Core::Object::PROP_Y);
}

void WalkTo::Execute()
{
    // This functions is called when a player clicks "Attack" on an object
    // TODO: Enable DM-side attacking!!

    // Tell the server that we are starting an attack
    Network::MsgActionBegin message(this->GetID());
    message.Send();
   

    //////////////////////////////////////////////////////////////////////////
    // all TODO below!
    //////////////////////////////////////////////////////////////////////////

    // 2)  Show a dialog to the player, asking for the Hit Roll values
    // TODO Show a dialog to the player
    // Assuming some given string for now

    // 3)  Send the entered values to the Game Master
    //     Server side is handled elsewhere I hope?!
    //
    //     Server replies:
    // 4a) MISSED, were done. Kill the action!
    // 4b) CRIT CHANCE, roll again and send it to the server
    // 4c) HIT, enter damage (remember wether we did critical damage to show
    //     message "damage is auto-doubled") and send it to the server
    //
    // done!
}

Action* WalkTo::Clone()
{
    WalkTo* result = new WalkTo();

    // TODO: copy all values

    return dynamic_cast<Action*>(result);
}