#pragma once

static const unsigned int TILESIZE = 64;

// A pool of string objects to avoid creation and deletion of many strings
#include <string>
static const std::string STR_EMPTY = std::string("");
static const std::string STR_TRUE = std::string("true");
static const std::string STR_FALSE = std::string("false");
static const std::string STR_ID = std::string("ID");
static const std::string STR_PROPERTIES = std::string("Properties");
static const std::string STR_PARENT = std::string("parent");
static const std::string STR_OBJECTS = std::string("objects");
static const std::string STR_RIGHTS = std::string("rights");
static const std::string STR_0 = std::string("0");
static const std::string STR_FFFFFFFF = std::string("ffffffff");
static const std::string STR_MULTISELECTION = std::string("<Mehrfachauswahl>");



/************************************************************************/
/* ACTIONS                                                              */
/************************************************************************/

static const std::string STR_ACT_ATTACK = std::string("Angreifen");
static const std::string STR_ACT_WALKTO = std::string("Hierher gehen");

/************************************************************************/
/* MESSAGES                                                             */
/************************************************************************/
// Attack messages
static const std::string STR_MSG_HIT_NO_ARMOR_CLASS = std::string("Das Ziel hat keine Rüstungsklasse.");
static const std::string STR_MSG_HIT_NATURAL_TWENTY = std::string("Natürliche 20 gewürfelt.");
static const std::string STR_MSG_HIT_ARMOR_CLASS_BEAT = std::string("Rüstungsklasse wurde geschlagen.");
static const std::string STR_MSG_HIT_ARMOR_CLASS_NOT_BEAT = std::string("Rüstungsklasse wurde nicht geschlagen.");


/************************************************************************/
/* GENERAL PROPERTIES                                                   */
/************************************************************************/
static const std::string STR_PROP_LAYER		= std::string("Layer");		///< Name of layer property: rendering order
static const std::string STR_PROP_X			= std::string("X");			///< Name of position.x: floating point position
static const std::string STR_PROP_Y			= std::string("Y");			///< Name of position.y: floating point position
static const std::string STR_PROP_SPRITE	= std::string("Bild");		///< Name of sprite property: texture name
static const std::string STR_PROP_COLOR		= std::string("Farbe");		///< Name of color property: hexadecimal string
static const std::string STR_PROP_PATH		= std::string("path");		///< Name of path property: object list + boolean value
static const std::string STR_PROP_TARGET	= std::string("target");	///< Name of target property: next point to be reached linearly
static const std::string STR_PROP_OBSTACLE	= std::string("Hindernis");	///< Name of obstacle property: this object collides with the player
static const std::string STR_PROP_NAME		= std::string("Name");		///< Name of name property: object's name
static const std::string STR_PROP_PLAYER	= std::string("player");	///< Name of player: flags an object as player, the value is its ID


/************************************************************************/
/* PLAYER ATTRIBUTES                                                    */
/************************************************************************/
static const std::string STR_PROP_INVENTORY = std::string("Inventar");
static const std::string STR_PROP_STRENGTH = std::string("Stärke");
static const std::string STR_PROP_DEXTERITY = std::string("Geschicklichkeit");
static const std::string STR_PROP_CONSTITUTION = std::string("Konstitution");
static const std::string STR_PROP_INTELLIGENCE = std::string("Intelligenz");
static const std::string STR_PROP_WISDOM = std::string("Weisheit");
static const std::string STR_PROP_CHARISMA = std::string("Charisma");
static const std::string STR_PROP_HEALTH = std::string("Trefferpunkte");


/************************************************************************/
/* MODULES			                                                    */
/************************************************************************/
static const std::string STR_ATTACKABLE = std::string("Angreifbar");


/************************************************************************/
/* TEMPLATES		                                                    */
/************************************************************************/
static const std::string STR_GOBBO = std::string("Gobbo");
static const std::string STR_WALLH = std::string("Wand_H");
static const std::string STR_WALLV = std::string("Wand_V");
static const std::string STR_WALLC = std::string("Wand_K");