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
/* PLAYER ATTRIBUTES                                                    */
/************************************************************************/
static const std::string STR_INVENTORY = std::string("Inventar");
static const std::string STR_STRENGTH = std::string("Stärke");
static const std::string STR_DEXTERITY = std::string("Geschicklichkeit");
static const std::string STR_CONSTITUTION = std::string("Konstitution");
static const std::string STR_INTELLIGENCE = std::string("Intelligenz");
static const std::string STR_WISDOM = std::string("Weisheit");
static const std::string STR_CHARISMA = std::string("Charisma");
static const std::string STR_HEALTH = std::string("Trefferpunkte");


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