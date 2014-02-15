#include "states/NewPlayerState.hpp"
#include "StateMachine.hpp"
#include "Game.hpp"
#include "utils/Content.hpp"
#include "Constants.hpp"
#include "core/PredefinedProperties.hpp"
#include "core/World.hpp"
#include "utils/ValueInterpreter.hpp"

namespace States {

CharacterState::CharacterState(Core::ObjectID* _saveID) :
    GameState(),
	m_rand((uint32_t)this),
	m_draggedContent(nullptr),
	m_playerID(_saveID),
	m_player(nullptr),
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
	m_eRK(nullptr),
	m_playerTalents(nullptr),
	m_talentTemplates(nullptr)
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
	label->setSize( 95.0f, 30.0f );
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

	// Experience
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 115.0f );
	label->setText( STR_PROP_EXPERIENCE );
	m_eExperience = m_name.clone();					m_gui.add(m_eExperience);
	m_eExperience->setPosition( 120.0f, 110.0f );
	m_eExperience->setNumbersOnly( true );
	// Level
	label = label.clone();							m_gui.add(label);
	label->setPosition( 360.0f, 115.0f );
	label->setText( STR_PROP_LEVEL );
	m_eLevel = m_name.clone();						m_gui.add(m_eLevel);
	m_eLevel->setPosition( 460.0f, 110.0f );
	m_eLevel->setNumbersOnly(true);
	// Speed
	label = label.clone();							m_gui.add(label);
	label->setPosition( 700.0f, 115.0f );
	label->setText( STR_PROP_SPEED );
	m_eSpeed = m_name.clone();						m_gui.add(m_eSpeed);
	m_eSpeed->setPosition( 800.0f, 110.0f );

	// Size
	label = label.clone();							m_gui.add(label);
	label->setPosition( 430.0f, 576.0f );
	label->setText( STR_PROP_SIZE );
	label->setTextSize( 18 );
	m_eSize = m_name.clone();						m_gui.add(m_eSize);
	m_eSize->setPosition( 530.0f, 570.0f );
	m_eSize->setSize( 80.0f, 30.0f );
	// Age
	label = label.clone();							m_gui.add(label);
	label->setPosition( 625.0f, 576.0f );
	label->setText( STR_PROP_AGE );
	m_eAge = m_eSize.clone();						m_gui.add(m_eAge);
	m_eAge->setPosition( 725.0f, 570.0f );
	// Sex
	label = label.clone();							m_gui.add(label);
	label->setPosition( 820.0f, 576.0f );
	label->setText( STR_PROP_SEX );
	m_cSex = m_cAttitude.clone();					m_gui.add(m_cSex);
	m_cSex->setPosition( 920.0f, 570.0f );
	m_cSex->setSize( 80.0f, 30.0f );
	m_cSex->removeAllItems();
	m_cSex->addItem(STR_SEX_M);
	m_cSex->addItem(STR_SEX_F);
	m_cSex->setSelectedItem( 0 );
	// Weight
	label = label.clone();							m_gui.add(label);
	label->setPosition( 430.0f, 616.0f );
	label->setText( STR_PROP_WEIGHT );
	m_eWeight = m_eSize.clone();					m_gui.add(m_eWeight);
	m_eWeight->setPosition( 530.0f, 610.0f );
	// Hair color
	label = label.clone();							m_gui.add(label);
	label->setPosition( 625.0f, 616.0f );
	label->setText( STR_PROP_HAIRCOLOR );
	m_eHair = m_eSize.clone();						m_gui.add(m_eHair);
	m_eHair->setPosition( 725.0f, 610.0f );
	// Eye color
	label = label.clone();							m_gui.add(label);
	label->setPosition( 820.0f, 616.0f );
	label->setText( STR_PROP_EYECOLOR );
	m_eEye = m_eSize.clone();						m_gui.add(m_eEye);
	m_eEye->setPosition( 920.0f, 610.0f );

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
	m_eSt->bindCallback( &CharacterState::AttributeStrengthChanged, this , tgui::EditBox::TextChanged );
	m_eStMod = m_eSt.clone();						m_gui.add(m_eStMod);
	m_eStMod->disable();
	m_eStMod->setPosition( 265.0f, 160.0f );
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 205.0f );
	label->setText( "GE (Geschick)" );
	m_eGe = m_eSt.clone();							m_gui.add(m_eGe);
	m_eGe->setPosition( 175.0f, 200.0f );
	m_eGe->bindCallback( &CharacterState::AttributeDexterityChanged, this , tgui::EditBox::TextChanged );
	m_eGeMod = m_eStMod.clone();					m_gui.add(m_eGeMod);
	m_eGeMod->setPosition( 265.0f, 200.0f );
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 245.0f );
	label->setText( "KO (Konstitution)" );
	m_eKo = m_eSt.clone();							m_gui.add(m_eKo);
	m_eKo->setPosition( 175.0f, 240.0f );
	m_eKo->bindCallback( &CharacterState::AttributeConstitutionChanged, this , tgui::EditBox::TextChanged );
	m_eKoMod = m_eStMod.clone();					m_gui.add(m_eKoMod);
	m_eKoMod->setPosition( 265.0f, 240.0f );
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 285.0f );
	label->setText( "IN (Intelligenz)" );
	m_eIn = m_eSt.clone();							m_gui.add(m_eIn);
	m_eIn->setPosition( 175.0f, 280.0f );
	m_eIn->bindCallback( &CharacterState::AttributeIntelligenceChanged, this , tgui::EditBox::TextChanged );
	m_eInMod = m_eStMod.clone();					m_gui.add(m_eInMod);
	m_eInMod->setPosition( 265.0f, 280.0f );
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 325.0f );
	label->setText( "WE (Weisheit)" );
	m_eWe = m_eSt.clone();							m_gui.add(m_eWe);
	m_eWe->setPosition( 175.0f, 320.0f );
	m_eWe->bindCallback( &CharacterState::AttributeWisdomChanged, this , tgui::EditBox::TextChanged );
	m_eWeMod = m_eStMod.clone();					m_gui.add(m_eWeMod);
	m_eWeMod->setPosition( 265.0f, 320.0f );
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 365.0f );
	label->setText( "CH (Charisma)" );
	m_eCh = m_eSt.clone();							m_gui.add(m_eCh);
	m_eCh->setPosition( 175.0f, 360.0f );
	m_eCh->bindCallback( &CharacterState::AttributeCharismaChanged, this , tgui::EditBox::TextChanged );
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

	// Basic attack
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 505.0f );
	label->setText( STR_PROP_BASIC_ATTACK );
	m_eBasicAttack = m_eRK.clone();					m_gui.add(m_eBasicAttack);
	m_eBasicAttack->setPosition( 175.0f, 500.0f );

	// Reflex, Will, Resilence
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 575.0f );
	label->setText( STR_PROP_REFLEX );
	m_eReflex = m_eRK.clone();						m_gui.add(m_eReflex);
	m_eReflex->setPosition( 175.0f, 570.0f );
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 615.0f );
	label->setText( STR_PROP_WILL );
	m_eWill = m_eRK.clone();						m_gui.add(m_eWill);
	m_eWill->setPosition( 175.0f, 610.0f );
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 655.0f );
	label->setText( STR_PROP_RESILENCE );
	m_eResilence = m_eRK.clone();					m_gui.add(m_eResilence);
	m_eResilence->setPosition( 175.0f, 650.0f );

	// Create/Go back to where we came from
	tgui::Button::Ptr button( m_gui );
	button->load("media/Black.conf");
	button->setText( STR_CREATE );
	button->setPosition( 740.0f, 700.0f );
	button->setSize( 200.0f, 40.0f );
	button->setTextSize( 26 );
	button->setCallbackId( 2 );
	button->bindCallback( &CharacterState::Create, this, tgui::Button::LeftMouseClicked );

	m_playerTalents = Interfaces::PropertyPanel::Ptr( m_gui );
	label = label.clone();							m_gui.add(label);
	label->setPosition( 430.0f, 165.0f );
	label->setSize( 300.0f, 30.0f );
	label->setText( "Deine Fertigkeiten" );
	m_playerTalents->Init( 430.0f, 195.0f, 270.0f, 360.0f, true, false, 0, &m_draggedContent );

	m_talentTemplates = Interfaces::PropertyPanel::Ptr( m_gui );
	label = label.clone();							m_gui.add(label);
	label->setPosition( 730.0f, 165.0f );
	label->setText( "Fertigkeiten Vorgaben" );
	m_talentTemplates->Init( 730.0f, 195.0f, 270.0f, 360.0f, true, false, 0, &m_draggedContent );
	// Search for the talents sub-object from template base
	Core::Object* propBase = g_Game->GetWorld()->GetPropertyBaseObject();
	Core::ObjectID talentBase = propBase->GetProperty( STR_PROP_TALENTS ).GetObjects()[0];
	m_talentTemplates->Show( g_Game->GetWorld(), g_Game->GetWorld()->GetObject(talentBase) );

	// Load the player or create a new one
	m_creatingNew = *_saveID == Core::INVALID_ID;
	if( m_creatingNew ) CreateNew();
	else {
		m_player = g_Game->GetWorld()->GetObject(*m_playerID);
		ShowPlayer();
		button->setText( "OK" );
	}
}




