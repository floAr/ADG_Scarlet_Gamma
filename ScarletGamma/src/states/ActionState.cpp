#include "states/ActionState.hpp"
#include "states/CommonState.hpp"
#include "Game.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"
#include "Constants.hpp"
#include "core/ObjectList.hpp"
#include "utils/Content.hpp"
#include "core/Object.hpp"
#include <math.h>
#include "../actions/ActionPool.hpp"
#include <vector>

States::ActionState::ActionState() :
	m_defaultButton()
{
	m_object=-1;
	m_gui.setWindow(g_Game->GetWindow());
	m_gui.setGlobalFont( Content::Instance()->LoadFont("media/arial.ttf") );
	m_dirty = false;

	m_defaultButton->load("lib/TGUI-0.6-RC/widgets/Black.conf");
}

void States::ActionState::SetObject(Core::ObjectID id){
	m_object=id;
	m_dirty=true;
}
void States::ActionState::OnBegin()
{
}


void States::ActionState::RecalculateGUI()
{
	m_gui.removeAllWidgets(); //clear gui
	//get the current object
	Core::Object* o=g_Game->GetWorld()->GetObject(m_object);
	auto pos=o->GetPosition();
	m_screenX=pos.x*TILESIZE;
	m_screenY=pos.y*TILESIZE;
	//get action list
	std::vector<Core::ActionID> actions=Actions::ActionPool::Instance().GetAllowedActions(*o);

	int i=0;
	int count =actions.size();
	//for each action 
	for(std::vector<Core::ActionID>::iterator it = actions.begin(); it != actions.end(); ++it) {
		tgui::Button::Ptr button = m_defaultButton.clone();
		m_gui.add(button);
		button->setSize(50, 40);
		//button->setPosition(100,100);
		positionButton(button, 360.01f / count * i, 45.0f + count * 3.0f);
		button->setText(Actions::ActionPool::Instance().GetActionName((*it)));

		button->setCallbackId(100+(*it));
		button->bindCallback(tgui::Button::LeftMouseClicked);
		i=i+1;
	}

	// Finally, use SetGui() to activate the GUI (rendering, events, callbacks)
	SetGui(&m_gui);
	m_dirty=false;
}

void States::ActionState::Update(float dt)
{
	if(m_dirty)
		RecalculateGUI();
	m_previousState->Update(dt);
}


void States::ActionState::Draw(sf::RenderWindow& win)
{
	m_previousState->Draw(win);

	GameState::Draw(win);

}


void States::ActionState::GuiCallback(tgui::Callback& args)
{
	if(args.id>=100)//item clicked
	{
		Actions::ActionPool::Instance().StartAction(args.id-100);
		m_finished=true;
	}
}





void  States::ActionState::positionButton(tgui::Button::Ptr b, float angle, float radius)
{
	float bx = m_screenX - b->getSize().x*0.5f + radius * sin(angle*0.01745329251f); //0.01745329251 is to got radians from degrees
	float by = m_screenY - b->getSize().y*0.5f + radius * cos(angle*0.01745329251f);
	b->setPosition(bx, by);
}