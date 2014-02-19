#include "states/MasterState.hpp"
#include "Game.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"
#include "states/StateMachine.hpp"
#include "Constants.hpp"
#include "utils/Falloff.hpp"
#include "network/Messenger.hpp"
#include "interfaces/DragNDrop.hpp"
#include <iostream>
#include "core/PredefinedProperties.hpp"
#include "network/WorldMessages.hpp"
#include "events/InputHandler.hpp"
#include "network/CombatMessages.hpp"
#include "GameRules/MasterCombat.hpp"
#include "states/PromptState.hpp"
#include "NewPlayerState.hpp"
#include "DismissableDialogState.hpp"
#include "actions/ActionPool.hpp"
#include <functional>
#include "utils/Clipboard.hpp"
#include "utils/ValueInterpreter.hpp"

using namespace Core;

namespace States {

	MasterState::MasterState( const std::string& _loadFile ) :
		m_propertyPanel(nullptr),
		m_modulePanel(nullptr),
		m_objectsPanel(nullptr),
		m_selectionView(nullptr),
		m_toolbar(nullptr),
		m_worldFileName(_loadFile),
		m_rectSelection(false)
	{
		// Load the map
		Jo::Files::HDDFile file(_loadFile);
		g_Game->GetWorld()->Load( file );

		m_viewPanel = Interfaces::PropertyPanel::Ptr(m_gui);
		m_viewPanel->Init( 240.0f, 384.0f, 240.0f, 384.0f, false, false, 0, &m_draggedContent );

		// Load properties from database into gui
		m_propertyPanel = Interfaces::PropertyPanel::Ptr(m_gui);
		m_propertyPanel->Init( 0.0f, 0.0f, 240.0f, 384.0f, true, false, 0, &m_draggedContent );
		m_propertyPanel->Show( g_Game->GetWorld(), g_Game->GetWorld()->GetPropertyBaseObject() );

		// Load Modules from database into gui
		m_modulePanel = Interfaces::ObjectPanel::Ptr(m_gui);
		m_modulePanel->Init( 0.0f, 384.0f, 240.0f, 384.0f, true, Interfaces::DragContent::MODULES_PANEL, &m_draggedContent, m_viewPanel );

		// Load Objects from database into gui
		m_objectsPanel = Interfaces::ObjectPanel::Ptr(m_gui);
		m_objectsPanel->Init( 240.0f, 0.0f, 240.0f, 384.0f, true, Interfaces::DragContent::OBJECT_PANEL, &m_draggedContent, m_viewPanel );

		// The temporary selection menu
		m_selectionView = Interfaces::PropertyPanel::Ptr(m_gui);
		m_selectionView->Init(624.0f, 100.0f, 400.0f, 428.0f, false, false, 0, &m_draggedContent);
		m_selectionView->hide();

		m_toolbar = Interfaces::Toolbar::Ptr(m_gui);
		m_toolbar->Init( 480.0f, 0.0f, 544.0f, 100.0f );
		m_toolbar->AddToolbox( m_mapTool );
		m_toolbar->AddToolbox( m_modeTool );
		m_toolbar->AddToolbox( m_playerTool );
		m_playerTool->SetDragNDropHandler( &m_draggedContent );
		std::function<void(const Object*)> gotoFunc = std::bind(&MasterState::GoTo, this, std::placeholders::_1);
		m_playerTool->SetGoToMethod(gotoFunc);
		m_toolbar->AddToolbox( m_gotoTool );
		m_gotoTool->SetGoToMethod(gotoFunc);
		m_toolbar->AddToolbox( m_combatTool );

		// Set chat color...
		m_color = sf::Color(80,80,250);
		m_name = "Master";

		// Set window size
		sf::Vector2u size = g_Game->GetWindow().getSize();
		Resize( sf::Vector2f( (float) size.x, (float) size.y ) );
	}


	void MasterState::Update(float dt)
	{
		CommonState::Update(dt);

		g_Game->GetWorld()->Update(dt);

		m_toolbar->Update( dt );

		// Repair selection
		for( int i=0; i<m_selection.Size(); ++i )
		{
			Core::Object* obj = g_Game->GetWorld()->GetObject( m_selection[i] );
			// If removed from map or from game delete!
			if( !obj || !obj->IsLocatedOnAMap() )
			{
				m_selection.Remove( m_selection[i--] );
				m_selectionChanged = true;
			}
		}

		// For empty selections hide the selection view
		if( m_selectionChanged )
		{
			if( m_selection.Size() > 0 )
			{
				m_selectionView->show();
			} else m_selectionView->hide();
			m_selectionChanged = false;
		}

		// Update the viewed properties
		if( m_selectionView->isVisible() )
			m_selectionView->Show( g_Game->GetWorld(), m_selection );
		m_viewPanel->RefreshFilter();
	}

