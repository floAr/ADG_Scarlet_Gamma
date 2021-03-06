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
#include "actions/ActionPool.hpp"
#include <functional>
#include "utils/Clipboard.hpp"
#include "utils/ValueInterpreter.hpp"
#include "utils/Exception.hpp"

using namespace Core;

namespace States {

	MasterState::MasterState( const std::string& _loadFile ) :
		m_propertyPanel(nullptr),
		m_modulePanel(nullptr),
		m_objectsPanel(nullptr),
		m_selectionView(nullptr),
		m_toolbar(nullptr),
		m_worldFileName(_loadFile),
		m_rectSelection(false),
		m_lastObjectPanelUpdate(0.0f)
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

		// Create hint text
		m_saveHintTime = -1;
		m_hintText.setFont(Content::Instance()->LoadFont("media/arial.ttf"));
		m_hintText.setCharacterSize(20);
		m_hintText.setStyle(sf::Text::Bold);

		// Set window size
		sf::Vector2u size = g_Game->GetWindow().getSize();
		Resize( sf::Vector2f( (float) size.x, (float) size.y ) );
	}


	void MasterState::Update(float dt)
	{
		CommonState::Update(dt);

		// Move with mouse cursors
		float dirX = 0, dirY = 0;
		if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Left) )
			dirX -= 1.0f;
		if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Right) )
			dirX += 1.0f;
		if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Up) )
			dirY -= 1.0f;
		if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Down) )
			dirY += 1.0f;
		if (dirX != 0 || dirY != 0)
		{
			float speed = 150.0f;
			sf::Vector2f scale(m_stateView.getSize().x / g_Game->GetWindow().getSize().x,
                               m_stateView.getSize().y / g_Game->GetWindow().getSize().y);
			m_stateView.move( sf::Vector2f(dirX * speed * dt * scale.x, dirY * speed * dt * scale.y) );
			SetStateView();
		}

		// Update the game world
		g_Game->GetWorld()->Update(dt);

		// Update the toolbar
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

		// Update saved hint time if necessary
		if (m_saveHintTime > 0)
			m_saveHintTime -= dt;

		// Update the viewed properties
		if( m_selectionView->isVisible() )
			m_selectionView->Show( g_Game->GetWorld(), m_selection );
		m_viewPanel->RefreshFilter();

		// Update the object views seldom (they always reload all pictures)
		m_lastObjectPanelUpdate += dt;
		if( m_lastObjectPanelUpdate > 1.5f )
		{
			m_modulePanel->RefreshFilter();
			m_objectsPanel->RefreshFilter();
			m_lastObjectPanelUpdate = 0.0f;
		}
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

		// Hint texts
		if (m_saveHintTime > 0)
		{
			m_hintText.setString("GESPEICHERT");
			Resize(sf::Vector2f((float) win.getSize().x, (float) win.getSize().y));
			SetGuiView();
			// Faking dark border
			m_hintText.setColor(sf::Color(0, 0, 0, 100));
			m_hintText.move(-1, -1); win.draw(m_hintText);
			m_hintText.move( 2,  0); win.draw(m_hintText);
			m_hintText.move( 0,  2); win.draw(m_hintText);
			m_hintText.move(-2,  0); win.draw(m_hintText);
			m_hintText.setColor(sf::Color(255, 255, 255, 100));
			m_hintText.move( 1, -1); win.draw(m_hintText);
			SetStateView();
		}
		else
		{
			// Draw pause text if game is paused
			if (g_Game->GetWorld()->IsPaused())
			{
				m_hintText.setString("PAUSE");
				Resize(sf::Vector2f((float) win.getSize().x, (float) win.getSize().y));
				SetGuiView();
				// Faking dark border
				m_hintText.setColor(sf::Color(0, 0, 0, 100));
				m_hintText.move(-1, -1); win.draw(m_hintText);
				m_hintText.move( 2,  0); win.draw(m_hintText);
				m_hintText.move( 0,  2); win.draw(m_hintText);
				m_hintText.move(-2,  0); win.draw(m_hintText);
				m_hintText.setColor(sf::Color(255, 255, 255, 100));
				m_hintText.move( 1, -1); win.draw(m_hintText);
				SetStateView();
			}
		}

		CommonState::Draw(win);
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
						// If the object is part of the selection append the whole selection
						if( m_selection.Contains( topmostObject ) )
							m_draggedContent->manyObjects = &m_selection;
					}
				}
				break; }
		case sf::Mouse::Right: {
			if( GetCurrentMap() != nullptr && GetCurrentMap()->GetObjectsAt(tileX, tileY).Size() > 0 )
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
		{
			delete m_draggedContent;
			m_draggedContent = nullptr;
			return;
		}

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
							GetCurrentMap()->SetObjectPosition( object, sf::Vector2f((float)x,(float)y) );
						}
						object->ResetTarget();
					} else if( m_draggedContent->from == Interfaces::DragContent::PROPERTY_PANEL )
					{
						if( m_draggedContent->object && m_draggedContent->object->HasProperty( STR_PROP_ITEM ) )
						{
							// Take away from the source object
							m_draggedContent->prop->RemoveObject(m_draggedContent->object);
							// Insert into map
							GetCurrentMap()->Add( m_draggedContent->object->ID(), x, y, AutoDetectLayer(m_draggedContent->object) );
						}
					} else if( m_draggedContent->from == Interfaces::DragContent::MAP )
					{
						if( m_draggedContent->manyObjects )
						{
							// Determine move-vector form main object
							sf::Vector2f dir = sf::Vector2f((float)x,(float)y) - m_draggedContent->object->GetPosition();
							for( int i = 0; i < m_draggedContent->manyObjects->Size(); ++i )
							{
								Core::Object* object = g_Game->GetWorld()->GetObject( (*m_draggedContent->manyObjects)[i] );
								GetCurrentMap()->SetObjectPosition( object, object->GetPosition() + dir );
								object->ResetTarget();
							}
						} else {
							// Insert into map
							GetCurrentMap()->SetObjectPosition( m_draggedContent->object, sf::Vector2f((float)x,(float)y) );
							m_draggedContent->object->ResetTarget();
						}
					}
				}
			}

			delete m_draggedContent;
			m_draggedContent = nullptr;
		}

		if(m_rectSelection)
		{
			// If not pressing cntrl -> clear selection
			if(!Events::InputHandler::IsControlPressed())
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

		m_zoom = (float)-wheel.delta;
	}


	void MasterState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
	{
		// Handle copy & paste
		GameState::KeyPressed(key, guiHandled);

		// This should work ALWAYS, even if GUI is focused:
		if( key.code >= sf::Keyboard::Num1 && key.code <= sf::Keyboard::Num9 )
		{
			if(Events::InputHandler::IsControlPressed()) {
				if( m_selection.Size() == 1 )
					SetHotkeyToObject(key.code - sf::Keyboard::Num1, m_selection[0]);
			} else
				GoTo( GetObjectFromHotkey(key.code - sf::Keyboard::Num1) );
		}

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

		case sf::Keyboard::D:
			// Duplicate the current template
			if( Events::InputHandler::IsControlPressed() )
			{
				g_Game->GetWorld()->NewObjectTemplate( m_objectsPanel->GetSelected()->ID() );
			}
			break;

			//case sf::Keyboard::T:
			//	PromptState* gs = dynamic_cast<PromptState*>(g_Game->GetStateMachine()->PushGameState(GST_PROMPT));
			//	gs->ConfigurePromp("Das ist dein toller Text",false);
			//	gs->AddButton("testbutton",std::bind(&MasterState::TestButtonCallback,this,std::placeholders::_1),sf::Vector2f(212,460));
			//	break;
		}

		// Return if the GUI already handled it
		// Everything in that damned gui eats the focus - only edits are using it
		tgui::EditBox* edit = dynamic_cast<tgui::EditBox*>(GameState::GetFocusedElement().get());
		guiHandled &= edit != nullptr;

		// Let common state handle input
		CommonState::KeyPressed(key, guiHandled);

		if (guiHandled)
			return;

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
				// Do not simply remove objects with the player attribute!
				if ( g_Game->GetWorld()->GetObject(id)->HasProperty(STR_PROP_PLAYER) )
				{
					if( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::LShift ) || sf::Keyboard::isKeyPressed( sf::Keyboard::Key::RShift ) )
					{
						States::PromptState* prompt = static_cast<States::PromptState*>(g_Game->GetStateMachine()->PushGameState(States::GST_PROMPT));
						prompt->SetTextInputRequired(false);
						prompt->SetText(STR_PLAYER + ' ' + g_Game->GetWorld()->GetObject(id)->GetName() + STR_MSG_DELETE_PLAYER);
						prompt->DisableMinimize();
						prompt->AddButton(STR_CANCEL, [](const std::string&) {}, sf::Keyboard::Escape);
						prompt->AddButton(STR_OK, [id](const std::string&) {
							g_Game->GetWorld()->RemoveObject( id );
						}, sf::Keyboard::Return);
					}
				} else
					g_Game->GetWorld()->RemoveObject( id );	// Assumes real deletion otherwise
			}
			m_selection.Clear();
			m_selectionView->hide();
			break;
		case sf::Keyboard::Pause:
		case sf::Keyboard::P:
			g_Game->GetWorld()->SetPause( !g_Game->GetWorld()->IsPaused() );
			break;
		case sf::Keyboard::F1:
			m_modeTool->SetMode( Interfaces::ModeToolbox::SELECTION );
			break;
		case sf::Keyboard::F2:
			m_modeTool->SetMode( Interfaces::ModeToolbox::BRUSH );
			break;
		case sf::Keyboard::F3:
			m_modeTool->SetMode( Interfaces::ModeToolbox::ACTION );
			break;
		case sf::Keyboard::F4:
			m_modeTool->SetMode( Interfaces::ModeToolbox::DRAGNDROP );
			break;
		case sf::Keyboard::S:
			if( Events::InputHandler::IsControlPressed() )
			{
				// Save the world
				m_saveHintTime = 2.f;
				Jo::Files::HDDFile file(m_worldFileName, Jo::Files::HDDFile::OVERWRITE);
				g_Game->GetWorld()->Save( file );
				g_Game->AppendToChatLog( Network::ChatMsg("Welt gespeichert", sf::Color(255, 255, 200, 255) ) );
			}
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

		tgui::ChatBox::Ptr localOut = m_gui.get( "Messages" );
		m_selectionView->setSize( m_selectionView->getSize().x, localOut->getPosition().y - 100.0f );
		m_selectionView->setPosition( _size.x - m_selectionView->getSize().x, 100.0f );
		m_toolbar->setSize( _size.x - m_objectsPanel->getSize().x*2, 100.0f );

		// Resize combatant liste, provide width of toolbar
		m_combatantPanel->Resize(_size, m_propertyPanel->getSize().x + m_viewPanel->getSize().x);

		// Scale editor sidebar
		float height = InCombat() ? _size.y - m_combatantPanel->getSize().y : _size.y;
		m_objectsPanel->setSize( m_objectsPanel->getSize().x, height * 0.5f );
		m_modulePanel->setSize( m_modulePanel->getSize().x, height * 0.5f );
		m_modulePanel->setPosition( 0.0f, height * 0.5f );
		m_propertyPanel->setSize( m_propertyPanel->getSize().x, height * 0.5f );
		m_viewPanel->setSize( m_viewPanel->getSize().x, height * 0.5f );
		m_viewPanel->setPosition( m_modulePanel->getSize().x, height * 0.5f );

		// Reposition pause text
		m_hintText.setPosition(_size.x / 2.0f - m_hintText.getGlobalBounds().width / 2.0f,
								_size.y / 2.0f - m_hintText.getGlobalBounds().height / 2.0f);
	}


	void MasterState::MouseMoved(int deltaX, int deltaY, bool guiHandled)
	{
		CommonState::MouseMoved(deltaX, deltaY, guiHandled);

		// If not in action mode, reset the default action
		if (m_modeTool->GetMode() != Interfaces::ModeToolbox::ACTION)
			Actions::ActionPool::Instance().UpdateDefaultAction(m_selection, 0);

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

        // Don't react to any key if gui handled it
        if (guiHandled)
            return;

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
		Resize(sf::Vector2f((float) g_Game->GetWindow().getSize().x,
							(float) g_Game->GetWindow().getSize().y));

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
			prompt->SetText("Die aktuell ausgew�hlten " + std::to_string(m_selection.Size()) +
				" Objekte nehmen am Kampf teil. Weiter?\n");
			prompt->DisableMinimize();
			prompt->AddButton("Ja", std::bind(&MasterState::CreateCombatFromSelection, this), sf::Keyboard::Return);
			prompt->AddButton("Nein", [](const std::string&) -> void {}, sf::Keyboard::Escape);
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

		// Resize to show combat panel
		m_combatantPanel->show();
		Resize(sf::Vector2f((float) g_Game->GetWindow().getSize().x,
							(float) g_Game->GetWindow().getSize().y));

		// Add all combattants
		for (int i = 0; i<m_selection.Size(); ++i)
			CreateCombat(m_selection[i]);
	}

	void MasterState::CreateDiceRollState()
	{
		CommonState::CreateDiceRollState();
		m_diceRollState->AddButton("Verdeckt w�rfeln", std::bind(&MasterState::RollSecretly, this, std::placeholders::_1));
		m_diceRollState->AddButton("Offen w�rfeln", std::bind(&MasterState::RollOpen, this, std::placeholders::_1));
		m_diceRollState->AddButton("Abbrechen", [](const std::string&)->void{}, sf::Keyboard::Escape);
	}

	void MasterState::RollSecretly(const std::string& _result)
	{
		try {
			if( m_selection.Size() == 0 )
			{
				int x = Utils::EvaluateFormula(_result, g_Game->RANDOM, nullptr);
				g_Game->AppendToChatLog(Network::ChatMsg("[Master] - W�rfelwurf: "+_result+" = "+std::to_string(x),m_color));
			}
			for( int i = 0; i < m_selection.Size(); ++i )
			{
				// Once for each object
				Core::Object* object = g_Game->GetWorld()->GetObject(m_selection[0]);
				int x = Utils::EvaluateFormula(_result, g_Game->RANDOM, object);
				// Master output only
				g_Game->AppendToChatLog(Network::ChatMsg("["+object->GetName()+"] - W�rfelwurf: "+_result+" = "+std::to_string(x),m_color));
			}
		} catch( Exception::InvalidFormula _e ) {
			// If one object fails the others are likely to do the same
			g_Game->AppendToChatLog(Network::ChatMsg(_e.to_string(), sf::Color::Red));
		}
	}

	void MasterState::RollOpen(const std::string& _result)
	{
		try {
			if( m_selection.Size() == 0 )
			{
				int x = Utils::EvaluateFormula(_result, g_Game->RANDOM, nullptr);
				Network::ChatMsg("[Master] - W�rfelwurf: "+_result+" = "+std::to_string(x), m_color).Send();
			}
			for( int i = 0; i < m_selection.Size(); ++i )
			{
				// Once for each object
				Core::Object* object = g_Game->GetWorld()->GetObject(m_selection[0]);
				int x = Utils::EvaluateFormula(_result, g_Game->RANDOM, object);
				// Output for all
				Network::ChatMsg("["+object->GetName()+"] - W�rfelwurf: "+_result+" = "+std::to_string(x), m_color).Send();
			}
		} catch( Exception::InvalidFormula _e ) {
			// If one object fails the others are likely to do the same
			g_Game->AppendToChatLog(Network::ChatMsg(_e.to_string(), sf::Color::Red));
		}
	}
}// namespace States
