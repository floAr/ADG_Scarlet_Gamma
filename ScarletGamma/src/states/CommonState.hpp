#pragma once

#include "states/GameState.hpp"
#include "states/SelectionState.hpp"
#include "utils/Falloff.hpp"
#include "Prerequisites.hpp"

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

		/// \brief Close the network connection. (Initialization was state
		///		dependent).
		virtual void OnEnd() override;

		/// \brief Poll network messages, updates zoom, pulls chat messages.
		virtual void Update(float dt) override;

		/// \brief Move the visible position on middle mouse button down
		virtual void MouseMoved(int deltaX, int deltaY, bool guiHandled) override;

		/// \brief TODO: Selection stuff
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePo, bool guiHandleds) override {}

		/// \brief Change the zoom.
		virtual void MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled) override;

		/// \brief Open a chat menu on Enter.
		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override;

		/// \brief Right-aligns the chat.
		virtual void Resize(const sf::Vector2f& _size) override;


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

		//----------------------------------------------------------------------
		// COMBAT STUFF
        virtual void BeginCombat(Core::ObjectID _object) = 0;
		virtual void EndCombat();
		GameRules::Combat* GetCombat()
		{
			return m_combat;
		}


	protected:
		Utils::Falloff m_zoom;       ///< Zoom for the current map
		tgui::Gui m_gui;             ///< A gui to show chat messages and everything else.
		std::string m_name;          ///< Name of the player used in the chat
		sf::Color m_color;           ///< Color of the player in the chat
		GameRules::Combat* m_combat; ///< Pointer to Combat in progress, 0 if none

		/// \brief Checks which actions where done and adjusts the zoom.
		void ZoomView(float delta);


		/// \param [in] _whosePath An object with the Target or Path property
		///		whose path should be drawn.
		void DrawPathOverlay(sf::RenderWindow& _window, Core::Object* _whosePath);

		void SubmitChat(const tgui::Callback& _call);
		Core::ObjectList m_selection;	///< List of currently selected objects
		bool m_selectionChanged;		///< Update gui only if something changed.
	};
}