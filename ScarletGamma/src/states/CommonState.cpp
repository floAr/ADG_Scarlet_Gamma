#include <cassert>

#include "Game.hpp"
#include "Constants.hpp"
#include "CommonState.hpp"
#include "network/Messenger.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/Object.hpp"
#include "core/World.hpp"
#include "network/ChatMessages.hpp"
#include "actions/ActionPool.hpp"

namespace States {

CommonState::CommonState() :
		m_zoom(Utils::Falloff::FT_QUADRATIC, 0.75f, 0.05f),
		m_selected(nullptr),
		m_selectionChanged(false)
{
	m_gui.setWindow(g_Game->GetWindow());
	m_gui.setGlobalFont(Content::Instance()->LoadFont("media/arial.ttf"));
	if( !m_gui.loadWidgetsFromFile( "media/Chat.gui" ) )
		std::cout << "[CommonState::CommonState] Could not load GUI for chat.\n";
	tgui::EditBox::Ptr enterTextEdit = m_gui.get( "EnterText" );
	enterTextEdit->bindCallbackEx( &CommonState::SubmitChat, this, tgui::EditBox::ReturnKeyPressed );

	SetGui(&m_gui);
}


void CommonState::OnEnd()
{
	Network::Messenger::Close();
}



void CommonState::Update( float dt )
{
	// Handle network events
	Network::Messenger::Poll( false );

	m_zoom.Update(dt);
	if( m_zoom != 0 )
		ZoomView(m_zoom);

	while( g_Game->HasLoggedNewChatMessages() )
	{
		tgui::ChatBox::Ptr localOut = m_gui.get( "Messages" );
		const Network::ChatMsg& chatMessage = g_Game->GetNextUntreatedChatMessage();
		localOut->addLine(chatMessage.Text(), chatMessage.Color());
		// STRANGE HACK: set the m_Panel property public in ChatBox.hpp.
		// This solves a bug where lines are cut at the bottom.
		tgui::Label::Ptr newestLine = localOut->m_Panel->getWidgets().back();
		//localOut->m_FullTextHeight = (newestLine->getSize().y+5.0f) * localOut->getLineAmount();
		newestLine->setSize(newestLine->getSize().x, newestLine->getSize().y+5.0f);
		localOut->m_FullTextHeight += localOut->getLineAmount() == 1 ? 10.0f : 5.0f;
	}
}


void CommonState::MouseMoved(int deltaX, int deltaY)
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		// Get the render window
		sf::RenderWindow& win = g_Game->GetWindow();

		// Create a new view with its center shifted by the mouse position
		sf::Vector2f center = win.getView().getCenter();
		sf::View newView = win.getView();
		sf::Vector2f scale(newView.getSize().x / win.getSize().x,
			newView.getSize().y / win.getSize().y);
		newView.setCenter(center.x - (deltaX * scale.x),
			center.y - (deltaY * scale.y));

		// Apply view to the window
		win.setView(newView);
	}
}


void CommonState::MouseWheelMoved(sf::Event::MouseWheelEvent& wheel)
{
	m_zoom = (float)wheel.delta;
}


void CommonState::KeyPressed( sf::Event::KeyEvent& key )
{
	if( key.code == sf::Keyboard::Return )
	{
		// Show message input field if not visible or submit message.
		tgui::EditBox::Ptr enterTextEdit = m_gui.get( "EnterText" );
		if( !enterTextEdit->isVisible() )
		{
			enterTextEdit->show();
			enterTextEdit->setText("");
			enterTextEdit->focus();
		}
	}

    // Testing attack action
    // TODO: remove
    if ( key.code == sf::Keyboard::Space )
    {
        Actions::ActionPool::Instance().StartAction(0);
    }
}


void CommonState::Resize(const sf::Vector2f& _size)
{
	tgui::ChatBox::Ptr localOut = m_gui.get( "Messages" );
	tgui::EditBox::Ptr enterTextEdit = m_gui.get( "EnterText" );
	localOut->setPosition( _size - localOut->getSize() - sf::Vector2f(0.0f, enterTextEdit->getSize().y) );
	enterTextEdit->setPosition( _size - enterTextEdit->getSize() );
}


