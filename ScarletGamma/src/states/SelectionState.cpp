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

States::SelectionState::SelectionState(){
	m_menuFont=Content::Instance()->LoadFont("media/arial.ttf");
	m_gui.setWindow(g_Game->GetWindow());
	m_gui.setGlobalFont(m_menuFont);
	m_dirty=false;

}

void States::SelectionState::OnBegin(){
	m_previousState->OnResume();
	m_objects.clear();
	m_selected=false;
}

void States::SelectionState::AddObject(Core::ObjectID& value){
	m_objects.push_back(value);
	m_dirty=true;
}

void States::SelectionState::AddTilePosition(int x,int y){
	Core::ObjectList objects= g_Game->GetWorld()->GetMap(0)->GetObjectsAt(x,y);
	m_objects.insert(m_objects.end(), objects.Objects().begin(), objects.Objects().end());
	m_x=x;
	m_y=y;
	m_dirty=true;
}

void States::SelectionState::RecalculateGUI(){
	m_gui.removeAllWidgets(); //clear gui

	//TODO Get current selection to make buttons transparent when they not selected
	CommonState* previousState = dynamic_cast<CommonState*>(m_previousState);
	m_alreadySelected=previousState->GetSelection();


	int count=m_objects.size();

	int i;
	for(i=0;i<count;i++){
		Core::Object* o=g_Game->GetWorld()->GetObject(m_objects[i]);
		tgui::Button::Ptr button(m_gui);
		button->load("lib/TGUI-0.6-RC/widgets/Black.conf"); // TODO: this causes an exception later when main() finishes. I don't get it all :)
		//button->setPosition(m_x*TILESIZE, float(m_y*TILESIZE+i*45));
		positionButton(button,360/count*i,45);
		if(o->HasProperty(Core::Object::PROP_NAME))
			button->setText(o->GetProperty(Core::Object::PROP_NAME).Value());
		else
			button->setText(std::to_string(o->ID()));
		if(std::find(m_alreadySelected->Objects().begin(), m_alreadySelected->Objects().end(), m_objects[i]) != m_alreadySelected->Objects().end()) {
			//already selected
			button->setTransparency(255);
		} else {
			button->setTransparency(150);
		}

		button->setCallbackId(100+i);
		button->bindCallback(tgui::Button::LeftMouseClicked);
		button->setSize(50, 40);
		// Finally, use SetGui() to activate the GUI (rendering, events, callbacks)
	}

	SetGui(&m_gui);
	m_dirty=false;
}

void States::SelectionState::Update(float dt){
	if(m_dirty)
		RecalculateGUI();
	m_previousState->Update(dt);
	if(m_selected&&!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
		m_finished=true;
}
void States::SelectionState::Draw(sf::RenderWindow& win){
	m_previousState->Draw(win);
	//win.clear();

	GameState::Draw(win);

}

void States::SelectionState::GuiCallback(tgui::Callback& args){
	if(args.id>=100)//item clicked
	{
		//todo inject objects
		CommonState* previousState = dynamic_cast<CommonState*>(m_previousState);
		// The parent is not set or not of type CommonState, but it should be!
		assert(previousState);
	m_alreadySelected=previousState->GetSelection();

	if(std::find(m_alreadySelected->Objects().begin(), m_alreadySelected->Objects().end(), m_objects[args.id-100]) != m_alreadySelected->Objects().end()) {
			//already selected
		previousState->RemoveFromSelection(m_objects[args.id-100]);
		} else {
			previousState->AddToSelection(m_objects[args.id-100]);
		}
	m_dirty=true;
	m_selected=true;

	}
}



void States::SelectionState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos)
{
	if(button.button==sf::Mouse::Button::Right)
	{
	//	m_finished=true;
	//	m_previousState->MouseButtonPressed(button,tilePos);
		
	}
}


void  States::SelectionState::positionButton(tgui::Button::Ptr b,float angle,float radius){
	float bx = m_x*TILESIZE + radius * sin(angle*0.01745329251); //0.01745329251 is to got radians from degrees
	float by = m_y*TILESIZE + radius * cos(angle*0.01745329251 );
   b->setPosition(bx,by);

}