#include "states/SelectionState.hpp"
#include "Game.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"
#include "Constants.hpp"
#include "core/ObjectList.hpp"
#include "utils/Content.hpp"

States::SelectionState::SelectionState(){
		m_menuFont=Content::Instance()->LoadFont("media/arial.ttf");
	 m_gui.setWindow(g_Game->GetWindow());
    m_gui.setGlobalFont(m_menuFont);
    // Now create stuff or load using loadWidgetsFromFile()
    tgui::Button::Ptr button(m_gui);
    button->load("lib/TGUI-0.6-RC/widgets/Black.conf"); // TODO: this causes an exception later when main() finishes. I don't get it all :)
    button->setPosition(30, 200);
    button->setText("Quit");
    button->setCallbackId(1);
    button->bindCallback(tgui::Button::LeftMouseClicked);
    button->setSize(300, 40);
    // Finally, use SetGui() to activate the GUI (rendering, events, callbacks)
    SetGui(&m_gui);
}

 void States::SelectionState::OnBegin(){
	 m_previousState->OnResume();
}

void States::SelectionState::AddObject(Core::ObjectID& value){
	m_objects.push_back(value);
}

void States::SelectionState::AddTilePosition(int x,int y){
	Core::ObjectList objects= g_Game->GetWorld()->GetMap(0)->GetObjectsAt(x,y);
	m_objects.insert(m_objects.end(), objects.Objects().begin(), objects.Objects().end());
}


void States::SelectionState::Update(float dt){
	m_previousState->Update(dt);
}
void States::SelectionState::Draw(sf::RenderWindow& win){
	m_previousState->Draw(win);
	//win.clear();
}