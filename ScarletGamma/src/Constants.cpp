#include "SFML\Graphics\Color.hpp"
#include "Constants.hpp"

const unsigned int TILESIZE = 64;
const sf::Color MIGHTY_SLATE = sf::Color(85, 98, 112);

// A pool of string objects to avoid creation and deletion of many strings
const std::string STR_EMPTY("");
const std::string STR_TRUE = std::string("true");
const std::string STR_FALSE = std::string("false");
const std::string STR_ID = std::string("ID");
const std::string STR_PROPERTIES = std::string("Properties");
const std::string STR_PARENT = std::string("parent");
const std::string STR_OBJECTS = std::string("objects");
const std::string STR_RIGHTS = std::string("rights");
const std::string STR_0 = std::string("0");
const std::string STR_1 = std::string("1");
const std::string STR_2 = std::string("2");
const std::string STR_3 = std::string("3");
const std::string STR_4 = std::string("4");
const std::string STR_5 = std::string("5");
const std::string STR_6 = std::string("6");
const std::string STR_7 = std::string("7");
const std::string STR_8 = std::string("8");
const std::string STR_9 = std::string("9");
const std::string STR_FFFFFFFF = std::string("ffffffff");
const std::string STR_MULTISELECTION = std::string("<Mehrfachauswahl>");
const std::string STR_PAUSE = std::string("Spiel pausiert");

const std::string STR_ON = std::string("An");
const std::string STR_OFF = std::string("Aus");


/************************************************************************/
/* ACTIONS                                                              */
/************************************************************************/

const std::string STR_ACT_ATTACK = std::string("Angreifen");
const std::string STR_ACT_WALKTO = std::string("Hierher gehen");
const std::string STR_ACT_SETJUMPPOINT = std::string("Als Sprungziel");
const std::string STR_ACT_USEJUMPPOINT = std::string("Betreten");
const std::string STR_ACT_USESWITCH = std::string("Betätigen");
const std::string STR_ACT_TAKEOBJECT = std::string("Nehmen");
const std::string STR_ACT_EXAMINE = std::string("Untersuchen");
const std::string STR_ACT_FREETEXT = std::string("Freitextaktion");

/************************************************************************/
/* MESSAGES                                                             */
/************************************************************************/
// Attack messages
const std::string STR_MSG_HIT_NO_ARMOR_CLASS = std::string("Das Ziel hat keine Rüstungsklasse.");
const std::string STR_MSG_HIT_NATURAL_TWENTY = std::string("Natürliche 20 gewürfelt.");
const std::string STR_MSG_HIT_ARMOR_CLASS_BEAT = std::string("Rüstungsklasse wurde geschlagen.");
const std::string STR_MSG_HIT_ARMOR_CLASS_NOT_BEAT = std::string("Rüstungsklasse wurde nicht geschlagen.");
const std::string STR_MSG_CREATE_MAP_FIRST = std::string("Erstelle zunächst eine Map!");
const std::string STR_MSG_SELECT_TYPE_FIRST = std::string("Wähle einen Objekttypen! Dazu auf eines der Templates klicken.");
const std::string STR_MSG_LAYER_INVISIBLE = std::string("Die Ziel-Ebene ist ausgeblendet. Drücke ALT + Zahlentaste um umzuschalten.");
const std::string STR_PLAYER_NOT_ON_MAP = std::string("Der Charakter befindet sich nicht auf\neiner Karte.\n\nRespawn notwendig.");
const std::string STR_AMBIGIOUS_DRAG = std::string("Objekte können nicht in eine Multiselektion verschoben werden.");
const std::string STR_AMBIGIOUS_DROP_TARGET = std::string("Ein Objekt muss auf eine Eigenschaft gezogen werden.");
const std::string STR_TO_FAR_AWAY = std::string("Objekt zu weit entfernt.");
const std::string STR_WRONG_ID = std::string("Es gibt kein Objekt mit der gegebenen ID.");
const std::string STR_NO_POSITION = std::string("Das Objekt ist kein gültiges Ziel. Die Position fehlt.");
const std::string STR_WRONG_PATH = std::string("Resourcen müssen im 'media' Ordner liegen.");
const std::string STR_MSG_NEW_MAP = std::string("Name für neue Map:");
const std::string STR_MSG_DELETE_MAP = std::string("Map wirklich löschen?");
const std::string STR_MSG_DELETE_PLAYER = std::string(" wirklich löschen?");
const std::string STR_MSG_RENAME_MAP = std::string("Neuer Name für Map:");
const std::string STR_MSG_EXPORT_MAP = std::string("Dateiname für Export (wird überschrieben):");
const std::string STR_MSG_IMPORT_MAP = std::string("Dateiname für Import:");


