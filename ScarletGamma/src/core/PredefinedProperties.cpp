

#include "PredefinedProperties.hpp"
#include "Constants.hpp"

namespace Core {

	const Property PROPERTY::NAME = Property(Property::R_VCEV00V00, STR_PROP_NAME, STR_EMPTY);
	const Property PROPERTY::SPRITE = Property(Property::R_V0E000000, STR_PROP_SPRITE, STR_EMPTY);
	const Property PROPERTY::X = Property(Property::R_V0E000000, STR_PROP_X, STR_0);
	const Property PROPERTY::Y = Property(Property::R_V0E000000, STR_PROP_Y, STR_0 );
	const Property PROPERTY::LAYER = Property(Property::R_V0E000000, STR_PROP_LAYER, STR_0);
	const Property PROPERTY::COLOR = Property(Property::R_VCEV0EV00, STR_PROP_COLOR, STR_FFFFFFFF);
	const Property PROPERTY::PATH = Property(Property::R_SYSTEMONLY, STR_PROP_PATH, STR_FALSE);
	const Property PROPERTY::TARGET = Property(Property::R_SYSTEMONLY, STR_PROP_TARGET, "0:0");
	const Property PROPERTY::OBSTACLE = Property(Property::R_VC0000000, STR_PROP_OBSTACLE, STR_EMPTY);
	const Property PROPERTY::PLAYER = Property(Property::R_SYSTEMONLY, STR_PROP_PLAYER, STR_0);
	const Property PROPERTY::JUMPPOINT = Property(Property::R_VCE000000, STR_PROP_JUMPPOINT, STR_0);
	const Property PROPERTY::OWNER = Property(Property::R_VCEV0E000, STR_PROP_OWNER, STR_EMPTY);

	const Property PROPERTY::INVENTORY = Property(Property::R_VCEV0EV00, STR_PROP_INVENTORY, STR_EMPTY);
	const Property PROPERTY::STRENGTH = Property(Property::R_VCEV0EV00, STR_PROP_STRENGTH, STR_0);
	const Property PROPERTY::STRENGTH_MOD = Property(Property::R_VCEV0EV00, STR_PROP_STRENGTH_MOD, "('ST'-10)/2");
	const Property PROPERTY::DEXTERITY = Property(Property::R_VCEV0EV00, STR_PROP_DEXTERITY, STR_0);
	const Property PROPERTY::DEXTERITY_MOD = Property(Property::R_VCEV0EV00, STR_PROP_DEXTERITY_MOD, "('GE'-10)/2");
	const Property PROPERTY::CONSTITUTION = Property(Property::R_VCEV0EV00, STR_PROP_CONSTITUTION, STR_0);
	const Property PROPERTY::CONSTITUTION_MOD = Property(Property::R_VCEV0EV00, STR_PROP_CONSTITUTION_MOD, "('KO'-10)/2");
	const Property PROPERTY::INTELLIGENCE = Property(Property::R_VCEV0EV00, STR_PROP_INTELLIGENCE, STR_0);
	const Property PROPERTY::INTELLIGENCE_MOD = Property(Property::R_VCEV0EV00, STR_PROP_INTELLIGENCE_MOD, "('IN'-10)/2");
	const Property PROPERTY::WISDOM = Property(Property::R_VCEV0EV00, STR_PROP_WISDOM, STR_0 );
	const Property PROPERTY::WISDOM_MOD = Property(Property::R_VCEV0EV00, STR_PROP_WISDOM_MOD, "('WE'-10)/2" );
	const Property PROPERTY::CHARISMA = Property(Property::R_VCEV0EV00, STR_PROP_CHARISMA, STR_0);
	const Property PROPERTY::CHARISMA_MOD = Property(Property::R_VCEV0EV00, STR_PROP_CHARISMA_MOD, "('CH'-10)/2");
	const Property PROPERTY::HEALTH = Property(Property::R_VCEV0EV00, STR_PROP_HEALTH, STR_0);
	const Property PROPERTY::HEALTH_MAX = Property(Property::R_VCEV0EV00, STR_PROP_HEALTH_MAX, STR_0);
    const Property PROPERTY::ARMORCLASS = Property(Property::R_VCEV0EV00, STR_PROP_ARMORCLASS, "10 + 'GE-Mod'");
	const Property PROPERTY::INITIATIVE_MOD = Property(Property::R_VCEV0EV00, STR_PROP_INITIATIVE_MOD, STR_0);
	const Property PROPERTY::EXPERIENCE = Property(Property::R_VCEV0EV00, STR_PROP_EXPERIENCE, STR_0);
	const Property PROPERTY::LEVEL = Property(Property::R_VCEV0EV00, STR_PROP_LEVEL, STR_0);
	const Property PROPERTY::SPEED = Property(Property::R_VCEV0EV00, STR_PROP_SPEED, STR_0);

