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

using namespace Core;

namespace States {

	MasterState::MasterState( const std::string& _loadFile ) :
		m_propertyPanel(nullptr),
		m_modulePanel(nullptr),
		m_objectsPanel(nullptr),
		m_selectionView(nullptr),
		m_toolbar(nullptr),
		m_hiddenLayers(10, 0),
		m_worldFileName(_loadFile),
		m_rectSelection(false),
		m_draggedContent(nullptr)
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
		m_toolbar->AddToolbox( Interfaces::NPCToolbox::Ptr() );

		// Set chat color...
		m_color = sf::Color(80,80,250);
		m_name = "Master";

		// Set window size
		sf::Vector2u size = g_Game->GetWindow().getSize();
		Resize( sf::Vector2f( (float) size.x, (float) size.y ) );
	}

	MasterState::~MasterState()
	{
		delete m_draggedContent;
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

		// Show the brush region
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
		// Register right-click on Player-Toolbox ( tgui has no RMB support ).
		if( button.button == sf::Mouse::Right )
		{
			float x = button.x - m_toolbar->getPosition().x;
			float y = button.y - m_toolbar->getPosition().y;
			if( m_playerTool->mouseOnWidget(x, y) )
			{
				// Set view to the player
				Object* player = m_playerTool->GetPlayer(x, y);
				sf::Vector2f pos = player->GetPosition();
				sf::Vector2f viewPos = pos * float(TILESIZE);
				sf::View newView = g_Game->GetWindow().getView();
				newView.setCenter(viewPos);
				g_Game->GetWindow().setView(newView);
			}
		}

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
				if( IsLayerVisible( m_modeTool->Brush()->GetLayer() ) ) {
					Network::ChatMsg(STR_MSG_LAYER_INVISIBLE, sf::Color::Red).Send();
					return;
				}
				// Paint new objects with the brush.
				m_brush.BeginPaint( *GetCurrentMap(),
					m_objectsPanel->GetSelected(),
					m_modeTool->Brush()->GetDiameter(),
					tileX, tileY,
					m_modeTool->Brush()->GetLayer(),
					m_modeTool->Brush()->GetMode() );
			}
			if( m_modeTool->GetMode() == Interfaces::ModeToolbox::SELECTION )
			{
				// Start rect-selection
				m_rectSelectionStart.x = tileX;
				m_rectSelectionStart.y = tileY;
				m_rectSelection = true;
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

						// Meaningful layer: on top
						int l = GetCurrentMap()->GetObjectsAt(x,y).Size();
						GetCurrentMap()->Add( id, x, y, l );
					} else if( m_draggedContent->from == Interfaces::DragContent::PLAYERS_LIST )
					{
						// Insert original to the map (player layer)
						Object* object = const_cast<Object*>(m_draggedContent->object);
						if( !object->IsLocatedOnAMap() )
							GetCurrentMap()->Add( object->ID(), x, y, 5 );
						else {
							GetCurrentMap()->SetObjectPosition( object, tilePos );
						}
						object->ResetTarget();
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
			// The cursor did not leave the tile -> interpret asa click only it time was also short
			if( tile.x==m_rectSelectionStart.x && tile.y==m_rectSelectionStart.y && time < 0.4f)
			{
				// Take only the topmost visible tile
				auto& objectList = GetCurrentMap()->GetObjectsAt((int)tilePos.x, (int)tilePos.y);
				ObjectID topmostObject = 0xffffffff;
				int maxLayer = -1000;
				for (int i = objectList.Size()-1; i >= 0; --i)
				{
					// If object is not on hidden layer it is worth a closer look
					int layer = g_Game->GetWorld()->GetObject(objectList[i])->GetLayer();
					if( !m_hiddenLayers[layer] && layer > maxLayer )
					{
						topmostObject = objectList[i];
						maxLayer = layer;
					}
				}
				if( topmostObject != 0xffffffff )
					AddToSelection(topmostObject);
			} else {
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
							if(!m_hiddenLayers[g_Game->GetWorld()->GetObject(objectList[i])->GetLayer()])
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

		case sf::Keyboard::C:
			// If only one object is selected show the character screen. Even
			// if it has no character properties. GM should know better...
			if( m_selection.Size() == 1 )
			g_Game->GetStateMachine()->PushGameState(new CharacterState(&m_selection[0]));
			break;

			// DEBUGGING COMBAT, TODO: remove
		case sf::Keyboard::Space:
			if (sf::Keyboard::isKeyPressed((sf::Keyboard::LControl)))
			{
				m_combat = new GameRules::MasterCombat();
                Core::World* world = g_Game->GetWorld();
                for( int i=0; i<m_selection.Size(); ++i )
                {
                    // Add participant to combat
                    static_cast<GameRules::MasterCombat*>(m_combat)->AddParticipant(m_selection[i]);
                }
			}
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
	}

	Core::Map* MasterState::GetCurrentMap()
	{
		MapID id = m_mapTool->GetSelectedMap();
		return g_Game->GetWorld()->GetMap(id);
	}

    void MasterState::BeginCombat( Core::ObjectID _object )
    {
        // Find the object
        Core::Object* object = g_Game->GetWorld()->GetObject(_object);

        // Does this object belong to me?
        if (!object->HasProperty(STR_PROP_OWNER))
        {
            // Maybe create a new Combat object
            if (!m_combat)
                m_combat = new GameRules::MasterCombat();

            // Prompt for initiative roll
            m_combat->PushInitiativePrompt(_object);
        }
    }

	//void States::MasterState::TestButtonCallback(std::string feedback){
	//	std::cout<<feedback;
	//}

}// namespace States