/************************************************************************/
/* GENERAL PROPERTIES                                                   */
/************************************************************************/
const std::string STR_PROP_LAYER		= std::string("Layer");		///< Name of layer property: rendering order
const std::string STR_PROP_X			= std::string("X");			///< Name of position.x: floating point position
const std::string STR_PROP_Y			= std::string("Y");			///< Name of position.y: floating point position
const std::string STR_PROP_SPRITE	= std::string("Bild");		///< Name of sprite property: texture name
const std::string STR_PROP_COLOR		= std::string("Farbe");		///< Name of color property: hexadecimal string
const std::string STR_PROP_PATH		= std::string("path");		///< Name of path property: object list + boolean value
const std::string STR_PROP_TARGET	= std::string("target");	///< Name of target property: next point to be reached linearly
const std::string STR_PROP_OBSTACLE	= std::string("Hindernis");	///< Name of obstacle property: this object collides with the player
const std::string STR_PROP_OCCLUDER	= std::string("Verdeckend");
const std::string STR_PROP_NAME		= std::string("Name");		///< Name of name property: object's name
const std::string STR_PROP_PLAYER	= std::string("player");	///< Name of player: flags an object as player, the value is its ID
const std::string STR_PROP_OWNER		= std::string("Besitzer");	///< Name of owner property: flags the object as owned by a player
const std::string STR_PROP_SWITCH	= std::string("Schalter");
const std::string STR_PROP_JUMPPOINT	= std::string("Sprungziel");///< Name of portal property: The value is an id of the target object.
const std::string STR_PROP_ITEM		= std::string("Item");
const std::string STR_PROP_PAUSE		= std::string("__GAME_PAUSED__");
const std::string STR_PROP_VIEWDISTANCE	= std::string("Sichtweite");