	const Property PROPERTY::ATTITUDE = Property(Property::R_VCEV0EV00, STR_PROP_ATTITUDE, STR_ATT_LAWFULGOOD);
	const Property PROPERTY::CLASS = Property(Property::R_VCEV0EV00, STR_PROP_CLASS, STR_EMPTY);
	const Property PROPERTY::FOLK = Property(Property::R_VCEV0EV00, STR_PROP_FOLK, STR_EMPTY);
	const Property PROPERTY::HOME = Property(Property::R_VCEV0EV00, STR_PROP_HOME, STR_EMPTY);
	const Property PROPERTY::FAITH = Property(Property::R_VCEV0EV00, STR_PROP_FAITH, STR_EMPTY);
	const Property PROPERTY::SIZE = Property(Property::R_VCEV0EV00, STR_PROP_SIZE, STR_0);
	const Property PROPERTY::AGE = Property(Property::R_VCEV0EV00, STR_PROP_AGE, STR_0);
	const Property PROPERTY::SEX = Property(Property::R_VCEV0EV00, STR_PROP_SEX, STR_SEX_M);
	const Property PROPERTY::WEIGHT = Property(Property::R_VCEV0EV00, STR_PROP_WEIGHT, STR_0);
	const Property PROPERTY::HAIRCOLOR = Property(Property::R_VCEV0EV00, STR_PROP_HAIRCOLOR, STR_FFFFFFFF);
	const Property PROPERTY::EYECOLOR = Property(Property::R_VCEV0EV00, STR_PROP_EYECOLOR, STR_FFFFFFFF);

	const Property PROPERTY::TALENTS = Property(Property::R_VC0V00V00, STR_PROP_TALENTS, STR_EMPTY);
	const Property PROPERTY::ACROBATICS = Property(Property::R_VCEVCEV00, STR_PROP_ACROBATICS, "'GE-Mod' + 0");
	const Property PROPERTY::DEMEANOR = Property(Property::R_VCEVCEV00, STR_PROP_DEMEANOR, "'CH-Mod' + 0");
	const Property PROPERTY::PROFESSION = Property(Property::R_VCEVCEV00, STR_PROP_PROFESSION, "'WE-Mod' + 0");
	const Property PROPERTY::BLUFFING = Property(Property::R_VCEVCEV00, STR_PROP_BLUFFING, "'CH-Mod' + 0");
	const Property PROPERTY::DIPLOMACY = Property(Property::R_VCEVCEV00, STR_PROP_DIPLOMACY, "'CH-Mod' + 0");
	const Property PROPERTY::INTIMIDATE = Property(Property::R_VCEVCEV00, STR_PROP_INTIMIDATE, "'CH-Mod' + 0");
	const Property PROPERTY::UNLEASHING = Property(Property::R_VCEVCEV00, STR_PROP_UNLEASHING, "'GE-Mod' + 0");
	const Property PROPERTY::PRESTIDIGITATION = Property(Property::R_VCEVCEV00, STR_PROP_PRESTIDIGITATION, "'GE-Mod' + 0");
	const Property PROPERTY::FLYING = Property(Property::R_VCEVCEV00, STR_PROP_FLYING, "'GE-Mod' + 0");
	const Property PROPERTY::CRAFT = Property(Property::R_VCEVCEV00, STR_PROP_CRAFT, "'IN-Mod' + 0");
	const Property PROPERTY::MEDICINE = Property(Property::R_VCEVCEV00, STR_PROP_MEDICINE, "'WE-Mod' + 0");
	const Property PROPERTY::STEALTH = Property(Property::R_VCEVCEV00, STR_PROP_STEALTH, "'GE-Mod' + 0");
	const Property PROPERTY::CLIMBING = Property(Property::R_VCEVCEV00, STR_PROP_CLIMBING, "'ST-Mod' + 0");
	const Property PROPERTY::MAGICUSING = Property(Property::R_VCEVCEV00, STR_PROP_MAGICUSING, "'CH-Mod' + 0");
	const Property PROPERTY::DEACTIVATE = Property(Property::R_VCEVCEV00, STR_PROP_DEACTIVATE, "'GE-Mod' + 0");
	const Property PROPERTY::ANIMALDEALING = Property(Property::R_VCEVCEV00, STR_PROP_ANIMALDEALING, "'CH-Mod' + 0");
	const Property PROPERTY::MOTIVERECOGNITION = Property(Property::R_VCEVCEV00, STR_PROP_MOTIVERECOGNITION, "'WE-Mod' + 0");
	const Property PROPERTY::RIDING = Property(Property::R_VCEVCEV00, STR_PROP_RIDING, "'GE-Mod' + 0");
	const Property PROPERTY::ESTIMATE = Property(Property::R_VCEVCEV00, STR_PROP_ESTIMATE, "'IN-Mod' + 0");
	const Property PROPERTY::SWIMMING = Property(Property::R_VCEVCEV00, STR_PROP_SWIMMING, "'ST-Mod' + 0");
	const Property PROPERTY::LANGUAGEUNDERSTANDING = Property(Property::R_VCEVCEV00, STR_PROP_LANGUAGEUNDERSTANDING, "'IN-Mod' + 0");
	const Property PROPERTY::SURVIVALING = Property(Property::R_VCEVCEV00, STR_PROP_SURVIVALING, "'WE-Mod' + 0");
	const Property PROPERTY::MASQUERADING = Property(Property::R_VCEVCEV00, STR_PROP_MASQUERADING, "'CH-Mod' + 0");
	const Property PROPERTY::PERCEPTION = Property(Property::R_VCEVCEV00, STR_PROP_PERCEPTION, "'WE-Mod' + 0");
	const Property PROPERTY::KNOWLEDGE = Property(Property::R_VCEVCEV00, STR_PROP_KNOWLEDGE, "'IN-Mod' + 0");
	const Property PROPERTY::MAGICART = Property(Property::R_VCEVCEV00, STR_PROP_MAGICART, "'IN-Mod' + 0");

} // namespace Core