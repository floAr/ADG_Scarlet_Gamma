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
#include "utils/StringUtil.hpp"

namespace States {

	CommonState::CommonState() :
		m_zoom(Utils::Falloff::FT_QUADRATIC, 0.75f, 0.05f),
		m_selectionChanged(false),
		m_hiddenLayers(10, 0),
		m_draggedContent(nullptr),
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

		// Add hidden combatant panel
		m_combatantPanel = Interfaces::CombatantPanel::Ptr(m_gui);
		m_combatantPanel->hide();

		SetGui(&m_gui);
	}


	CommonState::~CommonState()
	{
		delete m_draggedContent;
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

		if( !g_Game->GetWorld()->IsPaused() )
		{
			// Update continuous actions
			Actions::ActionPool::Instance().UpdateExecution();
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
			if( GetCurrentMap() )
			{
				Core::ObjectList targets = GetCurrentMap()->GetObjectsAt(tilePos.x, tilePos.y);
				if (targets.Size() == 0)
					Actions::ActionPool::Instance().UpdateDefaultAction(m_selection, 0);
				else
					Actions::ActionPool::Instance().UpdateDefaultAction(m_selection, g_Game->GetWorld()->GetObject(targets[targets.Size() - 1]));
			}
		}
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
			if( !enterTextEdit->isFocused() )
			{
				//	enterTextEdit->setText("");
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
		m_combatantPanel->Resize(_size, 200);
		GetStateView().setSize(_size);
	}


	void CommonState::ZoomView(float delta)
	{
		// Get the render window
		sf::RenderWindow& win = g_Game->GetWindow();

		// Create a new view with its center shifted by the mouse position
		sf::Vector2f center = win.getView().getCenter();
		sf::View& view = GetStateView();

		view.zoom(exp(delta * 0.1f));

		// Clamp scale so that one tile doesn't get smaller than 1 pixel
		// This is required to avoid overdraw, which blocks GPU threads and
		// leads to all pixels be drawn serially. Welcome, 1 fps!
		static const float clampFactor = 0.25f; 
		if (view.getSize().x / win.getSize().x > clampFactor * TILESIZE ||
			view.getSize().y / win.getSize().y > clampFactor * TILESIZE)
		{
			view.setSize(win.getSize().x * TILESIZE * clampFactor,
				win.getSize().y * TILESIZE * clampFactor);
		}

		if (view.getSize().x / win.getSize().x < clampFactor || 
			view.getSize().y / win.getSize().y < clampFactor)
		{
			view.setSize(win.getSize().x * clampFactor, win.getSize().y * clampFactor);
		}

		// Apply view to the window
		SetStateView();
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
					Core::Object* object = g_Game->GetWorld()->GetObject(wayPoints[i]);
					if( !object ) return;		// Path corrupted
					sf::Vector2i goal = sfUtils::Round(object->GetPosition());
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
		enterTextEdit->unfocus();
		// Send Message
		if( enterTextEdit->getText() != "" )
		{
			std::string text = enterTextEdit->getText().toAnsiString();
			sf::Color textColor = m_color;
			std::string sendText;
			if(text[0] == '/'){
				std::vector<std::string> arguments = Utils::ISplitString(text,' ');
				if(Utils::IStringEqual(arguments[0],"/me")) //emote for the current player
				{
					textColor = sf::Color(135, 206, 235);
					sendText = m_name; //write owners name
					auto it =arguments.begin();
					it++; //skip command
					for (it;it!=arguments.end();it++){ //parse into the text
						sendText += " " + *it ;
					}				
				}
				else
				{
					if(Utils::IStringEqual(arguments[0],"/e")) // text as emote
					{
						if(arguments.size()>1){
							textColor = sf::Color(135, 206, 235);
							auto it =arguments.begin();
							it++; //skip command
							for (it;it!=arguments.end();it++){ //parse in text
								sendText += " " + *it ;
							}	
						}
						else
							sendText = "Bitte Syntax beachten: /e TEXT";
					}
					else
					{
						if(Utils::IStringEqual(arguments[0],"/npc")) // text as emote
						{
							if(arguments.size()>2){
								textColor = sf::Color(135, 206, 235);
								auto it =arguments.begin();
								it++; //skip command
								sendText = '[' + *it + "] "; //put in name
								it++;
								for (it;it!=arguments.end();it++){ //parse in text
									sendText += " " + *it ;
								}	
							}
							else
								sendText = "Bitte syntax beachten: /npc NAME TEXT";
						}
						else
						{
							sendText = "Unbekanntes Kommando";
						}
					}
				}
			}
			else
			{
				tgui::ChatBox::Ptr localOut = m_gui.get( "Messages" );
				sendText = '[' + m_name + "] " + text;
			}
			Network::ChatMsg(sendText, textColor).Send();
			enterTextEdit->setText( STR_EMPTY );
		}
	}


	void CommonState::AddToSelection( Core::ObjectID _id )
	{
		// Assumes that the selection state only calls this if possible
		// which is the case due to toggeling
		//assert( !m_selection.Contains(_id) );
		if(m_selection.Contains(_id))
			return;
		m_selection.Add(_id);
		m_selectionChanged = true;
	}

	void CommonState::RemoveFromSelection( Core::ObjectID _id )
	{
		m_selection.Remove(_id);
		m_selectionChanged = true;
	}


	void CommonState::GoTo( const Core::Object* _object )
	{
		// Center the view at the object
		try {
			sf::Vector2f pos = _object->GetPosition();
			sf::Vector2f viewPos = pos * float(TILESIZE);
			GetStateView().setCenter(viewPos);
			SetStateView();
		} catch(...) {
			g_Game->AppendToChatLog( Network::ChatMsg( STR_NO_POSITION, sf::Color::Red) );
		}
	}


	void CommonState::EndCombat()
	{
		// Delete combat object, in case of DM he will spread the word
		delete m_combat;
		m_combat = 0;

		// Hide the combatant display
		m_combatantPanel->hide();
		Resize(sf::Vector2f((float) g_Game->GetWindow().getSize().x,
			(float) g_Game->GetWindow().getSize().y));
	}


	int CommonState::AutoDetectLayer( const Core::Object* _object )
	{
		// Use previous layer
		if( _object->HasProperty(STR_PROP_LAYER) )
			return _object->GetLayer();

		// Try to find a semantic
		if( _object->HasProperty(STR_PROP_ITEM) )
			return 4;
		if( _object->HasProperty(STR_PROP_OBSTACLE) )
			return 3;
		if( _object->HasProperty(STR_PROP_PLAYER) )
			return 6;
		if( _object->HasProperty(STR_PROP_HEALTH) )	// No player but attackable
			return 8;

		// Search the topmost visible layer
		for( int i = 10; i >= 1; --i )
			if( IsLayerVisible(i) ) return i;

		return 10;
	}


	Core::ObjectID CommonState::FindTopmostTile(int _x, int _y)
	{
		if( !GetCurrentMap() ) return Core::INVALID_ID;
		auto& objectList = GetCurrentMap()->GetObjectsAt(_x, _y);
		Core::ObjectID topmostObject = Core::INVALID_ID;
		int maxLayer = -1000;
		for (int i = objectList.Size()-1; i >= 0; --i)
		{
			// If object is not on hidden layer it is worth a closer look
			int layer = g_Game->GetWorld()->GetObject(objectList[i])->GetLayer();
			if( !IsLayerVisible(layer) && layer > maxLayer )
			{
				topmostObject = objectList[i];
				maxLayer = layer;
			}
		}
		return topmostObject;
	}

	void CommonState::OnResume()
	{
		// Mouse moved already handles what we need
		MouseMoved(0, 0, false);
	}

	void CommonState::OnPause()
	{
		Actions::ActionPool::Instance().UpdateDefaultAction(Core::ObjectList(), 0);
	}

	void CommonState::CreateDiceRollState(){
		// Create Dicerollstate
		m_diceRollState = static_cast<PromptState*>(g_Game->GetStateMachine()->PushGameState(States::GST_PROMPT));
		m_diceRollState->SetText("Zu evaluierende Formel eingeben");
		m_diceRollState->SetTextInputRequired(true);
		m_diceRollState->SetOrbSprite("media/orb_diceroll.png");
		m_diceRollState->SetMinimized(true);
		m_diceRollState->SetKeepAlive(true);
	}

} // namespace States