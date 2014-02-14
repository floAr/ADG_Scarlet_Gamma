#pragma once

#include "GameState.hpp"
#include "../core/Object.hpp"
#include <TGUI/TGUI.hpp>
#include "interfaces/CircularMenu.hpp"

namespace States
{
	class SelectionState : public GameState
	{
	public:
		SelectionState();
		/// \brief Set the tile to select from
		/// \details This sets the tile to select from and adds all visible objects to the selection
		/// \param [in] x	x-position in tile-coords
		/// \param [in] y	y-position in tile-coords
		/// \param [in] hiddenLayers	optional, layers which should be disregarded when selecting
		void SetTilePosition(int x, int y, const bool* hiddenLayers=nullptr);
				/// \brief Triggers a recalculation of the GUI
		/// \details Let the GUI recalculate all the buttons and position
		void RecalculateGUI();
		virtual void OnBegin() override;
		virtual void Update(float dt) override;
		virtual void GuiCallback(tgui::Callback&  args) override;
		virtual void Draw(sf::RenderWindow& win) override;
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos,
			bool guiHandled) override;

	private:
		/// \brief Show the actions for the selected object
		/// \details Sets m_finished to true.
		/// \param [in] _targetObject The object on which the selection should perform tasks.
		/// \param [in] _x Where to show the action menu.
		/// \param [in] _y Where to show the action menu.
		void ShowActionState(Core::ObjectID _targetObject, int _x, int _y);

		tgui::Gui m_gui;
		Interfaces::CircularMenu m_menu;
		Core::ObjectList m_objects;			///< A copy of the object list for the current tile.
		bool m_dirty;						///< Flag to mark a change in the GUI
		bool m_controlWasPressed;			///< Buffer for control key
		unsigned int m_screenX, m_screenY;	///< Position on screen
		const bool* m_hiddenLayers;			///< indicator if layer is visible
	};
}