void CharacterState::Draw(sf::RenderWindow& win)
{
    // Set window color according to mouse position...
    win.clear(sf::Color::Black);

	GameState::Draw(win);
}

void CharacterState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
    switch (key.code)
    {
    // Quit with escape and c
	case sf::Keyboard::C:
		if( m_creatingNew || guiHandled ) break;	// Disable c in creating mode
    case sf::Keyboard::Escape:
		if( m_creatingNew )
		{
			// Break creation -> delete the objects (incusive subobjects)
			g_Game->GetWorld()->RemoveObject(m_player->GetProperty(STR_PROP_TALENTS).GetObjects()[0]);
			g_Game->GetWorld()->RemoveObject(m_player->ID());
			*m_playerID = Core::INVALID_ID;
		}

        m_finished = true;
        break;
    }
}


void CharacterState::CreateNew()
{
	// Create a new player object
	*m_playerID = g_Game->GetWorld()->NewObject("media/smile_1.png");
	m_player = g_Game->GetWorld()->GetObject(*m_playerID);
	m_player->Add( Core::PROPERTY::NAME );
	m_player->Add( Core::PROPERTY::OWNER );
	m_player->Add( Core::PROPERTY::PLAYER );
	m_player->Add( Core::PROPERTY::TARGET );
	m_player->Add( Core::PROPERTY::PATH );

	m_player->Add( Core::PROPERTY::STRENGTH );
	m_player->Add( Core::PROPERTY::STRENGTH_MOD );
	m_player->Add( Core::PROPERTY::DEXTERITY );
	m_player->Add( Core::PROPERTY::DEXTERITY_MOD );
	m_player->Add( Core::PROPERTY::CONSTITUTION );
	m_player->Add( Core::PROPERTY::CONSTITUTION_MOD );
	m_player->Add( Core::PROPERTY::INTELLIGENCE );
	m_player->Add( Core::PROPERTY::INTELLIGENCE_MOD );
	m_player->Add( Core::PROPERTY::WISDOM );
	m_player->Add( Core::PROPERTY::WISDOM_MOD );
	m_player->Add( Core::PROPERTY::CHARISMA );
	m_player->Add( Core::PROPERTY::CHARISMA_MOD );

	m_player->Add( Core::PROPERTY::HEALTH );
	m_player->Add( Core::PROPERTY::HEALTH_MAX );
	m_player->Add( Core::PROPERTY::ARMORCLASS );
	m_player->Add( Core::PROPERTY::BASIC_ATTACK );
	m_player->Add( Core::PROPERTY::REFLEX );
	m_player->Add( Core::PROPERTY::WILL );
	m_player->Add( Core::PROPERTY::RESILENCE );
	m_player->Add( Core::PROPERTY::EXPERIENCE );
	m_player->Add( Core::PROPERTY::LEVEL );
	m_player->Add( Core::PROPERTY::SPEED );

	m_player->Add( Core::PROPERTY::ATTITUDE );
	m_player->Add( Core::PROPERTY::CLASS );
	m_player->Add( Core::PROPERTY::FOLK );
	m_player->Add( Core::PROPERTY::HOME );
	m_player->Add( Core::PROPERTY::FAITH );
	m_player->Add( Core::PROPERTY::SIZE );
	m_player->Add( Core::PROPERTY::AGE );
	m_player->Add( Core::PROPERTY::SEX );
	m_player->Add( Core::PROPERTY::WEIGHT );
	m_player->Add( Core::PROPERTY::HAIRCOLOR );
	m_player->Add( Core::PROPERTY::EYECOLOR );

	// Create a player sub-object containing his talents
	Core::Object* talentO = g_Game->GetWorld()->GetObject( g_Game->GetWorld()->NewObject( STR_EMPTY ) );
	m_player->Add( Core::PROPERTY::TALENTS ).AddObject(talentO->ID());
	talentO->Add( Core::PROPERTY::NAME ).SetValue( STR_PROP_TALENTS );
	talentO->GetProperty( STR_PROP_SPRITE ).SetRights( Core::Property::R_SYSTEMONLY );
	talentO->GetProperty( STR_PROP_NAME ).SetRights( Core::Property::R_SYSTEMONLY );
	talentO->Add( Core::PROPERTY::ACROBATICS );
	talentO->Add( Core::PROPERTY::BLUFFING );
	talentO->Add( Core::PROPERTY::DIPLOMACY );
	talentO->Add( Core::PROPERTY::INTIMIDATE );
	talentO->Add( Core::PROPERTY::UNLEASHING );
	talentO->Add( Core::PROPERTY::FLYING );
	talentO->Add( Core::PROPERTY::STEALTH );
	talentO->Add( Core::PROPERTY::CLIMBING );
	talentO->Add( Core::PROPERTY::MOTIVERECOGNITION );
	talentO->Add( Core::PROPERTY::RIDING );
	talentO->Add( Core::PROPERTY::ESTIMATE );
	talentO->Add( Core::PROPERTY::SWIMMING );
	talentO->Add( Core::PROPERTY::SURVIVALING );
	talentO->Add( Core::PROPERTY::MASQUERADING );
	talentO->Add( Core::PROPERTY::PERCEPTION );

	m_player->Add( Core::PROPERTY::INVENTORY );

	ShowPlayer();
}