	void MasterState::Draw(sf::RenderWindow& win)
	{
		// Draw some color to the background
		static sf::Color c(20, 26, 36);
		win.clear(c);

		// Render
		if( GetCurrentMap() )
		{
			Graphics::TileRenderer::Render(win, *GetCurrentMap(),
				[](Core::Map&,sf::Vector2i&){ return 1.0f; },
				(const bool*)(&m_hiddenLayers[0]));
		}

		// If the selected object has a path draw it
		for( int i=0; i<m_selection.Size(); ++i )
			DrawPathOverlay(win, g_Game->GetWorld()->GetObject(m_selection[i]));

		Graphics::TileRenderer::RenderSelection( win, m_selection );

		// Show the brush region
		if( m_modeTool->GetMode() == Interfaces::ModeToolbox::BRUSH )
		{
			int r0 = m_modeTool->Brush()->GetDiameter() / 2, r1 = m_modeTool->Brush()->GetDiameter() - r0;
			sf::Vector2i mousePos = Events::InputHandler::GetMouseTilePosition();
			Graphics::TileRenderer::RenderRect( win, sf::Vector2i(mousePos.x-r0, mousePos.y-r0), sf::Vector2i(mousePos.x+r1, mousePos.y+r1) );
		}

		// Show the selection rect region
		if( m_modeTool->GetMode() == Interfaces::ModeToolbox::SELECTION && m_rectSelection )
		{
			sf::Vector2i mousePos = Events::InputHandler::GetMouseTilePosition();
			int minX,minY,maxX,maxY;
			ComputeSelectionRect( mousePos, minX, maxX, minY, maxY );
			Graphics::TileRenderer::RenderRect( win, sf::Vector2i(minX,minY), sf::Vector2i(maxX,maxY) );
		}

		GameState::Draw(win);
	}


