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

States::SelectionState::SelectionState() :
	m_defaultButton()
{
	m_gui.setWindow(g_Game->GetWindow());
	m_gui.setGlobalFont( Content::Instance()->LoadFont("media/arial.ttf") );
	m_dirty = false;

	m_defaultButton->load("lib/TGUI-0.6-RC/widgets/Black.conf");
}

void States::SelectionState::OnBegin()
{
	m_controlWasPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl);

	// If control is not hold clear old selection
	if( !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) )
	{
		CommonState* previousState = dynamic_cast<CommonState*>(m_previousState);
		previousState->ClearSelection();
	}
}

void States::SelectionState::SetTilePosition(int x, int y, float _screenX, float _screenY)
{
	m_objects = g_Game->GetWorld()->GetMap(0)->GetObjectsAt(x,y);
	m_screenX = _screenX;
	m_screenY = _screenY;
	m_dirty = true;
}

void States::SelectionState::RecalculateGUI()
{
	m_gui.removeAllWidgets(); //clear gui

	// Get current selection to make buttons transparent when they not selected
	CommonState* previousState = dynamic_cast<CommonState*>(m_previousState);
	const Core::ObjectList* alreadySelected = previousState->GetSelection();

	int count = m_objects.Size();

	int i;
	for(i=0;i<count;i++){
		Core::Object* o=g_Game->GetWorld()->GetObject(m_objects[i]);
		tgui::Button::Ptr button = m_defaultButton.clone();
		m_gui.add(button);
		button->setSize(50, 40);
		positionButton(button, 360.0f / count * i, 45.0f + count * 3.0f);
		if(o->HasProperty(Core::Object::PROP_NAME))
			button->setText(o->GetProperty(Core::Object::PROP_NAME).Value());
		else
			button->setText(std::to_string(o->ID()));
		if( alreadySelected->Contains(m_objects[i]) ) {
			// already selected
			button->setTransparency(255);
		} else {
			button->setTransparency(150);
		}

		button->setCallbackId(100+i);
		button->bindCallback(tgui::Button::LeftMouseClicked);
	}

	// Finally, use SetGui() to activate the GUI (rendering, events, callbacks)
	SetGui(&m_gui);
	m_dirty=false;
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
	if(args.id>=100)//item clicked
	{
		/* ACTION SELECTION BEHAVIOUR
		Core::ObjectID id = (*m_objects)[args.id-100];
		auto action=dynamic_cast<ActionState*>( g_Game->GetStateMachine()->PushGameState(States::GST_ACTION));
		action->SetObject(id);
		m_finished=true;
		// */
		//* NORMAL SELECTION BEHAVIOR
		CommonState* previousState = dynamic_cast<CommonState*>(m_previousState);
		// The parent is not set or not of type CommonState, but it should be!
		assert(previousState);
		const Core::ObjectList* alreadySelected = previousState->GetSelection();

		Core::ObjectID id = m_objects[args.id-100];
		if( alreadySelected->Contains(id) )
		{
			// already selected
			previousState->RemoveFromSelection(id);
		} else {
			previousState->AddToSelection(id);
		}
		m_dirty=true;
		if( !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) )
			m_finished = true;
		// 	*/
	}
}



void States::SelectionState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos)
{
	if(button.button == sf::Mouse::Button::Right)
	{
		// Reset menu position
		SetTilePosition((int)floor(tilePos.x), (int)floor(tilePos.y), (float)button.x, (float)button.y);

	} else if(button.button == sf::Mouse::Left)
	{
		// If clicked somewhere not on the gui finish in single selection mode
		if( !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) )
			m_finished = true;
		else
			SetTilePosition((int)floor(tilePos.x), (int)floor(tilePos.y), (float)button.x, (float)button.y);
	}
}


void  States::SelectionState::positionButton(tgui::Button::Ptr b, float angle, float radius)
{
	float bx = m_screenX - b->getSize().x*0.5f + radius * sin(angle*0.01745329251f); //0.01745329251 is to got radians from degrees
	float by = m_screenY - b->getSize().y*0.5f + radius * cos(angle*0.01745329251f);
	b->setPosition(bx, by);
}