void CharacterState::Create()
{
	if( m_player->HasProperty(STR_PROP_NAME) )
		m_player->GetProperty( STR_PROP_NAME ).SetValue( m_name->getText() );
	else if(!m_name->getText().isEmpty())
		// What there is no name? - Going to change that.
		m_player->Add( Core::PROPERTY::NAME ).SetValue( m_name->getText() );
	if( m_player->HasProperty(STR_PROP_OWNER) )
		m_player->GetProperty( STR_PROP_OWNER ).SetValue( m_name->getText() );

	// Set all the attributes which are not updated automatically
	if( m_player->HasProperty(STR_PROP_HEALTH) )
		m_player->GetProperty( STR_PROP_HEALTH ).SetValue(m_eTP->getText());
	if( m_player->HasProperty(STR_PROP_HEALTH_MAX) )
		m_player->GetProperty( STR_PROP_HEALTH_MAX ).SetValue(m_eTPMax->getText());
	if( m_player->HasProperty(STR_PROP_ARMORCLASS) )
		m_player->GetProperty( STR_PROP_ARMORCLASS ).SetValue(m_eRK->getText());
	if( m_player->HasProperty(STR_PROP_BASIC_ATTACK) )
		m_player->GetProperty( STR_PROP_BASIC_ATTACK ).SetValue(m_eBasicAttack->getText());
	if( m_player->HasProperty(STR_PROP_REFLEX) )
		m_player->GetProperty( STR_PROP_REFLEX ).SetValue(m_eReflex->getText());
	if( m_player->HasProperty(STR_PROP_WILL) )
		m_player->GetProperty( STR_PROP_WILL ).SetValue(m_eWill->getText());
	if( m_player->HasProperty(STR_PROP_RESILENCE) )
		m_player->GetProperty( STR_PROP_RESILENCE ).SetValue(m_eResilence->getText());
	if( m_player->HasProperty(STR_PROP_EXPERIENCE) )
		m_player->GetProperty( STR_PROP_EXPERIENCE ).SetValue(m_eExperience->getText());
	if( m_player->HasProperty(STR_PROP_LEVEL) )
		m_player->GetProperty( STR_PROP_LEVEL ).SetValue(m_eLevel->getText());
	if( m_player->HasProperty(STR_PROP_SPEED) )
		m_player->GetProperty( STR_PROP_SPEED ).SetValue(m_eSpeed->getText());

	if( m_player->HasProperty(STR_PROP_ATTITUDE) )
		// Find the right combobox entry
		m_player->GetProperty( STR_PROP_ATTITUDE ).SetValue(m_cAttitude->getSelectedItem());
	if( m_player->HasProperty(STR_PROP_CLASS) )
		m_player->GetProperty( STR_PROP_CLASS ).SetValue(m_eClass->getText());
	if( m_player->HasProperty(STR_PROP_FOLK) )
		m_player->GetProperty( STR_PROP_FOLK ).SetValue(m_eFolk->getText());
	if( m_player->HasProperty(STR_PROP_HOME) )
		m_player->GetProperty( STR_PROP_HOME ).SetValue(m_eHome->getText());
	if( m_player->HasProperty(STR_PROP_FAITH) )
		m_player->GetProperty( STR_PROP_FAITH ).SetValue(m_eFaith->getText());
	if( m_player->HasProperty(STR_PROP_SIZE) )
		m_player->GetProperty( STR_PROP_SIZE ).SetValue(m_eSize->getText());
	if( m_player->HasProperty(STR_PROP_AGE) )
		m_player->GetProperty( STR_PROP_AGE ).SetValue(m_eAge->getText());
	if( m_player->HasProperty(STR_PROP_SEX) )
		m_player->GetProperty( STR_PROP_SEX ).SetValue(m_cSex->getSelectedItem());
	if( m_player->HasProperty(STR_PROP_WEIGHT) )
		m_player->GetProperty( STR_PROP_WEIGHT ).SetValue(m_eWeight->getText());
	if( m_player->HasProperty(STR_PROP_HAIRCOLOR) )
		m_player->GetProperty( STR_PROP_HAIRCOLOR ).SetValue(m_eHair->getText());
	if( m_player->HasProperty(STR_PROP_EYECOLOR) )
		m_player->GetProperty( STR_PROP_EYECOLOR ).SetValue(m_eEye->getText());

	m_finished = true;
}


