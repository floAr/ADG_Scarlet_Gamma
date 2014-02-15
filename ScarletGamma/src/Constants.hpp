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
static const std::string STR_1 = std::string("1");
static const std::string STR_2 = std::string("2");
static const std::string STR_3 = std::string("3");
static const std::string STR_4 = std::string("4");
static const std::string STR_FFFFFFFF = std::string("ffffffff");
static const std::string STR_MULTISELECTION = std::string("<Mehrfachauswahl>");



/************************************************************************/
/* ACTIONS                                                              */
/************************************************************************/

static const std::string STR_ACT_ATTACK = std::string("Angreifen");
static const std::string STR_ACT_WALKTO = std::string("Hierher gehen");
static const std::string STR_ACT_SETJUMPPOINT = std::string("Als Sprungziel");
static const std::string STR_ACT_USEJUMPPOINT = std::string("Betreten");
static const std::string STR_ACT_USESWITCH = std::string("Betätigen");
static const std::string STR_ACT_TAKEOBJECT = std::string("Nehmen");

/************************************************************************/
/* MESSAGES                                                             */
/************************************************************************/
// Attack messages
static const std::string STR_MSG_HIT_NO_ARMOR_CLASS = std::string("Das Ziel hat keine Rüstungsklasse.");
static const std::string STR_MSG_HIT_NATURAL_TWENTY = std::string("Natürliche 20 gewürfelt.");
static const std::string STR_MSG_HIT_ARMOR_CLASS_BEAT = std::string("Rüstungsklasse wurde geschlagen.");
static const std::string STR_MSG_HIT_ARMOR_CLASS_NOT_BEAT = std::string("Rüstungsklasse wurde nicht geschlagen.");
static const std::string STR_MSG_CREATE_MAP_FIRST = std::string("Erstelle zunächst eine Map!");
static const std::string STR_MSG_SELECT_TYPE_FIRST = std::string("Wähle einen Objekttypen! Dazu auf eines der Templates klicken.");
static const std::string STR_MSG_LAYER_INVISIBLE = std::string("Die Ziel-Ebene ist ausgeblendet. Drücke ALT + Zahlentaste um umzuschalten.");
static const std::string STR_PLAYER_NOT_ON_MAP = std::string("Der Charakter befindet sich nicht auf\neiner Karte.\n\nRespawn notwendig.");
static const std::string STR_AMBIGIOUS_DRAG = std::string("Objekte können nicht in eine Multiselektion verschoben werden.");
static const std::string STR_AMBIGIOUS_DROP_TARGET = std::string("Ein Objekt muss auf eine Eigenschaft gezogen werden.");


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
static const std::string STR_PROP_OWNER		= std::string("Besitzer");	///< Name of owner property: flags the object as owned by a player
static const std::string STR_PROP_SWITCH	= std::string("Schalter");
static const std::string STR_PROP_JUMPPOINT	= std::string("Sprungziel");///< Name of portal property: The value is an id of the target object.
static const std::string STR_PROP_ITEM		= std::string("Item");


/************************************************************************/
/* PLAYER ATTRIBUTES                                                    */
/************************************************************************/
static const std::string STR_PROP_INVENTORY = std::string("Inventar");
static const std::string STR_PROP_STRENGTH = std::string("ST");
static const std::string STR_PROP_STRENGTH_MOD = std::string("ST-Mod");
static const std::string STR_PROP_DEXTERITY = std::string("GE");
static const std::string STR_PROP_DEXTERITY_MOD = std::string("GE-Mod");
static const std::string STR_PROP_CONSTITUTION = std::string("KO");
static const std::string STR_PROP_CONSTITUTION_MOD = std::string("KO-Mod");
static const std::string STR_PROP_INTELLIGENCE = std::string("IN");
static const std::string STR_PROP_INTELLIGENCE_MOD = std::string("IN-Mod");
static const std::string STR_PROP_WISDOM = std::string("WE");
static const std::string STR_PROP_WISDOM_MOD = std::string("WE-Mod");
static const std::string STR_PROP_CHARISMA = std::string("CH");
static const std::string STR_PROP_CHARISMA_MOD = std::string("CH-Mod");
static const std::string STR_PROP_HEALTH = std::string("TP");
static const std::string STR_PROP_HEALTH_MAX = std::string("TP-Max");
static const std::string STR_PROP_ARMORCLASS = std::string("RK");
static const std::string STR_PROP_INITIATIVE_MOD = std::string("INI-Mod");
static const std::string STR_PROP_ATTITUDE = std::string("Gesinnung");
static const std::string STR_ATT_LAWFULGOOD = std::string("Rechtschaffen Gut");
static const std::string STR_ATT_NEUTRALGOOD = std::string("Neutral Gut");
static const std::string STR_ATT_CHAOTICGOOD = std::string("Chaotisch Gut");
static const std::string STR_ATT_LAWFULNEUTRAL = std::string("Rechtschaffen Neutral");
static const std::string STR_ATT_NEUTRALNEUTRAL = std::string("Neutral");
static const std::string STR_ATT_CHAOTICNEUTRAL = std::string("Chaotisch Neutral");
static const std::string STR_ATT_LAWFULEVIL = std::string("Rechtschaffen Böse");
static const std::string STR_ATT_NEUTRALEVIL = std::string("Neutral Böse");
static const std::string STR_ATT_CHAOTICEVIL = std::string("Chaotisch Böse");
static const std::string STR_PROP_CLASS = std::string("Klasse");
static const std::string STR_PROP_FOLK = std::string("Volk");
static const std::string STR_PROP_HOME = std::string("Heimat");
static const std::string STR_PROP_FAITH = std::string("Gottheit");
static const std::string STR_PROP_SIZE = std::string("Größe");
static const std::string STR_PROP_AGE = std::string("Alter");
static const std::string STR_PROP_SEX = std::string("Geschlecht");
static const std::string STR_SEX_M = std::string("M");
static const std::string STR_SEX_F = std::string("W");
static const std::string STR_PROP_WEIGHT = std::string("Gewicht");
static const std::string STR_PROP_HAIRCOLOR = std::string("Haarfarbe");
static const std::string STR_PROP_EYECOLOR = std::string("Augenfarbe");
static const std::string STR_PROP_EXPERIENCE = std::string("Erfahrung");
static const std::string STR_PROP_LEVEL = std::string("Stufe");
static const std::string STR_PROP_SPEED = std::string("Bewegungsrate");
static const std::string STR_PROP_REFLEX = std::string("Reflex");
static const std::string STR_PROP_WILL = std::string("Willen");
static const std::string STR_PROP_RESILENCE = std::string("Zähigkeit");
static const std::string STR_PROP_BASIC_ATTACK = std::string("Grundangriff");

