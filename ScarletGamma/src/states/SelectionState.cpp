#include "states/SelectionState.hpp"
#include "Game.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"
#include "Constants.hpp"
#include "core/ObjectList.hpp"
#include "utils/Content.hpp"
#include "core/Object.hpp"

States::SelectionState::SelectionState(){
	m_menuFont=Content::Instance()->LoadFont("media/arial.ttf");
	m_gui.setWindow(g_Game->GetWindow());
	m_gui.setGlobalFont(m_menuFont);
	m_dirty=false;

}

void States::SelectionState::OnBegin(){
	m_previousState->OnResume();
	m_objects.clear();
}

void States::SelectionState::AddObject(Core::ObjectID& value){
	m_objects.push_back(value);
	m_dirty=true;
}

void States::SelectionState::AddTilePosition(int x,int y){
	Core::ObjectList objects= g_Game->GetWorld()->GetMap(0)->GetObjectsAt(x,y);
	m_objects.insert(m_objects.end(), objects.Objects().begin(), objects.Objects().end());
	m_dirty=true;
}

void States::SelectionState::RecalculateGUI(){
	m_gui.removeAllWidgets(); //clear gui


	int count=m_objects.size();

	int i;
	for(i=0;i<count;i++){
		Core::Object* o=g_Game->GetWorld()->GetObject(m_objects[i]);
		tgui::Button::Ptr button(m_gui);
		button->load("lib/TGUI-0.6-RC/widgets/Black.conf"); // TODO: this causes an exception later when main() finishes. I don't get it all :)
		button->setPosition(30, 200+i*45);
		if(o->HasProperty(Core::Object::PROP_NAME))
			button->setText(o->GetProperty(Core::Object::PROP_NAME).Value());
		else
			button->setText(o->ID());
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
}
void States::SelectionState::Draw(sf::RenderWindow& win){
	m_previousState->Draw(win);
	//win.clear();
}

void States::SelectionState::GuiCallback(tgui::Callback& args){
	if(args.id>=100)//item clicked
	{
		//todo inject objects
		m_previousState->AddToSelection(m_objects[args.id-100]);
		this->m_finished=true;
	}
}
