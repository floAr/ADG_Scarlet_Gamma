#include "states/NewPlayerState.hpp"
#include "StateMachine.hpp"
#include "Game.hpp"
#include "SFML/Window.hpp"
#include <iostream>
#include "utils/Content.hpp"
#include "SFML/Network/IpAddress.hpp"
#include "Constants.hpp"
#include "core/PredefinedProperties.hpp"
#include "core/World.hpp"
#include "utils/ValueInterpreter.hpp"

namespace States {

NewPlayerState::NewPlayerState(tgui::EditBox::Ptr _nameEdit, Core::ObjectID* _saveID) :
    GameState(),
	m_nameOutputEdit(_nameEdit),
	m_rand((uint32_t)_nameEdit.get()),
	m_newPlayerID(_saveID),
	m_newPlayer(nullptr),
	m_name(nullptr),
	m_cAttitude(nullptr),
	m_eFaith(nullptr),
	m_eClass(nullptr),
	m_eFolk(nullptr),
	m_eHome(nullptr),
	m_eSize(nullptr),
	m_eAge(nullptr),
	m_cSex(nullptr),
	m_eWeight(nullptr),
	m_eHair(nullptr),
	m_eEye(nullptr),
	m_eSt(nullptr),
	m_eStMod(nullptr),
	m_eGe(nullptr),
	m_eGeMod(nullptr),
	m_eKo(nullptr),
	m_eKoMod(nullptr),
	m_eIn(nullptr),
	m_eInMod(nullptr),
	m_eWe(nullptr),
	m_eWeMod(nullptr),
	m_eCh(nullptr),
	m_eChMod(nullptr),
	m_eTP(nullptr),
	m_eTPMax(nullptr),
	m_eRK(nullptr)
{
    //--------------------------------------
    // CREATE GUI
	m_menuFont = Content::Instance()->LoadFont("media/arial.ttf");
    m_gui.setWindow(g_Game->GetWindow());
    m_gui.setGlobalFont(m_menuFont);
    SetGui(&m_gui);

	// From now on creating elements relies much on cloning because this is
	// much faster.

	// Enter name
	tgui::Label::Ptr label( m_gui );
	label->setPosition( 20.0f, 35.0f );
	label->setText( STR_PROP_NAME );
	label->setTextSize( 19 );
	label->setSize( 90.0f, 30.0f );
	label->setTextColor( sf::Color( 200, 200, 200 ) );
	m_name = tgui::EditBox::Ptr( m_gui );
	m_name->load("media/Black.conf");
	m_name->setText("");
	m_name->setPosition( 120.0f, 30.0f );
	m_name->setSize( 200.0f, 30.0f );
	// Attitude
	label = label.clone();		m_gui.add(label);
	label->setPosition( 360.0f, 35.0f );
	label->setText( STR_PROP_ATTITUDE );
	m_cAttitude = tgui::ComboBox::Ptr( m_gui );
	m_cAttitude->load( "media/Black.conf" );
	m_cAttitude->setSize( 200.0f, 30.0f );
	m_cAttitude->setPosition( 460.0f, 30.0f );
	m_cAttitude->addItem( STR_ATT_LAWFULGOOD );
	m_cAttitude->addItem( STR_ATT_NEUTRALGOOD );
	m_cAttitude->addItem( STR_ATT_CHAOTICGOOD );
	m_cAttitude->addItem( STR_ATT_LAWFULNEUTRAL );
	m_cAttitude->addItem( STR_ATT_NEUTRALNEUTRAL );
	m_cAttitude->addItem( STR_ATT_CHAOTICNEUTRAL );
	m_cAttitude->addItem( STR_ATT_LAWFULEVIL );
	m_cAttitude->addItem( STR_ATT_NEUTRALEVIL );
	m_cAttitude->addItem( STR_ATT_CHAOTICEVIL );
	m_cAttitude->setSelectedItem( 0 );
	// Faith
	label = label.clone();							m_gui.add(label);
	label->setPosition( 700.0f, 35.0f );
	label->setText( STR_PROP_FAITH );
	m_eFaith = m_name.clone();						m_gui.add(m_eFaith);
	m_eFaith->setPosition( 800.0f, 30.0f );

	// Enter class
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 75.0f );
	label->setText( STR_PROP_CLASS );
	m_eClass = m_name.clone();						m_gui.add(m_eClass);
	m_eClass->setPosition( 120.0f, 70.0f );
	// Folk
	label = label.clone();							m_gui.add(label);
	label->setPosition( 360.0f, 75.0f );
	label->setText( STR_PROP_FOLK );
	m_eFolk = m_name.clone();						m_gui.add(m_eFolk);
	m_eFolk->setPosition( 460.0f, 70.0f );
	// Home
	label = label.clone();							m_gui.add(label);
	label->setPosition( 700.0f, 75.0f );
	label->setText( STR_PROP_HOME );
	m_eHome = m_name.clone();						m_gui.add(m_eHome);
	m_eHome->setPosition( 800.0f, 70.0f );

