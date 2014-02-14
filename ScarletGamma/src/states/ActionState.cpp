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
	m_menu(m_gui)
{
	m_targetObject=-1;
	m_sourceObject=-1;
	m_gui.setWindow(g_Game->GetWindow());
	m_gui.setGlobalFont( Content::Instance()->LoadFont("media/arial.ttf") );
	m_dirty = false;

	// Finally, use SetGui() to activate the GUI (rendering, events, callbacks)
	SetGui(&m_gui);
}

void States::ActionState::SetTargetObject(Core::ObjectID id){
	m_targetObject=id;
	m_dirty=true;
}

void States::ActionState::SetSourceObject(Core::ObjectID id){
	m_sourceObject=id;
	m_dirty=true;
}

void States::ActionState::SetPosition(int x, int y) {
	m_screenX = (float)x;
	m_screenY = (float)y;
}
void States::ActionState::OnBegin()
{
}


void States::ActionState::RecalculateGUI()
{
	//get the current object
	Core::Object* o=g_Game->GetWorld()->GetObject(m_targetObject);
	auto pos=o->GetPosition();
	//m_screenX=pos.x*TILESIZE;
	//m_screenY=pos.y*TILESIZE;
	//get action list
    Core::ObjectList executors;
	executors.Add( m_sourceObject );
	std::vector<Core::ActionID> actions = Actions::ActionPool::Instance().GetAllowedActions(executors, *o);

	std::vector<Interfaces::CircularMenu::ObjInfo> items;
	//for each action 
	for(std::vector<Core::ActionID>::iterator it = actions.begin(); it != actions.end(); ++it) {
		items.push_back( Interfaces::CircularMenu::ObjInfo( *it, true ) );
	}

	m_menu.Show( sf::Vector2f((float)m_screenX, (float)m_screenY), items );

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
	Actions::ActionPool::Instance().StartLocalAction(args.id, m_sourceObject, m_targetObject);
	m_finished=true;
}

void States::ActionState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos,
											 bool guiHandled)
{
	// Return if the GUI already handled it
	if (guiHandled)
		return;

	//if not close the state
	m_finished = true;
}