void CommonState::ZoomView(float delta)
{
	// Get the render window
	sf::RenderWindow& win = g_Game->GetWindow();

	// Create a new view with its center shifted by the mouse position
	sf::Vector2f center = win.getView().getCenter();
	sf::View newView = win.getView();

	newView.zoom(exp(delta * 0.1f));

	// Clamp scale so that one tile doesn't get smaller than 1 pixel
	// This is required to avoid overdraw, which blocks GPU threads and
	// leads to all pixels be drawn serially. Welcome, 1 fps!
	static const float clampFactor = 0.25f; 
	if (newView.getSize().x / win.getSize().x > clampFactor * TILESIZE ||
		newView.getSize().y / win.getSize().y > clampFactor * TILESIZE)
	{
		newView.setSize(win.getSize().x * TILESIZE * clampFactor,
			win.getSize().y * TILESIZE * clampFactor);
	}

	if (newView.getSize().x / win.getSize().x < clampFactor || 
		newView.getSize().y / win.getSize().y < clampFactor)
	{
		newView.setSize(win.getSize().x * clampFactor, win.getSize().y * clampFactor);
	}

	// Apply view to the window
	win.setView(newView);
}


void CommonState::DrawPathOverlay(sf::RenderWindow& _window, Core::Object* _whosePath)
{
	assert( _whosePath );
	try {
		std::vector<sf::Vector2i> path;
		sf::Vector2i start(sfUtils::Round(_whosePath->GetPosition()));
		if( _whosePath->HasProperty(Core::Object::PROP_PATH) )
		{
			// For each way point compute the shortest path
			Core::Property& pathProperty = _whosePath->GetProperty(Core::Object::PROP_PATH);
			auto& wayPoints = pathProperty.GetObjects();
			for( int i=0; i<wayPoints.Size(); ++i )
			{
				sf::Vector2i goal = sfUtils::Round(g_Game->GetWorld()->GetObject(wayPoints[i])->GetPosition());
				auto part = g_Game->GetWorld()->GetMap(0)->FindPath(start, goal);
				start = goal;
				path.insert( path.end(), part.begin(), part.end() );
			}
			// Loop?
			if( pathProperty.Value() == "true" )
			{
				sf::Vector2i goal = sfUtils::Round(g_Game->GetWorld()->GetObject(wayPoints[0])->GetPosition());
				auto part = g_Game->GetWorld()->GetMap(0)->FindPath(start, goal);
				path.insert( path.end(), part.begin(), part.end() );
			}
		} else if( _whosePath->HasProperty(Core::Object::PROP_TARGET) ) {
			// There are no paths but a short time target.
			sf::Vector2i goal = sfUtils::Round(sfUtils::to_vector(_whosePath->GetProperty(Core::Object::PROP_TARGET).Value()));
			path = g_Game->GetWorld()->GetMap(0)->FindPath(start, goal);
		}

		if( path.size() > 0 )
			Graphics::TileRenderer::RenderPath(_window, path);
	} catch(...) {
		// In case of an invalid selection just draw no path
	}
}



void CommonState::SubmitChat(const tgui::Callback& _call)
{
	tgui::EditBox* enterTextEdit = (tgui::EditBox*)_call.widget;
	enterTextEdit->hide();
	// Send Message
	if( enterTextEdit->getText() != "" )
	{
		tgui::ChatBox::Ptr localOut = m_gui.get( "Messages" );
		std::string text = '[' + m_name + "] " + enterTextEdit->getText().toAnsiString();
		Network::ChatMsg(text, m_color).Send();
	}
}

void CommonState::AddToSelection( Core::ObjectID _id )
{
	// Assumes that the selection state only calls this if possible
	// which is the case due to toggeling
	assert( !m_selection.Contains(_id) );
	m_selection.Add(_id);
	m_selectionChanged = true;
}

void CommonState::RemoveFromSelection( Core::ObjectID _id )
{
	m_selection.Remove(_id);
	m_selectionChanged = true;
}


} // namespace States