	// Size
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 118.0f );
	label->setText( STR_PROP_SIZE );
	label->setTextSize( 16 );
	m_eSize = m_name.clone();						m_gui.add(m_eSize);
	m_eSize->setPosition( 75.0f, 110.0f );
	m_eSize->setSize( 80.0f, 30.0f );
	// Age
	label = label.clone();							m_gui.add(label);
	label->setPosition( 175.0f, 118.0f );
	label->setText( STR_PROP_AGE );
	m_eAge = m_eSize.clone();						m_gui.add(m_eAge);
	m_eAge->setPosition( 220.0f, 110.0f );
	// Sex
	label = label.clone();							m_gui.add(label);
	label->setPosition( 320.0f, 118.0f );
	label->setText( STR_PROP_SEX );
	m_cSex = m_cAttitude.clone();					m_gui.add(m_cSex);
	m_cSex->setPosition( 403.0f, 110.0f );
	m_cSex->setSize( 70.0f, 30.0f );
	m_cSex->removeAllItems();
	m_cSex->addItem(STR_SEX_M);
	m_cSex->addItem(STR_SEX_F);
	m_cSex->setSelectedItem( 0 );
	// Weight
	label = label.clone();							m_gui.add(label);
	label->setPosition( 498.0f, 118.0f );
	label->setText( STR_PROP_WEIGHT );
	m_eWeight = m_eSize.clone();					m_gui.add(m_eWeight);
	m_eWeight->setPosition( 560.0f, 110.0f );
	// Hair color
	label = label.clone();							m_gui.add(label);
	label->setPosition( 660.0f, 118.0f );
	label->setText( STR_PROP_HAIRCOLOR );
	m_eHair = m_eSize.clone();						m_gui.add(m_eHair);
	m_eHair->setPosition( 737.0f, 110.0f );
	// Eye color
	label = label.clone();							m_gui.add(label);
	label->setPosition( 834.0f, 118.0f );
	label->setText( STR_PROP_EYECOLOR );
	m_eEye = m_eSize.clone();						m_gui.add(m_eEye);
	m_eEye->setPosition( 920.0f, 110.0f );

	// Attributes
	label = label.clone();							m_gui.add(label);
	label->setTextSize( 19 );
	label->setPosition( 20.0f, 165.0f );
	label->setSize( 145.0f, 30.0f );
	label->setText( "ST (Stärke)" );
	m_eSt = m_eSize.clone();						m_gui.add(m_eSt);
	m_eSt->setNumbersOnly( true );
	m_eSt->setText( STR_0 );
	m_eSt->setPosition( 175.0f, 160.0f );
	m_eSt->bindCallback( &NewPlayerState::AttributeStrengthChanged, this , tgui::EditBox::TextChanged );
	m_eStMod = m_eSt.clone();						m_gui.add(m_eStMod);
	m_eStMod->disable();
	m_eStMod->setPosition( 265.0f, 160.0f );
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 205.0f );
	label->setText( "GE (Geschick)" );
	m_eGe = m_eSt.clone();							m_gui.add(m_eGe);
	m_eGe->setPosition( 175.0f, 200.0f );
	m_eGe->bindCallback( &NewPlayerState::AttributeDexterityChanged, this , tgui::EditBox::TextChanged );
	m_eGeMod = m_eStMod.clone();					m_gui.add(m_eGeMod);
	m_eGeMod->setPosition( 265.0f, 200.0f );
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 245.0f );
	label->setText( "KO (Konstitution)" );
	m_eKo = m_eSt.clone();							m_gui.add(m_eKo);
	m_eKo->setPosition( 175.0f, 240.0f );
	m_eKo->bindCallback( &NewPlayerState::AttributeConstitutionChanged, this , tgui::EditBox::TextChanged );
	m_eKoMod = m_eStMod.clone();					m_gui.add(m_eKoMod);
	m_eKoMod->setPosition( 265.0f, 240.0f );
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 285.0f );
	label->setText( "IN (Intelligenz)" );
	m_eIn = m_eSt.clone();							m_gui.add(m_eIn);
	m_eIn->setPosition( 175.0f, 280.0f );
	m_eIn->bindCallback( &NewPlayerState::AttributeIntelligenceChanged, this , tgui::EditBox::TextChanged );
	m_eInMod = m_eStMod.clone();					m_gui.add(m_eInMod);
	m_eInMod->setPosition( 265.0f, 280.0f );
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 325.0f );
	label->setText( "WE (Weisheit)" );
	m_eWe = m_eSt.clone();							m_gui.add(m_eWe);
	m_eWe->setPosition( 175.0f, 320.0f );
	m_eWe->bindCallback( &NewPlayerState::AttributeWisdomChanged, this , tgui::EditBox::TextChanged );
	m_eWeMod = m_eStMod.clone();					m_gui.add(m_eWeMod);
	m_eWeMod->setPosition( 265.0f, 320.0f );
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 365.0f );
	label->setText( "CH (Charisma)" );
	m_eCh = m_eSt.clone();							m_gui.add(m_eCh);
	m_eCh->setPosition( 175.0f, 360.0f );
	m_eCh->bindCallback( &NewPlayerState::AttributeCharismaChanged, this , tgui::EditBox::TextChanged );
	m_eChMod = m_eStMod.clone();					m_gui.add(m_eChMod);
	m_eChMod->setPosition( 265.0f, 360.0f );

	// Health
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 415.0f );
	label->setText( "TP akt./max." );
	m_eTP = m_eSt.clone();							m_gui.add(m_eTP);
	m_eTP->setPosition( 175.0f, 410.0f );
	m_eTPMax = m_eSt.clone();						m_gui.add(m_eTPMax);
	m_eTPMax->setPosition( 265.0f, 410.0f );

	// Armor class
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 465.0f );
	label->setText( "RK (Rüstungsklasse)" );
	m_eRK = m_eSt.clone();							m_gui.add(m_eRK);
	m_eRK->setPosition( 175.0f, 460.0f );
	m_eRK->setSize( 170.0f, 30.0f );
	m_eRK->setNumbersOnly( false );

	// Create/Go back to where we came from
	tgui::Button::Ptr button( m_gui );
	button->load("media/Black.conf");
	button->setText( STR_CREATE );
	button->setPosition( 740.0f, 700.0f );
	button->setSize( 200.0f, 40.0f );
	button->setTextSize( 26 );
	button->setCallbackId( 2 );
	button->bindCallback( &NewPlayerState::Create, this, tgui::Button::LeftMouseClicked );

	// Load the player or create a new one
	if( *_saveID == 0xffffffff ) CreateNew();
	else {
		m_newPlayer = g_Game->GetWorld()->GetObject(*m_newPlayerID);
		ShowPlayer();
		button->setText( "OK" );
	}
}