void CharacterState::AttributeStrengthChanged()
{
	// Send new data to player and recalculate dependent attributes
	try {
		m_player->GetProperty( STR_PROP_STRENGTH ).SetValue( m_eSt->getText() );
		int modificator = Utils::EvaluateFormula( m_player->GetProperty(STR_PROP_STRENGTH_MOD).Value(), &m_rand, m_player );
		m_eStMod->setText( std::to_string(modificator) );
	} catch(...) {}
}

void CharacterState::AttributeDexterityChanged()
{
	// Send new data to player and recalculate dependent attributes
	try {
		m_player->GetProperty( STR_PROP_DEXTERITY ).SetValue( m_eGe->getText() );
		int modificator = Utils::EvaluateFormula( m_player->GetProperty(STR_PROP_DEXTERITY_MOD).Value(), &m_rand, m_player );
		m_eGeMod->setText( std::to_string(modificator) );
	} catch(...) {}
}

void CharacterState::AttributeConstitutionChanged()
{
	// Send new data to player and recalculate dependent attributes
	try {
		m_player->GetProperty( STR_PROP_CONSTITUTION ).SetValue( m_eKo->getText() );
		int modificator = Utils::EvaluateFormula( m_player->GetProperty(STR_PROP_CONSTITUTION_MOD).Value(), &m_rand, m_player );
		m_eKoMod->setText( std::to_string(modificator) );
	} catch(...) {}
}

