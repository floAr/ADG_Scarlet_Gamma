#pragma once

#include "Property.hpp"

namespace Core {

	struct PROPERTY
	{
		static const Property NAME;
		static const Property SPRITE;
		static const Property X;
		static const Property Y;
		static const Property LAYER;
		static const Property COLOR;
		static const Property PATH;
		static const Property TARGET;
		static const Property OBSTACLE;
		static const Property PLAYER;
		static const Property JUMPPOINT;

		static const Property INVENTORY;
		static const Property STRENGTH;
		static const Property DEXTERITY;
		static const Property CONSTITUTION;
		static const Property INTELLIGENCE;
		static const Property WISDOM;
		static const Property CHARISMA;
		static const Property HEALTH;
		static const Property ARMORCLASS;

		// A custom property with R_VCEV0EV00 access rights and an empty default.
		static Property Custom( const std::string& _name )
		{
			// TODO: obsolete parameter 1: 0
			return Core::Property(Core::Property::R_VCEV0EV00, _name, "");
		}
	};

} // namespace Core