void NewPlayerState::Draw(sf::RenderWindow& win)
{
    // Set window color according to mouse position...
    win.clear(sf::Color::Black);

	GameState::Draw(win);
}

void NewPlayerState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
    switch (key.code)
    {
    // Quit with escape
    case sf::Keyboard::Escape:
        m_finished = true;
        break;
    }
}


void NewPlayerState::CreateNew()
{
	// Create a new player object
	*m_newPlayerID = g_Game->GetWorld()->NewObject("media/smile_2.png");
	m_newPlayer = g_Game->GetWorld()->GetObject(*m_newPlayerID);
	m_newPlayer->Add( Core::PROPERTY::NAME );
	m_newPlayer->Add( Core::PROPERTY::OWNER );
	m_newPlayer->Add( Core::PROPERTY::PLAYER );
	m_newPlayer->Add( Core::PROPERTY::TARGET );
	m_newPlayer->Add( Core::PROPERTY::PATH );

	m_newPlayer->Add( Core::PROPERTY::STRENGTH );
	m_newPlayer->Add( Core::PROPERTY::STRENGTH_MOD );
	m_newPlayer->Add( Core::PROPERTY::DEXTERITY );
	m_newPlayer->Add( Core::PROPERTY::DEXTERITY_MOD );
	m_newPlayer->Add( Core::PROPERTY::CONSTITUTION );
	m_newPlayer->Add( Core::PROPERTY::CONSTITUTION_MOD );
	m_newPlayer->Add( Core::PROPERTY::INTELLIGENCE );
	m_newPlayer->Add( Core::PROPERTY::INTELLIGENCE_MOD );
	m_newPlayer->Add( Core::PROPERTY::WISDOM );
	m_newPlayer->Add( Core::PROPERTY::WISDOM_MOD );
	m_newPlayer->Add( Core::PROPERTY::CHARISMA );
	m_newPlayer->Add( Core::PROPERTY::CHARISMA_MOD );

	m_newPlayer->Add( Core::PROPERTY::HEALTH );
	m_newPlayer->Add( Core::PROPERTY::HEALTH_MAX );
	m_newPlayer->Add( Core::PROPERTY::ARMORCLASS );

	m_newPlayer->Add( Core::PROPERTY::ATTITUDE );
	m_newPlayer->Add( Core::PROPERTY::CLASS );
	m_newPlayer->Add( Core::PROPERTY::FOLK );
	m_newPlayer->Add( Core::PROPERTY::HOME );
	m_newPlayer->Add( Core::PROPERTY::FAITH );
	m_newPlayer->Add( Core::PROPERTY::SIZE );
	m_newPlayer->Add( Core::PROPERTY::AGE );
	m_newPlayer->Add( Core::PROPERTY::SEX );
	m_newPlayer->Add( Core::PROPERTY::WEIGHT );
	m_newPlayer->Add( Core::PROPERTY::HAIRCOLOR );
	m_newPlayer->Add( Core::PROPERTY::EYECOLOR );

	// Create a player sub-object containing his talents
	m_newPlayer->Add( Core::PROPERTY::TALENTS );

	ShowPlayer();
}

