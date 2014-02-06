

#include "PredefinedProperties.hpp"
#include "Constants.hpp"

namespace Core {

	const Property PROPERTY::NAME = Property(Property::R_VCEV0EV00, STR_PROP_NAME, STR_EMPTY);
	const Property PROPERTY::SPRITE = Property(Property::R_V0E000000, STR_PROP_SPRITE, STR_EMPTY);
	const Property PROPERTY::X = Property(Property::R_V0EV00000, STR_PROP_X, STR_0);
	const Property PROPERTY::Y = Property(Property::R_V0EV00000, STR_PROP_Y, STR_0 );
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
	const Property PROPERTY::DEXTERITY = Property(Property::R_VCEV0EV00, STR_PROP_DEXTERITY, STR_0);
	const Property PROPERTY::CONSTITUTION = Property(Property::R_VCEV0EV00, STR_PROP_CONSTITUTION, STR_0);
	const Property PROPERTY::INTELLIGENCE = Property(Property::R_VCEV0EV00, STR_PROP_INTELLIGENCE, STR_0);
	const Property PROPERTY::WISDOM = Property(Property::R_VCEV0EV00, STR_PROP_WISDOM, STR_0 );
	const Property PROPERTY::CHARISMA = Property(Property::R_VCEV0EV00, STR_PROP_CHARISMA, STR_0);
	const Property PROPERTY::HEALTH = Property(Property::R_VCEV00000, STR_PROP_HEALTH, STR_0);
    const Property PROPERTY::ARMORCLASS = Property(Property::R_VCEV00000, STR_PROP_ARMORCLASS, STR_0);
	const Property PROPERTY::INITIATIVE_MOD = Property(Property::R_VCEV00000, STR_PROP_INITIATIVE_MOD, STR_0);

} // namespace Core