#include "states/SelectionState.hpp"
#include "states/CommonState.hpp"
#include "Game.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"
#include "Constants.hpp"
#include "core/ObjectList.hpp"
#include "utils/Content.hpp"
#include "core/Object.hpp"
#include <math.h>
#include "StateMachine.hpp"
#include "ActionState.hpp"
#include "Constants.hpp"
#include "core/PredefinedProperties.hpp"
#include "PlayerState.hpp"

States::SelectionState::SelectionState() :
	m_menu(m_gui)
{
	m_gui.setWindow(g_Game->GetWindow());
	m_gui.setGlobalFont( Content::Instance()->LoadFont("media/arial.ttf") );
	m_dirty = false;
	m_hiddenLayers=nullptr;

	// Finally, use SetGui() to activate the GUI (rendering, events, callbacks)
	SetGui(&m_gui);
}

void States::SelectionState::OnBegin()
{
	m_controlWasPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl);
}

void States::SelectionState::SetTilePosition(int x, int y,const bool* hiddenLayers)
{
	CommonState* previousState = dynamic_cast<CommonState*>(m_previousState);
	m_objects = previousState->GetCurrentMap()->GetObjectsAt(x,y);
	sf::Vector2i pos = g_Game->GetWindow().mapCoordsToPixel(sf::Vector2f((x + 0.5f) * TILESIZE, (y + 0.5f) * TILESIZE));
	m_screenX = pos.x;
	m_screenY = pos.y;
	m_hiddenLayers = hiddenLayers;

	m_dirty = true;
}

void States::SelectionState::RecalculateGUI()
{
	std::vector<Interfaces::CircularMenu::ObjInfo> items;

	// Get current selection to make buttons transparent when they not selected
	CommonState* previousState = dynamic_cast<CommonState*>(m_previousState);
	const Core::ObjectList* alreadySelected = previousState->GetSelection();

	for( int i = 0; i < m_objects.Size(); ++i )
	{
		Core::Object* object = g_Game->GetWorld()->GetObject(m_objects[i]);
		if(m_hiddenLayers) // if hidden layers have been set
			if(m_hiddenLayers[atoi(object->GetProperty(Core::PROPERTY::LAYER.Name()).Value().c_str())-1]) // if object is on hidden layer
				continue;

		items.push_back( Interfaces::CircularMenu::ObjInfo( m_objects[i],
			alreadySelected->Contains(m_objects[i]),
			object->GetName() ) );
	}

	m_menu.Show( sf::Vector2f((float)m_screenX, (float)m_screenY), items );

	m_dirty = false;
}

void States::SelectionState::Update(float dt)
{
	if(m_dirty)
		RecalculateGUI();
	m_previousState->Update(dt);
	if(m_controlWasPressed && !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
		m_finished=true;
	m_controlWasPressed |= sf::Keyboard::isKeyPressed(sf::Keyboard::LControl);
}


void States::SelectionState::Draw(sf::RenderWindow& win)
{
	m_previousState->Draw(win);

	GameState::Draw(win);
}


void States::SelectionState::GuiCallback(tgui::Callback& args)
{
	//* NORMAL SELECTION BEHAVIOR
	if( dynamic_cast<PlayerState*>(m_previousState) )
	{
		// For player also map the left click on the action menu
		sf::Vector2i mousePos = sf::Mouse::getPosition(g_Game->GetWindow());
		ShowActionState( args.id, mousePos.x, mousePos.y );
	} else {
		CommonState* previousState = dynamic_cast<CommonState*>(m_previousState);
		// The parent is not set or not of type CommonState, but it should be!
		assert(previousState);

		// If control is not pressed clear selection
		if( !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && !sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) )
		{
			previousState->ClearSelection();
		}

		const Core::ObjectList* alreadySelected = previousState->GetSelection();
	
		if( alreadySelected->Contains(args.id) )
		{
			// already selected
			previousState->RemoveFromSelection(args.id);
		} else {
			previousState->AddToSelection(args.id);
		}
		m_dirty = true;
		if( !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) )
			m_finished = true;
	}
	// 	*/
}



void States::SelectionState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, bool guiHandled)
{
	// Return if the GUI already handled it
	if (guiHandled)
		return;

	CommonState* previousState = dynamic_cast<CommonState*>(m_previousState);
	// The parent is not set or not of type CommonState, but it should be!
	assert(previousState);

	if (button.button == sf::Mouse::Button::Right)
	{
		if(previousState->GetSelection()->Size() > 0)
		{
			Core::ObjectID id = m_menu.GetClickedItem((float)button.x, (float)button.y);
			if( id != Core::INVALID_ID )
			{
				ShowActionState( id, button.x, button.y );
				return;
			}
		}

		// Reset menu position
		SetTilePosition((int)floor(tilePos.x), (int)floor(tilePos.y));
	}
	else if(button.button == sf::Mouse::Left)
	{
		// If clicked somewhere not on the gui finish in single selection mode
		if ( !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) )
			m_finished = true;
		else
			SetTilePosition((int)floor(tilePos.x), (int)floor(tilePos.y));
	}
}


void States::SelectionState::ShowActionState(Core::ObjectID _targetObject, int _x, int _y)
{
	auto action = dynamic_cast<ActionState*>( g_Game->GetStateMachine()->PushGameState(States::GST_ACTION) );
	action->SetTargetObject(_targetObject);
	const Core::ObjectList* selection = dynamic_cast<CommonState*>(m_previousState)->GetSelection();
	//TODO: Forschleife mit mehreren aktionen für alle objekte in der selektion
	action->SetSourceObject((*selection)[0]);
	action->SetPosition(_x, _y);
	m_finished = true;
}