#pragma once

#include "GameState.hpp"
#include "../core/Object.hpp"
#include <TGUI/TGUI.hpp>

namespace States
{
	class SelectionState : public GameState
	{
	public:
		SelectionState();
		void AddObject(Core::ObjectID &value );
		void AddTilePosition(int x,int y);
		void RecalculateGUI();
		virtual void OnBegin();
		virtual void OnEnd() {}
		virtual void OnPause() {}
		virtual void OnResume() {}
		virtual void Update(float dt);
		virtual void GuiCallback(tgui::Callback&  args);
		virtual void Draw(sf::RenderWindow& win);
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos);

		private:
			void positionButton(tgui::Button::Ptr b,float angle,float radius);

		tgui::Gui m_gui;
		sf::Font m_menuFont;
		std::vector<Core::ObjectID> m_objects;
		Core::ObjectList* m_alreadySelected;
		bool m_dirty;
		bool m_selected;
		int m_x,m_y;
	};
}