/************************************************************************/
/* PLAYER ATTRIBUTES                                                    */
/************************************************************************/
const std::string STR_PROP_INVENTORY = std::string("Inventar");
const std::string STR_PROP_STRENGTH = std::string("ST");
const std::string STR_PROP_STRENGTH_MOD = std::string("ST-Mod");
const std::string STR_PROP_DEXTERITY = std::string("GE");
const std::string STR_PROP_DEXTERITY_MOD = std::string("GE-Mod");
const std::string STR_PROP_CONSTITUTION = std::string("KO");
const std::string STR_PROP_CONSTITUTION_MOD = std::string("KO-Mod");
const std::string STR_PROP_INTELLIGENCE = std::string("IN");
const std::string STR_PROP_INTELLIGENCE_MOD = std::string("IN-Mod");
const std::string STR_PROP_WISDOM = std::string("WE");
const std::string STR_PROP_WISDOM_MOD = std::string("WE-Mod");
const std::string STR_PROP_CHARISMA = std::string("CH");
const std::string STR_PROP_CHARISMA_MOD = std::string("CH-Mod");
const std::string STR_PROP_HEALTH = std::string("TP");
const std::string STR_PROP_HEALTH_MAX = std::string("TP-Max");
const std::string STR_PROP_ARMORCLASS = std::string("RK");
const std::string STR_PROP_INITIATIVE_MOD = std::string("INI-Mod");
const std::string STR_PROP_ATTITUDE = std::string("Gesinnung");
const std::string STR_ATT_LAWFULGOOD = std::string("Rechtschaffen Gut");
const std::string STR_ATT_NEUTRALGOOD = std::string("Neutral Gut");
const std::string STR_ATT_CHAOTICGOOD = std::string("Chaotisch Gut");
const std::string STR_ATT_LAWFULNEUTRAL = std::string("Rechtschaffen Neutral");
const std::string STR_ATT_NEUTRALNEUTRAL = std::string("Neutral");
const std::string STR_ATT_CHAOTICNEUTRAL = std::string("Chaotisch Neutral");
const std::string STR_ATT_LAWFULEVIL = std::string("Rechtschaffen Böse");
const std::string STR_ATT_NEUTRALEVIL = std::string("Neutral Böse");
const std::string STR_ATT_CHAOTICEVIL = std::string("Chaotisch Böse");
const std::string STR_PROP_CLASS = std::string("Klasse");
const std::string STR_PROP_FOLK = std::string("Volk");
const std::string STR_PROP_HOME = std::string("Heimat");
const std::string STR_PROP_FAITH = std::string("Gottheit");
const std::string STR_PROP_SIZE = std::string("Größe");
const std::string STR_ATT_SIZE_XS = std::string("Sehr klein");
const std::string STR_ATT_SIZE_S = std::string("Klein");
const std::string STR_ATT_SIZE_M = std::string("Normal");
const std::string STR_ATT_SIZE_L = std::string("Groß");
const std::string STR_ATT_SIZE_XL = std::string("Riesig");
const std::string STR_ATT_SIZE_XXL = std::string("Gigantisch");
const std::string STR_PROP_AGE = std::string("Alter");
const std::string STR_PROP_SEX = std::string("Geschlecht");
const std::string STR_SEX_M = std::string("M");
const std::string STR_SEX_F = std::string("W");
const std::string STR_PROP_WEIGHT = std::string("Gewicht");
const std::string STR_PROP_HAIRCOLOR = std::string("Haarfarbe");
const std::string STR_PROP_EYECOLOR = std::string("Augenfarbe");
const std::string STR_PROP_EXPERIENCE = std::string("Erfahrung");
const std::string STR_PROP_LEVEL = std::string("Stufe");
const std::string STR_PROP_SPEED = std::string("Bewegungsrate");
const std::string STR_PROP_REFLEX = std::string("Reflex");
const std::string STR_PROP_WILL = std::string("Willen");
const std::string STR_PROP_RESILENCE = std::string("Zähigkeit");
const std::string STR_PROP_BASIC_ATTACK = std::string("Grundangriff");

const std::string STR_PROP_TALENTS = std::string("Fertigkeiten");
const std::string STR_PROP_ACROBATICS = std::string("Akrobatik");
const std::string STR_PROP_DEMEANOR = std::string("Auftreten ---");
const std::string STR_PROP_PROFESSION = std::string("Beruf ---*");
const std::string STR_PROP_BLUFFING = std::string("Bluffen");
const std::string STR_PROP_DIPLOMACY = std::string("Diplomatie");
const std::string STR_PROP_INTIMIDATE = std::string("Einschüchtern");
const std::string STR_PROP_UNLEASHING = std::string("Enfesslungskunst");
const std::string STR_PROP_PRESTIDIGITATION = std::string("Fingerfertigkeit*");
const std::string STR_PROP_FLYING = std::string("Fliegen");
const std::string STR_PROP_CRAFT = std::string("Handwerk ---");
const std::string STR_PROP_MEDICINE = std::string("Heilkunde ---");
const std::string STR_PROP_STEALTH = std::string("Heimlichkeit");
const std::string STR_PROP_CLIMBING = std::string("Klettern");
const std::string STR_PROP_MAGICUSING = std::string("Magiekunde*");
const std::string STR_PROP_DEACTIVATE = std::string("Deaktivieren*");
const std::string STR_PROP_ANIMALDEALING = std::string("Tierumgang*");
const std::string STR_PROP_MOTIVERECOGNITION = std::string("Motiv erkennen");
const std::string STR_PROP_RIDING = std::string("Reiten");
const std::string STR_PROP_ESTIMATE = std::string("Schätzen");
const std::string STR_PROP_SWIMMING = std::string("Schwimmen");
const std::string STR_PROP_LANGUAGEUNDERSTANDING = std::string("Sprachkunde*");
const std::string STR_PROP_SURVIVALING = std::string("Überlebenskunst");
const std::string STR_PROP_MASQUERADING = std::string("Verkleiden");
const std::string STR_PROP_PERCEPTION = std::string("Wahrnehmung");
const std::string STR_PROP_KNOWLEDGE = std::string("Wissen ---*");
const std::string STR_PROP_MAGICART = std::string("Zauberkunde*");