void CharacterState::AttributeIntelligenceChanged()
{
	// Send new data to player and recalculate dependent attributes
	try {
		m_player->GetProperty( STR_PROP_INTELLIGENCE ).SetValue( m_eIn->getText() );
		int modificator = Utils::EvaluateFormula( m_player->GetProperty(STR_PROP_INTELLIGENCE_MOD).Value(), &m_rand, m_player );
		m_eInMod->setText( std::to_string(modificator) );
	} catch(...) {}
}

void CharacterState::AttributeWisdomChanged()
{
	// Send new data to player and recalculate dependent attributes
	try {
		m_player->GetProperty( STR_PROP_WISDOM ).SetValue( m_eWe->getText() );
		int modificator = Utils::EvaluateFormula( m_player->GetProperty(STR_PROP_WISDOM_MOD).Value(), &m_rand, m_player );
		m_eWeMod->setText( std::to_string(modificator) );
	} catch(...) {}
}

void CharacterState::AttributeCharismaChanged()
{
	// Send new data to player and recalculate dependent attributes
	try {
		m_player->GetProperty( STR_PROP_CHARISMA ).SetValue( m_eCh->getText() );
		int modificator = Utils::EvaluateFormula( m_player->GetProperty(STR_PROP_CHARISMA_MOD).Value(), &m_rand, m_player );
		m_eChMod->setText( std::to_string(modificator) );
	} catch(...) {}
}