void NewPlayerState::Create()
{
	m_nameOutputEdit->setText( m_name->getText() );
	m_newPlayer->GetProperty( STR_PROP_NAME ).SetValue( m_name->getText() );
	m_newPlayer->GetProperty( STR_PROP_OWNER ).SetValue( m_name->getText() );

	// Set all the attributes which are not updated automatically
	if( m_newPlayer->HasProperty(STR_PROP_HEALTH) )
		m_newPlayer->GetProperty( STR_PROP_HEALTH ).SetValue(m_eTP->getText());
	if( m_newPlayer->HasProperty(STR_PROP_HEALTH_MAX) )
		m_newPlayer->GetProperty( STR_PROP_HEALTH_MAX ).SetValue(m_eTPMax->getText());
	if( m_newPlayer->HasProperty(STR_PROP_ARMORCLASS) )
		m_newPlayer->GetProperty( STR_PROP_ARMORCLASS ).SetValue(m_eRK->getText());

	if( m_newPlayer->HasProperty(STR_PROP_ATTITUDE) )
		// Find the right combobox entry
		m_newPlayer->GetProperty( STR_PROP_ATTITUDE ).SetValue(m_cAttitude->getSelectedItem());
	if( m_newPlayer->HasProperty(STR_PROP_CLASS) )
		m_newPlayer->GetProperty( STR_PROP_CLASS ).SetValue(m_eClass->getText());
	if( m_newPlayer->HasProperty(STR_PROP_FOLK) )
		m_newPlayer->GetProperty( STR_PROP_FOLK ).SetValue(m_eFolk->getText());
	if( m_newPlayer->HasProperty(STR_PROP_HOME) )
		m_newPlayer->GetProperty( STR_PROP_HOME ).SetValue(m_eHome->getText());
	if( m_newPlayer->HasProperty(STR_PROP_FAITH) )
		m_newPlayer->GetProperty( STR_PROP_FAITH ).SetValue(m_eFaith->getText());
	if( m_newPlayer->HasProperty(STR_PROP_SIZE) )
		m_newPlayer->GetProperty( STR_PROP_SIZE ).SetValue(m_eSize->getText());
	if( m_newPlayer->HasProperty(STR_PROP_AGE) )
		m_newPlayer->GetProperty( STR_PROP_AGE ).SetValue(m_eAge->getText());
	if( m_newPlayer->HasProperty(STR_PROP_SEX) )
		m_newPlayer->GetProperty( STR_PROP_SEX ).SetValue(m_cSex->getSelectedItem());
	if( m_newPlayer->HasProperty(STR_PROP_WEIGHT) )
		m_newPlayer->GetProperty( STR_PROP_WEIGHT ).SetValue(m_eWeight->getText());
	if( m_newPlayer->HasProperty(STR_PROP_HAIRCOLOR) )
		m_newPlayer->GetProperty( STR_PROP_HAIRCOLOR ).SetValue(m_eHair->getText());
	if( m_newPlayer->HasProperty(STR_PROP_EYECOLOR) )
		m_newPlayer->GetProperty( STR_PROP_EYECOLOR ).SetValue(m_eEye->getText());

	m_finished = true;
}


