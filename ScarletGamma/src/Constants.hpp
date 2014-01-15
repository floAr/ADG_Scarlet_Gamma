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



/************************************************************************/
/* ACTIONS                                                              */
/************************************************************************/

static const std::string STR_ACT_ATTACK = std::string("Attack");

/************************************************************************/
/* MESSAGES                                                             */
/************************************************************************/
// Attack messages
static const std::string STR_MSG_HIT_NO_ARMOR_CLASS = std::string("Target has no Armor Class.");
static const std::string STR_MSG_HIT_NATURAL_TWENTY = std::string("Rolled a natural 20.");
static const std::string STR_MSG_HIT_ARMOR_CLASS_BEAT = std::string("Armor Class was beat.");
static const std::string STR_MSG_HIT_ARMOR_CLASS_NOT_BEAT = std::string("Armor Class was not beat.");


/************************************************************************/
/* PLAYER ATTRIBUTES                                                    */
/************************************************************************/
static const std::string STR_INVENTORY = std::string("Inventar");
static const std::string STR_STRENGTH = std::string("St�rke");
static const std::string STR_DEXTERITY = std::string("Geschicklichkeit");
static const std::string STR_CONSTITUTION = std::string("Konstitution");
static const std::string STR_INTELLIGENCE = std::string("Intelligenz");
static const std::string STR_WISDOM = std::string("Weisheit");
static const std::string STR_CHARISMA = std::string("Charisma");