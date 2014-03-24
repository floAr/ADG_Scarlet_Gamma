#pragma once

extern const unsigned int TILESIZE;
extern const sf::Color MIGHTY_SLATE;

// A pool of string objects to avoid creation and deletion of many strings
#include <string>
extern const std::string STR_EMPTY;
extern const std::string STR_TRUE;
extern const std::string STR_FALSE;
extern const std::string STR_ID;
extern const std::string STR_PROPERTIES;
extern const std::string STR_PARENT;
extern const std::string STR_OBJECTS;
extern const std::string STR_RIGHTS;
extern const std::string STR_0;
extern const std::string STR_1;
extern const std::string STR_2;
extern const std::string STR_3;
extern const std::string STR_4;
extern const std::string STR_5;
extern const std::string STR_6;
extern const std::string STR_7;
extern const std::string STR_8;
extern const std::string STR_9;
extern const std::string STR_FFFFFFFF;
extern const std::string STR_MULTISELECTION;
extern const std::string STR_PAUSE;

extern const std::string STR_ON;
extern const std::string STR_OFF;


/************************************************************************/
/* ACTIONS                                                              */
/************************************************************************/

extern const std::string STR_ACT_ATTACK;
extern const std::string STR_ACT_WALKTO;
extern const std::string STR_ACT_SETJUMPPOINT;
extern const std::string STR_ACT_USEJUMPPOINT;
extern const std::string STR_ACT_USESWITCH;
extern const std::string STR_ACT_TAKEOBJECT;
extern const std::string STR_ACT_EXAMINE;
extern const std::string STR_ACT_FREETEXT;

/************************************************************************/
/* MESSAGES                                                             */
/************************************************************************/
// Attack messages
extern const std::string STR_MSG_HIT_NO_ARMOR_CLASS;
extern const std::string STR_MSG_HIT_NATURAL_TWENTY;
extern const std::string STR_MSG_HIT_ARMOR_CLASS_BEAT;
extern const std::string STR_MSG_HIT_ARMOR_CLASS_NOT_BEAT;
extern const std::string STR_MSG_CREATE_MAP_FIRST;
extern const std::string STR_MSG_SELECT_TYPE_FIRST;
extern const std::string STR_MSG_LAYER_INVISIBLE;
extern const std::string STR_PLAYER_NOT_ON_MAP;
extern const std::string STR_AMBIGIOUS_DRAG;
extern const std::string STR_AMBIGIOUS_DROP_TARGET;
extern const std::string STR_TO_FAR_AWAY;
extern const std::string STR_WRONG_ID;
extern const std::string STR_NO_POSITION;
extern const std::string STR_WRONG_PATH;
extern const std::string STR_MSG_NEW_MAP;
extern const std::string STR_MSG_DELETE_MAP;
extern const std::string STR_MSG_RENAME_MAP;
extern const std::string STR_MSG_EXPORT_MAP;
extern const std::string STR_MSG_IMPORT_MAP;


/************************************************************************/
/* GENERAL PROPERTIES                                                   */
/************************************************************************/
extern const std::string STR_PROP_LAYER;		///< Name of layer property: rendering order
extern const std::string STR_PROP_X;			///< Name of position.x: floating point position
extern const std::string STR_PROP_Y;			///< Name of position.y: floating point position
extern const std::string STR_PROP_SPRITE;		///< Name of sprite property: texture name
extern const std::string STR_PROP_COLOR;		///< Name of color property: hexadecimal string
extern const std::string STR_PROP_PATH;			///< Name of path property: object list + boolean value
extern const std::string STR_PROP_TARGET;		///< Name of target property: next point to be reached linearly
extern const std::string STR_PROP_OBSTACLE;		///< Name of obstacle property: this object collides with the player
extern const std::string STR_PROP_OCCLUDER;
extern const std::string STR_PROP_NAME;			///< Name of name property: object's name
extern const std::string STR_PROP_PLAYER;		///< Name of player: flags an object as player, the value is its ID
extern const std::string STR_PROP_OWNER;		///< Name of owner property: flags the object as owned by a player
extern const std::string STR_PROP_SWITCH;
extern const std::string STR_PROP_JUMPPOINT;	///< Name of portal property: The value is an id of the target object.
extern const std::string STR_PROP_ITEM;
extern const std::string STR_PROP_PAUSE;
extern const std::string STR_PROP_VIEWDISTANCE;