/************************************************************************/
/* MODULES			                                                    */
/************************************************************************/
const std::string STR_ATTACKABLE = std::string("Angreifbar");
const std::string STR_JUMPPOINT = std::string("Spungmarke");
const std::string STR_ATTRIBUTES = std::string("Attribute");
const std::string STR_APPEARANCE = std::string("Erscheinungsbild");


/************************************************************************/
/* TEMPLATES		                                                    */
/************************************************************************/
const std::string STR_GOBLIN_NAME = std::string("Goblin");
const std::string STR_HOBGOBLIN_NAME = std::string("Hobgoblin");
const std::string STR_GNOLL_NAME = std::string("Gnoll");
const std::string STR_KOBOLD_NAME = std::string("Kobold");
const std::string STR_LIZARDFOLK_NAME = std::string("Echsenvolk");
const std::string STR_SKELETON_NAME = std::string("Skelett");
const std::string STR_CHEST_NAME = std::string("Truhe");
const std::string STR_WALLH = std::string("Wand_H");
const std::string STR_WALLV = std::string("Wand_V");
const std::string STR_WALLC = std::string("Wand_K");
const std::string STR_STAIRS = std::string("Treppe");
const std::string STR_PLANKS = std::string("Holz");
const std::string STR_EARTH = std::string("Erde");
const std::string STR_GRASS = std::string("Gras");
const std::string STR_WATER = std::string("Wasser");
const std::string STR_WELL_NAME = std::string("Brunnen");
const std::string STR_BASALT = std::string("Basalt");
const std::string STR_GRANITE = std::string("Granit");


/************************************************************************/
/* GUI				                                                    */
/************************************************************************/
const std::string STR_MAP = std::string("Map");
const std::string STR_MODE = std::string("Modus");
const std::string STR_BRUSH = std::string("Pinsel");
const std::string STR_PLAYER = std::string("Spieler");
const std::string STR_NPC = std::string("NSC");
const std::string STR_GOTO = std::string("Gehe zu");
const std::string STR_COMBAT = std::string("Kampf");
const std::string STR_ACTION = std::string("Aktion");
const std::string STR_DRAGNDROP = std::string("Drag & Drop");
const std::string STR_DIAMETER = std::string("Größe");
const std::string STR_ADD = std::string("Hinzufügen");
const std::string STR_REPLACE = std::string("Ersetzen");
const std::string STR_REMOVE = std::string("Löschen");
const std::string STR_AUTOLAYER = std::string("Automatisch");
const std::string STR_LAYER1 = std::string("1: Boden");
const std::string STR_LAYER2 = std::string("2: Statisches I");
const std::string STR_LAYER3 = std::string("3: Statisches II");
const std::string STR_LAYER4 = std::string("4: Gegenstände");
const std::string STR_LAYER5 = std::string("5: Magisches");
const std::string STR_LAYER6 = std::string("6: Freunde");
const std::string STR_LAYER7 = std::string("7: Neutral");
const std::string STR_LAYER8 = std::string("8: Feinde");
const std::string STR_LAYER9 = std::string("9: Eigenes I");
const std::string STR_LAYER10 = std::string("0: Eigenes II");
const std::string STR_SELECTION = std::string("Selektion");
const std::string STR_CREATE = std::string("Erstellen");
const std::string STR_CANCEL = std::string("Abbrechen");
const std::string STR_OK = std::string("Ok");