static const std::string STR_PROP_TALENTS = std::string("Fertigkeiten");
static const std::string STR_PROP_ACROBATICS = std::string("Akrobatik");
static const std::string STR_PROP_DEMEANOR = std::string("Auftreten ---");
static const std::string STR_PROP_PROFESSION = std::string("Beruf ---*");
static const std::string STR_PROP_BLUFFING = std::string("Bluffen");
static const std::string STR_PROP_DIPLOMACY = std::string("Diplomatie");
static const std::string STR_PROP_INTIMIDATE = std::string("Einschüchtern");
static const std::string STR_PROP_UNLEASHING = std::string("Enfesslungskunst");
static const std::string STR_PROP_PRESTIDIGITATION = std::string("Fingerfertigkeit*");
static const std::string STR_PROP_FLYING = std::string("Fliegen");
static const std::string STR_PROP_CRAFT = std::string("Handwerk ---");
static const std::string STR_PROP_MEDICINE = std::string("Heilkunde ---");
static const std::string STR_PROP_STEALTH = std::string("Heimlichkeit");
static const std::string STR_PROP_CLIMBING = std::string("Klettern");
static const std::string STR_PROP_MAGICUSING = std::string("Magiekunde*");
static const std::string STR_PROP_DEACTIVATE = std::string("Deaktivieren*");
static const std::string STR_PROP_ANIMALDEALING = std::string("Tierumgang*");
static const std::string STR_PROP_MOTIVERECOGNITION = std::string("Motiv erkennen");
static const std::string STR_PROP_RIDING = std::string("Reiten");
static const std::string STR_PROP_ESTIMATE = std::string("Schätzen");
static const std::string STR_PROP_SWIMMING = std::string("Schwimmen");
static const std::string STR_PROP_LANGUAGEUNDERSTANDING = std::string("Sprachkunde*");
static const std::string STR_PROP_SURVIVALING = std::string("Überlebenskunst");
static const std::string STR_PROP_MASQUERADING = std::string("Verkleiden");
static const std::string STR_PROP_PERCEPTION = std::string("Wahrnehmung");
static const std::string STR_PROP_KNOWLEDGE = std::string("Wissen ---*");
static const std::string STR_PROP_MAGICART = std::string("Zauberkunde*");


/************************************************************************/
/* MODULES			                                                    */
/************************************************************************/
static const std::string STR_ATTACKABLE = std::string("Angreifbar");
static const std::string STR_JUMPPOINT = std::string("Spungmarke");


/************************************************************************/
/* TEMPLATES		                                                    */
/************************************************************************/
static const std::string STR_GOBBO = std::string("Gobbo");
static const std::string STR_WALLH = std::string("Wand_H");
static const std::string STR_WALLV = std::string("Wand_V");
static const std::string STR_WALLC = std::string("Wand_K");
static const std::string STR_STAIRS = std::string("Treppe");
static const std::string STR_PLANKS = std::string("Holz");
static const std::string STR_EARTH = std::string("Erde");
static const std::string STR_GRASS = std::string("Gras");
static const std::string STR_WATER = std::string("Wasser");


/************************************************************************/
/* GUI				                                                    */
/************************************************************************/
static const std::string STR_MAP = std::string("Map");
static const std::string STR_MODE = std::string("Modus");
static const std::string STR_BRUSH = std::string("Pinsel");
static const std::string STR_PLAYER = std::string("Spieler");
static const std::string STR_NPC = std::string("NSC");
static const std::string STR_ACTION = std::string("Aktion");
static const std::string STR_DIAMETER = std::string("Größe");
static const std::string STR_ADD = std::string("Hinzufügen");
static const std::string STR_REPLACE = std::string("Ersetzen");
static const std::string STR_REMOVE = std::string("Löschen");
static const std::string STR_LAYER0 = std::string("1: Boden");
static const std::string STR_LAYER1 = std::string("2: Statisches I");
static const std::string STR_LAYER2 = std::string("3: Statisches II");
static const std::string STR_LAYER3 = std::string("4: Gegenstände");
static const std::string STR_LAYER4 = std::string("5: Magisches");
static const std::string STR_LAYER5 = std::string("6: Freunde");
static const std::string STR_LAYER6 = std::string("7: Neutral");
static const std::string STR_LAYER7 = std::string("8: Feinde");
static const std::string STR_LAYER8 = std::string("9: Eigenes I");
static const std::string STR_LAYER9 = std::string("0: Eigenes II");
static const std::string STR_ONTOP = std::string("Obenauf");
static const std::string STR_SELECTION = std::string("Selektion");
static const std::string STR_CREATE = std::string("Erstellen");
static const std::string STR_CANCEL = std::string("Abbrechen");