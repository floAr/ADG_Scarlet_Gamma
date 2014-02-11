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

namespace States {

NewPlayerState::NewPlayerState(tgui::EditBox::Ptr _nameEdit, Core::ObjectID* _saveID) :
    GameState(),
	m_nameOutputEdit(_nameEdit),
	m_name(nullptr),
	m_newPlayer(_saveID)
{
	m_menuFont = Content::Instance()->LoadFont("media/arial.ttf");

    //--------------------------------------
    // CREATE GUI
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
	tgui::ComboBox::Ptr cAttitude( m_gui );
	cAttitude->load( "media/Black.conf" );
	cAttitude->setSize( 200.0f, 30.0f );
	cAttitude->setPosition( 460.0f, 30.0f );
	cAttitude->addItem( STR_ATT_HONESTGOOD );
	cAttitude->addItem( STR_ATT_NEUTRALGOOD );
	cAttitude->addItem( STR_ATT_CHAOTICGOOD );
	cAttitude->addItem( STR_ATT_HONESTNEUTRAL );
	cAttitude->addItem( STR_ATT_NEUTRALNEUTRAL );
	cAttitude->addItem( STR_ATT_CHAOTICNEUTRAL );
	cAttitude->addItem( STR_ATT_HONESTEVIL );
	cAttitude->addItem( STR_ATT_NEUTRALEVIL );
	cAttitude->addItem( STR_ATT_CHAOTICEVIL );
	cAttitude->setSelectedItem( 0 );
	// Faith
	label = label.clone();							m_gui.add(label);
	label->setPosition( 700.0f, 35.0f );
	label->setText( STR_PROP_FAITH );
	tgui::EditBox::Ptr eFaith = m_name.clone();		m_gui.add(eFaith);
	eFaith->setPosition( 800.0f, 30.0f );

	// Enter class
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 75.0f );
	label->setText( STR_PROP_CLASS );
	tgui::EditBox::Ptr eClass = m_name.clone();		m_gui.add(eClass);
	eClass->setPosition( 120.0f, 70.0f );
	// Folk
	label = label.clone();							m_gui.add(label);
	label->setPosition( 360.0f, 75.0f );
	label->setText( STR_PROP_FOLK );
	tgui::EditBox::Ptr eFolk = m_name.clone();		m_gui.add(eFolk);
	eFolk->setPosition( 460.0f, 70.0f );
	// Home
	label = label.clone();							m_gui.add(label);
	label->setPosition( 700.0f, 75.0f );
	label->setText( STR_PROP_HOME );
	tgui::EditBox::Ptr eHome = m_name.clone();		m_gui.add(eHome);
	eHome->setPosition( 800.0f, 70.0f );

	// Size
	label = label.clone();							m_gui.add(label);
	label->setPosition( 20.0f, 118.0f );
	label->setText( STR_PROP_SIZE );
	label->setTextSize( 16 );
	tgui::EditBox::Ptr eSize = m_name.clone();		m_gui.add(eSize);
	eSize->setPosition( 75.0f, 110.0f );
	eSize->setSize( 80.0f, 30.0f );
	// Age
	label = label.clone();							m_gui.add(label);
	label->setPosition( 175.0f, 118.0f );
	label->setText( STR_PROP_AGE );
	tgui::EditBox::Ptr eAge = eSize.clone();		m_gui.add(eAge);
	eAge->setPosition( 220.0f, 110.0f );
	// Sex
	label = label.clone();							m_gui.add(label);
	label->setPosition( 320.0f, 118.0f );
	label->setText( STR_PROP_SEX );
	tgui::ComboBox::Ptr cSex = cAttitude.clone();	m_gui.add(cSex);
	cSex->setPosition( 403.0f, 110.0f );
	cSex->setSize( 70.0f, 30.0f );
	cSex->removeAllItems();
	cSex->addItem(STR_SEX_M);
	cSex->addItem(STR_SEX_F);
	cSex->setSelectedItem( 0 );
	// Weight
	label = label.clone();							m_gui.add(label);
	label->setPosition( 498.0f, 118.0f );
	label->setText( STR_PROP_WEIGHT );
	tgui::EditBox::Ptr eWeight = eSize.clone();		m_gui.add(eWeight);
	eWeight->setPosition( 560.0f, 110.0f );
	// Hair color
	label = label.clone();							m_gui.add(label);
	label->setPosition( 660.0f, 118.0f );
	label->setText( STR_PROP_HAIRCOLOR );
	tgui::EditBox::Ptr eHair = eSize.clone();		m_gui.add(eHair);
	eHair->setPosition( 737.0f, 110.0f );
	// Eye color
	label = label.clone();							m_gui.add(label);
	label->setPosition( 834.0f, 118.0f );
	label->setText( STR_PROP_EYECOLOR );
	tgui::EditBox::Ptr eEye = eSize.clone();		m_gui.add(eEye);
	eEye->setPosition( 920.0f, 110.0f );

	// Create and Cancel
	tgui::Button::Ptr button( m_gui );
	button->load("media/Black.conf");
	button->setText( STR_CANCEL );
	button->setPosition( 500.0f, 700.0f );
	button->setSize( 200.0f, 40.0f );
	button->setTextSize( 26 );
	button->setCallbackId( 1 );
	button->bindCallback( &NewPlayerState::Cancel, this, tgui::Button::LeftMouseClicked );
	button = tgui::Button::Ptr( m_gui );
	button->load("media/Black.conf");
	button->setText( STR_CREATE );
	button->setPosition( 740.0f, 700.0f );
	button->setSize( 200.0f, 40.0f );
	button->setTextSize( 26 );
	button->setCallbackId( 2 );
	button->bindCallback( &NewPlayerState::Create, this, tgui::Button::LeftMouseClicked );
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


void NewPlayerState::Cancel()
{
	m_finished = true;
}


void NewPlayerState::Create()
{
	// Create a new player object
	Core::ObjectID objID = g_Game->GetWorld()->NewObject("media/smile_2.png");
	Core::Object* obj = g_Game->GetWorld()->GetObject(objID);
	obj->Add( Core::PROPERTY::NAME ).SetValue( m_name->getText() );
	obj->Add( Core::PROPERTY::OWNER ).SetValue( m_name->getText() );
	obj->Add( Core::PROPERTY::PLAYER );
	obj->Add( Core::PROPERTY::TARGET );
	obj->Add( Core::PROPERTY::PATH );

	m_nameOutputEdit->setText( m_name->getText() );

	// The problem is that during player construction there is no connection ->
	// Object is not synchronized. It is send the moment we get a connection
	*m_newPlayer = objID;

	m_finished = true;
}

} // namespace States