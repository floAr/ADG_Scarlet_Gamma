#include "states/CreditState.h"
#include "utils/Content.hpp"
#include "Game.hpp"

using namespace States;

States::CreditState::CreditState() :
	GameState()
{
	m_menuFont = Content::Instance()->LoadFont("media/arial.ttf");
	m_background = sf::Sprite(Content::Instance()->LoadTexture("media/main_bg.png"));
	m_background.setColor(sf::Color(64, 64, 64, 255));
	m_logo = sf::Sprite(Content::Instance()->LoadTexture("media/logo.png"));
	m_logo.setPosition(10, 25);

	// Prepare the gui
	m_gui.setWindow(g_Game->GetWindow());
	m_gui.setGlobalFont(m_menuFont);

	// Add GUI elements
	tgui::Label::Ptr label(m_gui);
	label->setAutoSize(true);
	label->setTextSize(24);
	label->setPosition(30, 200);
	label->setTextColor(sf::Color(170, 7, 4));
	label->setText("Johannes Jendersie\nFlorian Uhde\nDavid Kuri\n\nImage credits:\nCursors: http://opengameart.org/users/yd\n");
	SetGui(&m_gui);
}

void States::CreditState::Draw( sf::RenderWindow& win )
{
	SetGuiView();
	float sx = float(win.getSize().x) / m_background.getTexture()->getSize().x;
	float sy = float(win.getSize().y) / m_background.getTexture()->getSize().y;
	m_background.setScale( std::max(1.0f, sx), std::max(1.0f, sy) );
	win.draw(m_background);
	win.draw(m_logo);

	GameState::Draw(win);
}

void States::CreditState::KeyPressed( sf::Event::KeyEvent& key, bool guiHandled )
{
	m_finished = true;
}