bool CharacterState::SetComponentEnable( tgui::Widget::Ptr _component, bool _enable )
{
	if( _enable )
	{
		_component->enable();
		_component->setTransparency(255);
	} else {
		_component->setTransparency(150);
		_component->disable();
	}

	// Pass through for ifs
	return _enable;
}

void CharacterState::ShowPlayer()
{
	if( m_player->HasProperty(STR_PROP_NAME) )	
		m_name->setText( m_player->GetProperty( STR_PROP_NAME ).Value() );
	if( SetComponentEnable( m_eSt, m_player->HasProperty(STR_PROP_STRENGTH) ) )	{
		m_eSt->setText( m_player->GetProperty( STR_PROP_STRENGTH ).Value() );
		AttributeStrengthChanged();
	}
	if( SetComponentEnable( m_eGe, m_player->HasProperty(STR_PROP_DEXTERITY) ) )	{
		m_eGe->setText( m_player->GetProperty( STR_PROP_DEXTERITY ).Value() );
		AttributeDexterityChanged();
	}
	if( SetComponentEnable( m_eKo, m_player->HasProperty(STR_PROP_CONSTITUTION) ) )	{
		m_eKo->setText( m_player->GetProperty( STR_PROP_CONSTITUTION ).Value() );
		AttributeConstitutionChanged();
	}
	if( SetComponentEnable( m_eIn, m_player->HasProperty(STR_PROP_INTELLIGENCE) ) )	{
		m_eIn->setText( m_player->GetProperty( STR_PROP_INTELLIGENCE ).Value() );
		AttributeIntelligenceChanged();
	}
	if( SetComponentEnable( m_eWe, m_player->HasProperty(STR_PROP_WISDOM) ) )	{
		m_eWe->setText( m_player->GetProperty( STR_PROP_WISDOM ).Value() );
		AttributeWisdomChanged();
	}
	if( SetComponentEnable( m_eCh, m_player->HasProperty(STR_PROP_CHARISMA) ) )	{
		m_eCh->setText( m_player->GetProperty( STR_PROP_CHARISMA ).Value() );
		AttributeCharismaChanged();
	}

	if( SetComponentEnable( m_eTP, m_player->HasProperty(STR_PROP_HEALTH) ) )
		m_eTP->setText( m_player->GetProperty( STR_PROP_HEALTH ).Value() );
	if( SetComponentEnable( m_eTPMax, m_player->HasProperty(STR_PROP_HEALTH_MAX) ) )
		m_eTPMax->setText( m_player->GetProperty( STR_PROP_HEALTH_MAX ).Value() );
	if( SetComponentEnable( m_eRK, m_player->HasProperty(STR_PROP_ARMORCLASS) ) )
		m_eRK->setText( m_player->GetProperty( STR_PROP_ARMORCLASS ).Value() );
	if( SetComponentEnable( m_eBasicAttack, m_player->HasProperty(STR_PROP_BASIC_ATTACK) ) )
		m_eBasicAttack->setText( m_player->GetProperty( STR_PROP_BASIC_ATTACK ).Value() );
	if( SetComponentEnable( m_eReflex, m_player->HasProperty(STR_PROP_REFLEX) ) )
		m_eReflex->setText( m_player->GetProperty( STR_PROP_REFLEX ).Value() );
	if( SetComponentEnable( m_eWill, m_player->HasProperty(STR_PROP_WILL) ) )
		m_eWill->setText( m_player->GetProperty( STR_PROP_WILL ).Value() );
	if( SetComponentEnable( m_eResilence, m_player->HasProperty(STR_PROP_RESILENCE) ) )
		m_eResilence->setText( m_player->GetProperty( STR_PROP_RESILENCE ).Value() );
	if( SetComponentEnable( m_eExperience, m_player->HasProperty(STR_PROP_EXPERIENCE) ) )
		m_eExperience->setText( m_player->GetProperty( STR_PROP_EXPERIENCE ).Value() );
	if( SetComponentEnable( m_eLevel, m_player->HasProperty(STR_PROP_LEVEL) ) )
		m_eLevel->setText( m_player->GetProperty( STR_PROP_LEVEL ).Value() );
	if( SetComponentEnable( m_eSpeed, m_player->HasProperty(STR_PROP_SPEED) ) )
		m_eSpeed->setText( m_player->GetProperty( STR_PROP_SPEED ).Value() );

	if( SetComponentEnable( m_cAttitude, m_player->HasProperty(STR_PROP_ATTITUDE) ) )
		// Find the right combobox entry
		m_cAttitude->setSelectedItem( m_player->GetProperty( STR_PROP_ATTITUDE ).Value() );
	if( SetComponentEnable( m_eClass, m_player->HasProperty(STR_PROP_CLASS) ) )
		m_eClass->setText( m_player->GetProperty( STR_PROP_CLASS ).Value() );
	if( SetComponentEnable( m_eFolk, m_player->HasProperty(STR_PROP_FOLK) ) )
		m_eFolk->setText( m_player->GetProperty( STR_PROP_FOLK ).Value() );
	if( SetComponentEnable( m_eHome, m_player->HasProperty(STR_PROP_HOME) ) )
		m_eHome->setText( m_player->GetProperty( STR_PROP_HOME ).Value() );
	if( SetComponentEnable( m_eFaith, m_player->HasProperty(STR_PROP_FAITH) ) )
		m_eFaith->setText( m_player->GetProperty( STR_PROP_FAITH ).Value() );
	if( SetComponentEnable( m_eSize, m_player->HasProperty(STR_PROP_SIZE) ) )
		m_eSize->setText( m_player->GetProperty( STR_PROP_SIZE ).Value() );
	if( SetComponentEnable( m_eAge, m_player->HasProperty(STR_PROP_AGE) ) )
		m_eAge->setText( m_player->GetProperty( STR_PROP_AGE ).Value() );
	if( SetComponentEnable( m_cSex, m_player->HasProperty(STR_PROP_SEX) ) )
		m_cSex->setSelectedItem( m_player->GetProperty( STR_PROP_SEX ).Value() );
	if( SetComponentEnable(m_eWeight, m_player->HasProperty(STR_PROP_WEIGHT) ) )
		m_eWeight->setText( m_player->GetProperty( STR_PROP_WEIGHT ).Value() );
	if( SetComponentEnable( m_eHair, m_player->HasProperty(STR_PROP_HAIRCOLOR) ) )
		m_eHair->setText( m_player->GetProperty( STR_PROP_HAIRCOLOR ).Value() );
	if( SetComponentEnable( m_eEye, m_player->HasProperty(STR_PROP_EYECOLOR) ) )
		m_eEye->setText( m_player->GetProperty( STR_PROP_EYECOLOR ).Value() );

	if( SetComponentEnable( m_playerTalents, m_player->HasProperty(STR_PROP_TALENTS) ) )
	{
		Core::ObjectID talentID = m_player->GetProperty(STR_PROP_TALENTS).GetObjects()[0];
		m_playerTalents->Show( g_Game->GetWorld(), g_Game->GetWorld()->GetObject(talentID) );
	}
}

} // namespace States