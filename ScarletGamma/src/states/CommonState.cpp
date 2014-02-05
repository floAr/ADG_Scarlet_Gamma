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
#include "gamerules/Combat.hpp"
#include "states/PromptState.hpp"
#include "StateMachine.hpp"
#include "network/CombatMessages.hpp"
#include "events/InputHandler.hpp"

namespace States {

CommonState::CommonState() :
		m_zoom(Utils::Falloff::FT_QUADRATIC, 0.75f, 0.05f),
		m_selectionChanged(false),
		m_combat(0)
{
	// Tell the game about it's common state. Using Messenger::IsServer() you can take
	//   a wild guess whether this is a PlayerState or MasterState
	g_Game->SetCommonState(this);

	m_gui.setWindow(g_Game->GetWindow());
	m_gui.setGlobalFont(Content::Instance()->LoadFont("media/arial.ttf"));
	if( !m_gui.loadWidgetsFromFile( "media/Chat.gui" ) )
		std::cerr << "[CommonState::CommonState] Could not load GUI for chat.\n";
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


void CommonState::MouseMoved(int deltaX, int deltaY, bool guiHandled)
{
	// Update default action if we are not in the GUI
	if (guiHandled)
	{
		Actions::ActionPool::Instance().UpdateDefaultAction(m_selection, 0);
	}
	else
	{
		sf::Vector2i tilePos = Events::InputHandler::GetMouseTilePosition();
		Core::ObjectList targets = GetCurrentMap()->GetObjectsAt(tilePos.x, tilePos.y);
		if (targets.Size() == 0)
			Actions::ActionPool::Instance().UpdateDefaultAction(m_selection, 0);
		else
			Actions::ActionPool::Instance().UpdateDefaultAction(m_selection, g_Game->GetWorld()->GetObject(targets[targets.Size() - 1]));
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
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


void CommonState::MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled)
{
	// Don't react to any key if gui handled it
	if (guiHandled)
		return;

	m_zoom = (float)wheel.delta;
}


void CommonState::KeyPressed( sf::Event::KeyEvent& key, bool guiHandled )
{
	// Don't react to any key if gui handled it
	if (guiHandled)
		return;

	switch(key.code)
	{
	case sf::Keyboard::Return: {
		// Show message input field if not visible or submit message.
		tgui::EditBox::Ptr enterTextEdit = m_gui.get( "EnterText" );
		if( !enterTextEdit->isVisible() )
		{
			enterTextEdit->show();
			enterTextEdit->setText("");
			enterTextEdit->focus();
		}
		break; }
	case sf::Keyboard::Num0:
	case sf::Keyboard::Numpad0: {
		if( sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
			|| sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) )
		{
			// Reset zoom
			m_zoom = 0;
			sf::RenderWindow& win = g_Game->GetWindow();
			sf::View newView = win.getView();
			newView.setSize((float)win.getSize().x, (float)win.getSize().y);
			win.setView(newView);
		}
		break; }
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
		if( _whosePath->HasProperty(STR_PROP_PATH) )
		{
			// For each way point compute the shortest path
			Core::Property& pathProperty = _whosePath->GetProperty(STR_PROP_PATH);
			auto& wayPoints = pathProperty.GetObjects();
			for( int i=0; i<wayPoints.Size(); ++i )
			{
				sf::Vector2i goal = sfUtils::Round(g_Game->GetWorld()->GetObject(wayPoints[i])->GetPosition());
				auto part = g_Game->GetWorld()->GetMap(_whosePath->GetParentMap())->FindPath(start, goal);
				start = goal;
				path.insert( path.end(), part.begin(), part.end() );
			}
			// Loop?
			if( pathProperty.Value() == "true" )
			{
				sf::Vector2i goal = sfUtils::Round(g_Game->GetWorld()->GetObject(wayPoints[0])->GetPosition());
				auto part = g_Game->GetWorld()->GetMap(_whosePath->GetParentMap())->FindPath(start, goal);
				path.insert( path.end(), part.begin(), part.end() );
			}
		} else if( _whosePath->HasProperty(STR_PROP_TARGET) ) {
			// There are no paths but a short time target.
			sf::Vector2i goal = sfUtils::Round(sfUtils::to_vector(_whosePath->GetProperty(STR_PROP_TARGET).Value()));
			path = g_Game->GetWorld()->GetMap(_whosePath->GetParentMap())->FindPath(start, goal);
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

void CommonState::BeginCombat()
{
	m_combat = new GameRules::Combat();

	// Prompt for initiative roll
	PromptState* prompt = dynamic_cast<PromptState*>(
		g_Game->GetStateMachine()->PushGameState(States::GST_PROMPT));
	prompt->SetText("Angriffswurf eingeben:");
	prompt->AddPopCallback(std::bind(&CommonState::InitiativeRollPromptFinished, this, std::placeholders::_1));
}

void CommonState::InitiativeRollPromptFinished(States::GameState* ps)
{
	PromptState* prompt = dynamic_cast<PromptState*>(ps);
	assert(ps);

	// Send initiative roll string to server
	Jo::Files::MemFile data;
	const std::string& result = prompt->GetResult().c_str();
	data.Write(&m_selection[0], sizeof(m_selection[0]));
	data.Write(result.c_str(), result.length());
	Network::CombatMsg(Network::CombatMsgType::PL_COMBAT_INITIATIVE).Send(&data);
}

void CommonState::EndCombat()
{
	delete(m_combat);
	m_combat = 0;
}


} // namespace States