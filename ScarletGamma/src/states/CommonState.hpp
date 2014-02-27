#pragma once

#include "states/GameState.hpp"
#include "states/SelectionState.hpp"
#include "utils/Falloff.hpp"
#include "Prerequisites.hpp"
#include "interfaces/DragNDrop.hpp"
#include "interfaces/CombatantPanel.hpp"
#include <unordered_map>

namespace States
{
	/// \brief A base class for master and player state to cover common actions.
	/// \details All implemented methods should be used form derived class if
	///		the set of actions should be used. Which actions are done by the
	///		implementation is stated by the method documentation.
	class CommonState : public GameState
	{
	public:
		/// \brief Creates a Zoom object, The GUI for chats.
		CommonState();

		~CommonState();

		/// \brief Sets correct mouse cursor.
		virtual void OnResume() override;

		/// \brief Sets default mouse cursor.
		virtual void OnPause() override;

		/// \brief Close the network connection. (Initialization was state
		///		dependent).
		virtual void OnEnd() override;

		/// \brief Poll network messages, updates zoom, pulls chat messages.
		virtual void Update(float dt) override;

		/// \brief Move the visible position on middle mouse button down
		virtual void MouseMoved(int deltaX, int deltaY, bool guiHandled) override;

		/// \brief Open a chat menu on Enter.
		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override;

		/// \brief Right-aligns the chat.
		virtual void Resize(const sf::Vector2f& _size) override;

		virtual void CreateDiceRollState();


		//----------------------------------------------------------------------
		// SELECTION STUFF
		/// \brief Add an object to the current selection
		void AddToSelection(Core::ObjectID id);

		/// \brief Remove an object from the current selection
		void RemoveFromSelection(Core::ObjectID id);

		/// \brief Clear the current selection
		void ClearSelection() {
			m_selection.Clear();
			m_selectionChanged = true;
		}

		/// \brief Get the current selection
		const Core::ObjectList* GetSelection() const {
			return &m_selection;
		}

		/// \brief Return the current drawn map. The implementation depends
		///		on the mode.
		virtual Core::Map* GetCurrentMap() = 0;

		/// \brief Check if a layer is currently visible
		bool IsLayerVisible( int _layer )	{ return m_hiddenLayers[_layer-1] != 0; }

		/// \brief Go to some object (switch view)
		void GoTo( const Core::Object* _object );

		//----------------------------------------------------------------------
		// COMBAT STUFF
		virtual void CreateCombat(Core::ObjectID _object) = 0;
		virtual void EndCombat();
		GameRules::Combat* GetCombat()
		{
			return m_combat;
		}

		/// \brief Test whether we are currently in combat.
		bool InCombat()
		{
			return m_combat != nullptr;
		}

		/// \brief Checks whether an object is owned by the CommonState's owner.
		virtual bool OwnsObject(Core::ObjectID _object) = 0;

		/// \brief Returns a pointer to the combatant panel, to be filled by the combat.
		Interfaces::CombatantPanel::Ptr GetCombatantPanel()
		{
			return m_combatantPanel;
		}

	protected:
		Utils::Falloff m_zoom;       ///< Zoom for the current map
		tgui::Gui m_gui;             ///< A gui to show chat messages and everything else.
		std::string m_name;          ///< Name of the player used in the chat
		sf::Color m_color;           ///< Color of the player in the chat
		GameRules::Combat* m_combat; ///< Pointer to Combat in progress, 0 if none
		std::unordered_map<int,Core::ObjectID> m_hotkeys;	///< Defined hotkeys for entities

		PromptState* m_diceRollState;	///< Pointer to the state to roll the dice

		std::vector<char> m_hiddenLayers;	///< List of currently hidden layers

		/// \brief Some components fill this with content if mouse is pushed.
		///		On release an according action should be done.
		Interfaces::DragContent* m_draggedContent;

		/// \brief Panel that shows the combatants.
		Interfaces::CombatantPanel::Ptr m_combatantPanel;

		/// \brief Checks which actions where done and adjusts the zoom.
		void ZoomView(float delta);

		/// \param [in] _whosePath An object with the Target or Path property
		///		whose path should be drawn.
		void DrawPathOverlay(sf::RenderWindow& _window, Core::Object* _whosePath);

		void SubmitChat(const tgui::Callback& _call);
		Core::ObjectList m_selection;	///< List of currently selected objects
		bool m_selectionChanged;		///< Update gui only if something changed.

		/// \brief Find the best fitting layer if an object is drawn to the map
		int AutoDetectLayer( const Core::Object* _object );

		/// \brief Search in all visible layers for an object.
		/// \return An ObjectID or INVALID_ID if nothing could be found
		Core::ObjectID FindTopmostTile(int _x, int _y);

		/// \brief Get the object id which was mapped to the key.
		/// \details Try to append the current view onto another object, if player does not exist nothing happens
		/// \param [in] hotkey	pressed hotkey of the player
		Core::Object* GetObjectFromHotkey(const int hotkey);

		/// \brief Bind an object to the hotkey
		/// \details Save the currently focused object with the pressed hotkey
		/// \param [in] hotkey	pressed hotkey of the player
		/// \param [in] objectID	object to remember
		void SetHotkeyToObject(const int hotkey, Core::ObjectID objectID);
	};
}