/************************************************************************/
/* PLAYER ATTRIBUTES                                                    */
/************************************************************************/
extern const std::string STR_PROP_INVENTORY;
extern const std::string STR_PROP_STRENGTH;
extern const std::string STR_PROP_STRENGTH_MOD;
extern const std::string STR_PROP_DEXTERITY;
extern const std::string STR_PROP_DEXTERITY_MOD;
extern const std::string STR_PROP_CONSTITUTION;
extern const std::string STR_PROP_CONSTITUTION_MOD;
extern const std::string STR_PROP_INTELLIGENCE;
extern const std::string STR_PROP_INTELLIGENCE_MOD;
extern const std::string STR_PROP_WISDOM;
extern const std::string STR_PROP_WISDOM_MOD;
extern const std::string STR_PROP_CHARISMA;
extern const std::string STR_PROP_CHARISMA_MOD;
extern const std::string STR_PROP_HEALTH;
extern const std::string STR_PROP_HEALTH_MAX;
extern const std::string STR_PROP_ARMORCLASS;
extern const std::string STR_PROP_INITIATIVE_MOD;
extern const std::string STR_PROP_ATTITUDE;
extern const std::string STR_ATT_LAWFULGOOD;
extern const std::string STR_ATT_NEUTRALGOOD;
extern const std::string STR_ATT_CHAOTICGOOD;
extern const std::string STR_ATT_LAWFULNEUTRAL;
extern const std::string STR_ATT_NEUTRALNEUTRAL;
extern const std::string STR_ATT_CHAOTICNEUTRAL;
extern const std::string STR_ATT_LAWFULEVIL;
extern const std::string STR_ATT_NEUTRALEVIL;
extern const std::string STR_ATT_CHAOTICEVIL;
extern const std::string STR_PROP_CLASS;
extern const std::string STR_PROP_FOLK;
extern const std::string STR_PROP_HOME;
extern const std::string STR_PROP_FAITH;
extern const std::string STR_PROP_SIZE;
extern const std::string STR_ATT_SIZE_XS;
extern const std::string STR_ATT_SIZE_S;
extern const std::string STR_ATT_SIZE_M;
extern const std::string STR_ATT_SIZE_L;
extern const std::string STR_ATT_SIZE_XL;
extern const std::string STR_ATT_SIZE_XXL;
extern const std::string STR_PROP_AGE;
extern const std::string STR_PROP_SEX;
extern const std::string STR_SEX_M;
extern const std::string STR_SEX_F;
extern const std::string STR_PROP_WEIGHT;
extern const std::string STR_PROP_HAIRCOLOR;
extern const std::string STR_PROP_EYECOLOR;
extern const std::string STR_PROP_EXPERIENCE;
extern const std::string STR_PROP_LEVEL;
extern const std::string STR_PROP_SPEED;
extern const std::string STR_PROP_REFLEX;
extern const std::string STR_PROP_WILL;
extern const std::string STR_PROP_RESILENCE;
extern const std::string STR_PROP_BASIC_ATTACK;

extern const std::string STR_PROP_TALENTS;
extern const std::string STR_PROP_ACROBATICS;
extern const std::string STR_PROP_DEMEANOR;
extern const std::string STR_PROP_PROFESSION;
extern const std::string STR_PROP_BLUFFING;
extern const std::string STR_PROP_DIPLOMACY;
extern const std::string STR_PROP_INTIMIDATE;
extern const std::string STR_PROP_UNLEASHING;
extern const std::string STR_PROP_PRESTIDIGITATION;
extern const std::string STR_PROP_FLYING;
extern const std::string STR_PROP_CRAFT;
extern const std::string STR_PROP_MEDICINE;
extern const std::string STR_PROP_STEALTH;
extern const std::string STR_PROP_CLIMBING;
extern const std::string STR_PROP_MAGICUSING;
extern const std::string STR_PROP_DEACTIVATE;
extern const std::string STR_PROP_ANIMALDEALING;
extern const std::string STR_PROP_MOTIVERECOGNITION;
extern const std::string STR_PROP_RIDING;
extern const std::string STR_PROP_ESTIMATE;
extern const std::string STR_PROP_SWIMMING;
extern const std::string STR_PROP_LANGUAGEUNDERSTANDING;
extern const std::string STR_PROP_SURVIVALING;
extern const std::string STR_PROP_MASQUERADING;
extern const std::string STR_PROP_PERCEPTION;
extern const std::string STR_PROP_KNOWLEDGE;
extern const std::string STR_PROP_MAGICART;


/************************************************************************/
/* MODULES			                                                    */
/************************************************************************/
extern const std::string STR_ATTACKABLE;
extern const std::string STR_JUMPPOINT;
extern const std::string STR_ATTRIBUTES;
extern const std::string STR_APPEARANCE;


/************************************************************************/
/* TEMPLATES		                                                    */
/************************************************************************/
extern const std::string STR_GOBLIN_NAME;
extern const std::string STR_HOBGOBLIN_NAME;
extern const std::string STR_GNOLL_NAME;
extern const std::string STR_KOBOLD_NAME;
extern const std::string STR_LIZARDFOLK_NAME;
extern const std::string STR_SKELETON_NAME;
extern const std::string STR_CHEST_NAME;
extern const std::string STR_WALLH;
extern const std::string STR_WALLV;
extern const std::string STR_WALLC;
extern const std::string STR_STAIRS;
extern const std::string STR_PLANKS;
extern const std::string STR_EARTH;
extern const std::string STR_GRASS;
extern const std::string STR_WATER;
extern const std::string STR_WELL_NAME;
extern const std::string STR_BASALT;
extern const std::string STR_GRANITE;


/************************************************************************/
/* GUI				                                                    */
/************************************************************************/
extern const std::string STR_MAP;
extern const std::string STR_MODE;
extern const std::string STR_BRUSH;
extern const std::string STR_PLAYER;
extern const std::string STR_NPC;
extern const std::string STR_GOTO;
extern const std::string STR_COMBAT;
extern const std::string STR_ACTION;
extern const std::string STR_DRAGNDROP;
extern const std::string STR_DIAMETER;
extern const std::string STR_ADD;
extern const std::string STR_REPLACE;
extern const std::string STR_REMOVE;
extern const std::string STR_AUTOLAYER;
extern const std::string STR_LAYER1;
extern const std::string STR_LAYER2;
extern const std::string STR_LAYER3;
extern const std::string STR_LAYER4;
extern const std::string STR_LAYER5;
extern const std::string STR_LAYER6;
extern const std::string STR_LAYER7;
extern const std::string STR_LAYER8;
extern const std::string STR_LAYER9;
extern const std::string STR_LAYER10;
extern const std::string STR_SELECTION;
extern const std::string STR_CREATE;
extern const std::string STR_CANCEL;
extern const std::string STR_OK;