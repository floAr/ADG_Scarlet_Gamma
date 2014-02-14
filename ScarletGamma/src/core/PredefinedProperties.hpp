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
		static const Property OWNER;
		static const Property SWITCH;

		static const Property INVENTORY;
		static const Property STRENGTH;
		static const Property STRENGTH_MOD;
		static const Property DEXTERITY;
		static const Property DEXTERITY_MOD;
		static const Property CONSTITUTION;
		static const Property CONSTITUTION_MOD;
		static const Property INTELLIGENCE;
		static const Property INTELLIGENCE_MOD;
		static const Property WISDOM;
		static const Property WISDOM_MOD;
		static const Property CHARISMA;
		static const Property CHARISMA_MOD;
		static const Property HEALTH;
		static const Property HEALTH_MAX;
		static const Property ARMORCLASS;
		static const Property INITIATIVE_MOD;
		static const Property EXPERIENCE;
		static const Property LEVEL;
		static const Property SPEED;
		static const Property REFLEX;
		static const Property WILL;
		static const Property RESILENCE;
		static const Property BASIC_ATTACK;

		static const Property ATTITUDE;
		static const Property CLASS;
		static const Property FOLK;
		static const Property HOME;
		static const Property FAITH;
		static const Property SIZE;
		static const Property AGE;
		static const Property SEX;
		static const Property WEIGHT;
		static const Property HAIRCOLOR;
		static const Property EYECOLOR;

		static const Property TALENTS;
		static const Property ACROBATICS;
		static const Property DEMEANOR;
		static const Property PROFESSION;
		static const Property BLUFFING;
		static const Property DIPLOMACY;
		static const Property INTIMIDATE;
		static const Property UNLEASHING;
		static const Property PRESTIDIGITATION;
		static const Property FLYING;
		static const Property CRAFT;
		static const Property MEDICINE;
		static const Property STEALTH;
		static const Property CLIMBING;
		static const Property MAGICUSING;
		static const Property DEACTIVATE;
		static const Property ANIMALDEALING;
		static const Property MOTIVERECOGNITION;
		static const Property RIDING;
		static const Property ESTIMATE;
		static const Property SWIMMING;
		static const Property LANGUAGEUNDERSTANDING;
		static const Property SURVIVALING;
		static const Property MASQUERADING;
		static const Property PERCEPTION;
		static const Property KNOWLEDGE;
		static const Property MAGICART;


		// A custom property with R_VCEV0EV00 access rights and an empty default.
		static Property Custom( const std::string& _name )
		{
			// TODO: obsolete parameter 1: 0
			return Core::Property(Core::Property::R_VCEV0EV00, _name, "");
		}
	};

} // namespace Core