void NewPlayerState::AttributeStrengthChanged()
{
	// Send new data to player and recalculate dependent attributes
	try {
		m_newPlayer->GetProperty( STR_PROP_STRENGTH ).SetValue( m_eSt->getText() );
		int modificator = Utils::EvaluateFormula( m_newPlayer->GetProperty(STR_PROP_STRENGTH_MOD).Value(), &m_rand, m_newPlayer );
		m_eStMod->setText( std::to_string(modificator) );
	} catch(...) {}
}

void NewPlayerState::AttributeDexterityChanged()
{
	// Send new data to player and recalculate dependent attributes
	try {
		m_newPlayer->GetProperty( STR_PROP_DEXTERITY ).SetValue( m_eGe->getText() );
		int modificator = Utils::EvaluateFormula( m_newPlayer->GetProperty(STR_PROP_DEXTERITY_MOD).Value(), &m_rand, m_newPlayer );
		m_eGeMod->setText( std::to_string(modificator) );
	} catch(...) {}
}

void NewPlayerState::AttributeConstitutionChanged()
{
	// Send new data to player and recalculate dependent attributes
	try {
		m_newPlayer->GetProperty( STR_PROP_CONSTITUTION ).SetValue( m_eKo->getText() );
		int modificator = Utils::EvaluateFormula( m_newPlayer->GetProperty(STR_PROP_CONSTITUTION_MOD).Value(), &m_rand, m_newPlayer );
		m_eKoMod->setText( std::to_string(modificator) );
	} catch(...) {}
}

void NewPlayerState::AttributeIntelligenceChanged()
{
	// Send new data to player and recalculate dependent attributes
	try {
		m_newPlayer->GetProperty( STR_PROP_INTELLIGENCE ).SetValue( m_eIn->getText() );
		int modificator = Utils::EvaluateFormula( m_newPlayer->GetProperty(STR_PROP_INTELLIGENCE_MOD).Value(), &m_rand, m_newPlayer );
		m_eInMod->setText( std::to_string(modificator) );
	} catch(...) {}
}

void NewPlayerState::AttributeWisdomChanged()
{
	// Send new data to player and recalculate dependent attributes
	try {
		m_newPlayer->GetProperty( STR_PROP_WISDOM ).SetValue( m_eWe->getText() );
		int modificator = Utils::EvaluateFormula( m_newPlayer->GetProperty(STR_PROP_WISDOM_MOD).Value(), &m_rand, m_newPlayer );
		m_eWeMod->setText( std::to_string(modificator) );
	} catch(...) {}
}

void NewPlayerState::AttributeCharismaChanged()
{
	// Send new data to player and recalculate dependent attributes
	try {
		m_newPlayer->GetProperty( STR_PROP_CHARISMA ).SetValue( m_eCh->getText() );
		int modificator = Utils::EvaluateFormula( m_newPlayer->GetProperty(STR_PROP_CHARISMA_MOD).Value(), &m_rand, m_newPlayer );
		m_eChMod->setText( std::to_string(modificator) );
	} catch(...) {}
}