	void MasterState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, bool guiHandled)
	{
		// Return if the GUI already handled it
		if (guiHandled)
			return;

		// The element was dropped unregistered (e.g. outside window)
		if( m_draggedContent )
		{
			delete m_draggedContent;
			m_draggedContent = nullptr;
		}

		int tileX = (int)floor(tilePos.x);
		int tileY = (int)floor(tilePos.y);
		switch (button.button)
		{
		case sf::Mouse::Left: {
			if( m_modeTool->GetMode() == Interfaces::ModeToolbox::BRUSH )
			{
				if( !GetCurrentMap() ) {
					Network::ChatMsg(STR_MSG_CREATE_MAP_FIRST, sf::Color::Red).Send();
					return;
				}
				if( !m_objectsPanel->GetSelected() ) {
					Network::ChatMsg(STR_MSG_SELECT_TYPE_FIRST, sf::Color::Red).Send();
					return;
				}
				// Paint new objects with the brush.
				const Object* obj = m_objectsPanel->GetSelected();
				int layer = m_modeTool->Brush()->GetLayer();
				if( layer == 0 ) layer = AutoDetectLayer( obj );
				if( IsLayerVisible( layer ) ) {
					Network::ChatMsg(STR_MSG_LAYER_INVISIBLE, sf::Color::Red).Send();
					return;
				}
				m_brush.BeginPaint( *GetCurrentMap(),
					obj,
					m_modeTool->Brush()->GetDiameter(),
					tileX, tileY,
					layer,
					m_modeTool->Brush()->GetMode() );
			} else
				if( m_modeTool->GetMode() == Interfaces::ModeToolbox::SELECTION )
				{
					// Start rect-selection
					m_rectSelectionStart.x = tileX;
					m_rectSelectionStart.y = tileY;
					m_rectSelection = true;
				} else if(m_modeTool->GetMode() == Interfaces::ModeToolbox::ACTION) {	// Mode ACTION
					// Default action
					if (m_selection.Size() > 0)
					{
						auto& tiles = GetCurrentMap()->GetObjectsAt(tileX,tileY);
						if( tiles.Size() > 0 )
						{
							Core::Object* object = g_Game->GetWorld()->GetObject(tiles[tiles.Size()-1]);
							Actions::ActionPool::Instance().UpdateDefaultAction(m_selection, object);
							for (int i=0; i < m_selection.Size(); ++i)
								Actions::ActionPool::Instance().StartDefaultAction(m_selection[i], object->ID());
						}
					}
				} else {	// Mode Drag&Drop
					// Start drag&drop with the topmost visible tile
					ObjectID topmostObject = FindTopmostTile(tileX, tileY);
					if( topmostObject != INVALID_ID ) {
						if( !m_draggedContent ) m_draggedContent = new Interfaces::DragContent();
						m_draggedContent->from = Interfaces::DragContent::MAP;
						m_draggedContent->object = g_Game->GetWorld()->GetObject(topmostObject);
						m_draggedContent->prop = nullptr;
					}
				}
				break; }
		case sf::Mouse::Right: {
			if( GetCurrentMap()->GetObjectsAt(tileX, tileY).Size() > 0 )
			{
				SelectionState* gs = dynamic_cast<SelectionState*>(g_Game->GetStateMachine()->PushGameState(GST_SELECTION));
				gs->SetTilePosition(tileX, tileY,(const bool*)(&m_hiddenLayers[0]));
			}

			break; }
		}
	}

	void MasterState::MouseButtonReleased( sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, float time, bool guiHandled)
	{
		// Always a good idea to stop painting
		m_brush.EndPaint();

		// Return if the GUI already handled it
		if (guiHandled)
			return;

		// Handle drop-event of drag&drop action
		if( m_draggedContent )
		{
			// Where is the element dropped?
			if( m_objectsPanel->mouseOnWidget( (float)button.x, (float)button.y ) )
			{
				m_objectsPanel->HandleDropEvent();
			} else if( m_modulePanel->mouseOnWidget( (float)button.x, (float)button.y ) )
			{
				m_modulePanel->HandleDropEvent();
			} else if( m_viewPanel->mouseOnWidget( (float)button.x, (float)button.y ) )
			{
				//	m_viewPanel->HandleDropEvent();
			} else if( m_propertyPanel->mouseOnWidget( (float)button.x, (float)button.y ) )
			{
				// Just ignore things dragged to the property list
			} else {	// Things were dragged to the map.
				if( !GetCurrentMap() )
				{
					Network::ChatMsg(STR_MSG_CREATE_MAP_FIRST, sf::Color::Red).Send();
				} else {
					int x = (int)floor(tilePos.x);
					int y = (int)floor(tilePos.y);
					if( m_draggedContent->from == Interfaces::DragContent::OBJECT_PANEL )
					{
						// Insert object copy to the map
						ObjectID id = g_Game->GetWorld()->NewObject( m_draggedContent->object );

						// Use auto detected layer - no knowledge
						GetCurrentMap()->Add( id, x, y, AutoDetectLayer(g_Game->GetWorld()->GetObject(id)) );
					} else if( m_draggedContent->from == Interfaces::DragContent::PLAYERS_LIST )
					{
						// Insert original to the map (player layer)
						Object* object = const_cast<Object*>(m_draggedContent->object);
						if( !object->IsLocatedOnAMap() )
							GetCurrentMap()->Add( object->ID(), x, y, 6 );
						else {
							GetCurrentMap()->SetObjectPosition( object, tilePos );
						}
						object->ResetTarget();
					} else if( m_draggedContent->from == Interfaces::DragContent::PROPERTY_PANEL )
					{
						if( m_draggedContent->object->HasProperty( STR_PROP_ITEM ) )
						{
							// Take away from the source object
							m_draggedContent->prop->RemoveObject(m_draggedContent->object->ID());
							// Insert into map
							GetCurrentMap()->Add( m_draggedContent->object->ID(), x, y, AutoDetectLayer(m_draggedContent->object) );
						}
					} else if( m_draggedContent->from == Interfaces::DragContent::MAP )
					{
						// Insert into map
						GetCurrentMap()->SetObjectPosition( m_draggedContent->object, sf::Vector2f((float)x,(float)y) );
					}
				}
			}

			delete m_draggedContent;
			m_draggedContent = nullptr;
		}

		if(m_rectSelection)
		{
			// If not pressing cntrl -> clear selection
			if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
				m_selection.Clear();

			sf::Vector2i tile((int)floor(tilePos.x), (int)floor(tilePos.y));
			// The cursor did not leave the tile -> interpret as a click only if time was also short
			if( tile.x==m_rectSelectionStart.x && tile.y==m_rectSelectionStart.y && time < 0.4f)
			{
				// Take only the topmost visible tile
				ObjectID topmostObject = FindTopmostTile(tile.x, tile.y);
				if( topmostObject != INVALID_ID )
					AddToSelection(topmostObject);
			} else if( GetCurrentMap() ) {
				int minX,minY,maxX,maxY;
				ComputeSelectionRect( tile, minX, maxX, minY, maxY );
				for(int sX = minX; sX < maxX; sX++)
				{
					for(int sY = minY; sY < maxY; sY++)
					{
						auto& objectList = GetCurrentMap()->GetObjectsAt(sX,sY);
						for (int i = 0; i < objectList.Size(); i++)
						{
							// if object is not on hidden layer add it
							if(!IsLayerVisible(g_Game->GetWorld()->GetObject(objectList[i])->GetLayer()))
								AddToSelection(objectList[i]);
						}
					}

				}
			}
			m_rectSelection=false;
		}
	}


	void MasterState::ComputeSelectionRect( const sf::Vector2i& _position, int& _minX, int& _maxX, int& _minY, int& _maxY )
	{
		if(_position.x < m_rectSelectionStart.x)
		{
			_minX = _position.x;
			_maxX = m_rectSelectionStart.x + 1;
		}
		else
		{
			_maxX = _position.x + 1;
			_minX = m_rectSelectionStart.x;
		}
		if(_position.y < m_rectSelectionStart.y)
		{
			_minY = _position.y;
			_maxY = m_rectSelectionStart.y + 1;
		}
		else
		{
			_maxY = _position.y + 1;
			_minY = m_rectSelectionStart.y;
		}
	}


	void MasterState::MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled)
	{
		// Manually scroll panels (tgui::callback not provided for this action).
		// The scroll call has an effect only if the mouse is on the respective
		// element. So calling it for everybody is just fine.
		m_propertyPanel->Scroll(wheel.delta);
		m_modulePanel->Scroll(wheel.delta);
		m_objectsPanel->Scroll(wheel.delta);
		m_viewPanel->Scroll(wheel.delta);
		m_selectionView->Scroll(wheel.delta);

		if (guiHandled)
			return;

		m_zoom = (float)wheel.delta;
	}


	void MasterState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
	{

		// This should work ALWAYS, even if GUI is focused:
		switch (key.code)
		{
			//on alt clear the current mask
		case sf::Keyboard::LAlt:
			int l;
			for(l = 0; l < 10; l++)
			{
				m_hiddenLayers[l] = 0;
			}
			m_firstLayerSelection=true;
			break;
		case sf::Keyboard::C:
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){//ctrl+c -> copy
				Utils::Clipboard::Instance()->SetClipboardText("Toll wenn es klappen würde");
			}
			break;
		case sf::Keyboard::V:
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){//ctrl+c -> copy
				std::string content=Utils::Clipboard::Instance()->GetClipboardText();
				for (auto i = content.begin(); i != content.end(); ++i){
					sf::Event kevent;

					//trying with textentered
					kevent.type=sf::Event::TextEntered;
					kevent.text.unicode=*i;
					m_gui.handleEvent(kevent);
				}
			}
			break;
			//for each key add the mask, as long as alt is pressed (maybe cache this in local field)
		case sf::Keyboard::Num1:
		case sf::Keyboard::Num2:
		case sf::Keyboard::Num3:
		case sf::Keyboard::Num4:
		case sf::Keyboard::Num5:
		case sf::Keyboard::Num6:
		case sf::Keyboard::Num7:
		case sf::Keyboard::Num8:
		case sf::Keyboard::Num9:
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
				BlendLayer(key.code - sf::Keyboard::Num1);
			break;
		case sf::Keyboard::Num0:
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
				BlendLayer(9);
			break;

			//case sf::Keyboard::T:
			//	PromptState* gs = dynamic_cast<PromptState*>(g_Game->GetStateMachine()->PushGameState(GST_PROMPT));
			//	gs->ConfigurePromp("Das ist dein toller Text",false);
			//	gs->AddButton("testbutton",std::bind(&MasterState::TestButtonCallback,this,std::placeholders::_1),sf::Vector2f(212,460));
			//	break;
		}
		// Return if the GUI already handled it
		if (guiHandled)
			return;

		// Let common state handle input
		CommonState::KeyPressed(key, guiHandled);

		// This should work only if the GUI didn't handle before
		switch(key.code)
		{
		case sf::Keyboard::C:
			// If only one object is selected show the character screen. Even
			// if it has no character properties. GM should know better...
			if( m_selection.Size() == 1 )
				g_Game->GetStateMachine()->PushGameState(new CharacterState(&m_selection[0]));
			break;

		case sf::Keyboard::Delete:
			// Delete all selected objects from the map(s)
			for( int i=0; i<m_selection.Size(); ++i )
			{
				ObjectID id = m_selection[i];
				Map* map = g_Game->GetWorld()->GetMap( g_Game->GetWorld()->GetObject(id)->GetParentMap() );
				map->Remove( id );
				g_Game->GetWorld()->RemoveObject( id );	// Assumes real deletion
			}
			m_selection.Clear();
			m_selectionView->hide();
			break;
		case sf::Keyboard::Pause:
		case sf::Keyboard::P:
			g_Game->GetWorld()->SetPause( !g_Game->GetWorld()->IsPaused() );
			break;
		}
	}

	void MasterState::BlendLayer(int layerID)
	{
		if(m_firstLayerSelection)
		{
			int l;
			for(l = 0; l < 10; l++)
			{
				m_hiddenLayers[l] = 1;
			}
			m_firstLayerSelection = false;
		}
		// Toggle state: if number is pressed again hide layer
		m_hiddenLayers[layerID] = 1 - m_hiddenLayers[layerID];
	}

	void MasterState::OnBegin()
	{
		// Init server
		Network::Messenger::Initialize(nullptr);



	}

	void MasterState::OnEnd()
	{
		CommonState::OnEnd();

		// Save the world
		Jo::Files::HDDFile file(m_worldFileName, Jo::Files::HDDFile::OVERWRITE);
		g_Game->GetWorld()->Save( file );
	}


	void MasterState::Resize(const sf::Vector2f& _size)
	{
		// Scale chat too
		CommonState::Resize( _size );

		// Scale editor sidebar
		m_objectsPanel->setSize( m_objectsPanel->getSize().x, _size.y * 0.5f );
		m_modulePanel->setSize( m_modulePanel->getSize().x, _size.y * 0.5f );
		m_modulePanel->setPosition( 0.0f, _size.y * 0.5f );
		m_propertyPanel->setSize( m_propertyPanel->getSize().x, _size.y * 0.5f );
		m_viewPanel->setSize( m_viewPanel->getSize().x, _size.y * 0.5f );
		m_viewPanel->setPosition( m_modulePanel->getSize().x, _size.y * 0.5f );

		tgui::ChatBox::Ptr localOut = m_gui.get( "Messages" );
		m_selectionView->setSize( m_selectionView->getSize().x, localOut->getPosition().y - 100.0f );
		m_selectionView->setPosition( _size.x - m_selectionView->getSize().x, 100.0f );

		m_toolbar->setSize( _size.x - m_objectsPanel->getSize().x*2, 100.0f );
	}


	void MasterState::MouseMoved(int deltaX, int deltaY, bool guiHandled)
	{
		CommonState::MouseMoved(deltaX, deltaY, guiHandled);

		// If not in action mode, reset the default action
		if (m_modeTool->GetMode() != Interfaces::ModeToolbox::ACTION)
			Actions::ActionPool::Instance().UpdateDefaultAction(m_selection, 0);

		// Don't react to any key if gui handled it
		if (guiHandled)
			return;

		if( sf::Mouse::isButtonPressed(sf::Mouse::Left) )
		{
			// Most actions here require the tile position -> compute it
			sf::Vector2i mousePos = Events::InputHandler::GetMouseTilePosition();

			// In brush mode paint to the current position
			if( m_modeTool->GetMode() == Interfaces::ModeToolbox::BRUSH )
			{
				m_brush.Paint(mousePos.x, mousePos.y);
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
		{
			// Get the render window
			sf::RenderWindow& win = g_Game->GetWindow();

			// Create a new view with its center shifted by the mouse position
			sf::View& newView = GetStateView();
			sf::Vector2f center = newView.getCenter();
			sf::Vector2f scale(newView.getSize().x / win.getSize().x,
				newView.getSize().y / win.getSize().y);
			newView.setCenter(center.x - (deltaX * scale.x),
				center.y - (deltaY * scale.y));

			// Apply view to the window
			SetStateView();
		}
	}

	Core::Map* MasterState::GetCurrentMap()
	{
		MapID id = m_mapTool->GetSelectedMap();
		return g_Game->GetWorld()->GetMap(id);
	}

	void MasterState::GoTo( const Core::Object* _object )
	{
		if( !_object ) return;

		if( _object->IsLocatedOnAMap() )
		{
			CommonState::GoTo(_object);

			// Reset selection
			m_selection.Clear();
			AddToSelection(_object->ID());

			// Reset current map
			m_mapTool->SetMap(_object->GetParentMap());
		} else
			g_Game->AppendToChatLog( Network::ChatMsg( STR_NO_POSITION, sf::Color::Red) );
	}

	void MasterState::CreateCombat( Core::ObjectID _object )
	{
		// Maybe create a new Combat object
		if (!m_combat)
			m_combat = new GameRules::MasterCombat();

		// Show the combatant display
		m_combatantPanel->show();

		if (m_combat->HasParticipant(_object))
			return;

		// Find the object
		Core::Object* object = g_Game->GetWorld()->GetObject(_object);
		// Stop objects that are currently moving!
		// TODO: Only this objects or more?
		object->ResetTarget();

		// Prompt for initiative roll
		static_cast<GameRules::MasterCombat*>(m_combat)->AddParticipant(_object);
	}

	bool MasterState::OwnsObject( Core::ObjectID _object )
	{
		// Find the object
		Core::Object* object = g_Game->GetWorld()->GetObject(_object);

		// True only if the object is found and has no owner at all.
		return object != 0
			&& object->HasProperty(STR_PROP_OWNER) == false;
	}

	void MasterState::CreateCombatPrompt()
	{	
		if (m_selection.Size() > 0)
		{
			States::PromptState* prompt = static_cast<States::PromptState*>(g_Game->GetStateMachine()->PushGameState(GST_PROMPT));
			prompt->SetTextInputRequired(false);
			prompt->SetText("Alle aktuell ausgewählten Objekte nehmen am Kampf teil. Weiter?\n");
			prompt->AddButton("Ja", std::bind(&MasterState::CreateCombatFromSelection, this), sf::Keyboard::Return);
			prompt->AddButton("Nein", [](std::string) -> void {}, sf::Keyboard::Escape);
			prompt->DisableMinimize();
		}
		else
		{
			// Selection is empty, so this will just create a combat object
			CreateCombatFromSelection();
		}
	}

	void MasterState::CreateCombatFromSelection()
	{
		if (!m_combat)
			m_combat = new GameRules::MasterCombat;

		// Add all combattants
		for (int i = 0; i<m_selection.Size(); ++i)
			CreateCombat(m_selection[i]);
	}

	void MasterState::CreateDiceRollState(){
		CommonState::CreateDiceRollState();
		m_diceRollState->AddButton("Verdeckt würfeln",std::bind(&MasterState::RollSecretly,this,std::placeholders::_1));
		m_diceRollState->AddButton("Offen würfeln",std::bind(&MasterState::RollOpen,this,std::placeholders::_1));
	}

	void MasterState::RollSecretly(std::string& result){
		Core::Object* _object = nullptr; //by default roll without object
		if(m_selection.Size()>0) //if object is selected
			_object=g_Game->GetWorld()->GetObject(m_selection[0]); //roll over object
		if(m_diceRollState->CheckEvaluate(_object)) // if everything makes sense
		{
			int x=Utils::EvaluateFormula(result,g_Game->RANDOM,_object);
			g_Game->AppendToChatLog(Network::ChatMsg("[Master] - Würfelwurf: "+result+" = "+std::to_string(x),sf::Color::White)); //show it to the master

		}
	}

	void MasterState::RollOpen(std::string& result){
		Core::Object* _object = nullptr; //by default roll without object
		if(m_selection.Size()>0) //if object is selected
			_object=g_Game->GetWorld()->GetObject(m_selection[0]); //roll over object
		if(m_diceRollState->CheckEvaluate(_object)) // if everything makes sense
		{
			int x=Utils::EvaluateFormula(result,g_Game->RANDOM,_object);
			Network::ChatMsg msg("[Master] - Würfelwurf: "+result+" = "+std::to_string(x),sf::Color::White);
			msg.Send();		
		}
	}
}// namespace States