void NewPlayerState::ShowPlayer()
{
	if( m_newPlayer->HasProperty(STR_PROP_NAME) )
		m_name->setText( m_newPlayer->GetProperty( STR_PROP_NAME ).Value() );
	if( m_newPlayer->HasProperty(STR_PROP_STRENGTH) )	{
		m_eSt->setText( m_newPlayer->GetProperty( STR_PROP_STRENGTH ).Value() );
		AttributeStrengthChanged();
	}
	if( m_newPlayer->HasProperty(STR_PROP_DEXTERITY) )	{
		m_eGe->setText( m_newPlayer->GetProperty( STR_PROP_DEXTERITY ).Value() );
		AttributeDexterityChanged();
	}
	if( m_newPlayer->HasProperty(STR_PROP_CONSTITUTION) )	{
		m_eKo->setText( m_newPlayer->GetProperty( STR_PROP_CONSTITUTION ).Value() );
		AttributeConstitutionChanged();
	}
	if( m_newPlayer->HasProperty(STR_PROP_INTELLIGENCE) )	{
		m_eIn->setText( m_newPlayer->GetProperty( STR_PROP_INTELLIGENCE ).Value() );
		AttributeIntelligenceChanged();
	}
	if( m_newPlayer->HasProperty(STR_PROP_WISDOM) )	{
		m_eWe->setText( m_newPlayer->GetProperty( STR_PROP_WISDOM ).Value() );
		AttributeWisdomChanged();
	}
	if( m_newPlayer->HasProperty(STR_PROP_CHARISMA) )	{
		m_eCh->setText( m_newPlayer->GetProperty( STR_PROP_CHARISMA ).Value() );
		AttributeCharismaChanged();
	}

	if( m_newPlayer->HasProperty(STR_PROP_HEALTH) )
		m_eTP->setText( m_newPlayer->GetProperty( STR_PROP_HEALTH ).Value() );
	if( m_newPlayer->HasProperty(STR_PROP_HEALTH_MAX) )
		m_eTPMax->setText( m_newPlayer->GetProperty( STR_PROP_HEALTH_MAX ).Value() );
	if( m_newPlayer->HasProperty(STR_PROP_ARMORCLASS) )
		m_eRK->setText( m_newPlayer->GetProperty( STR_PROP_ARMORCLASS ).Value() );

	if( m_newPlayer->HasProperty(STR_PROP_ATTITUDE) )
		// Find the right combobox entry
		m_cAttitude->setSelectedItem( m_newPlayer->GetProperty( STR_PROP_ATTITUDE ).Value() );
	if( m_newPlayer->HasProperty(STR_PROP_CLASS) )
		m_eClass->setText( m_newPlayer->GetProperty( STR_PROP_CLASS ).Value() );
	if( m_newPlayer->HasProperty(STR_PROP_FOLK) )
		m_eFolk->setText( m_newPlayer->GetProperty( STR_PROP_FOLK ).Value() );
	if( m_newPlayer->HasProperty(STR_PROP_HOME) )
		m_eHome->setText( m_newPlayer->GetProperty( STR_PROP_HOME ).Value() );
	if( m_newPlayer->HasProperty(STR_PROP_FAITH) )
		m_eFaith->setText( m_newPlayer->GetProperty( STR_PROP_FAITH ).Value() );
	if( m_newPlayer->HasProperty(STR_PROP_SIZE) )
		m_eSize->setText( m_newPlayer->GetProperty( STR_PROP_SIZE ).Value() );
	if( m_newPlayer->HasProperty(STR_PROP_AGE) )
		m_eAge->setText( m_newPlayer->GetProperty( STR_PROP_AGE ).Value() );
	if( m_newPlayer->HasProperty(STR_PROP_SEX) )
		m_cSex->setSelectedItem( m_newPlayer->GetProperty( STR_PROP_SEX ).Value() );
	if( m_newPlayer->HasProperty(STR_PROP_WEIGHT) )
		m_eWeight->setText( m_newPlayer->GetProperty( STR_PROP_WEIGHT ).Value() );
	if( m_newPlayer->HasProperty(STR_PROP_HAIRCOLOR) )
		m_eHair->setText( m_newPlayer->GetProperty( STR_PROP_HAIRCOLOR ).Value() );
	if( m_newPlayer->HasProperty(STR_PROP_EYECOLOR) )
		m_eEye->setText( m_newPlayer->GetProperty( STR_PROP_EYECOLOR ).Value() );
}

} // namespace States