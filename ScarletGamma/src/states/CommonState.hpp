#pragma once

#include "states/GameState.hpp"
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

		/// \brief Move the visible position on left mouse button down
		/// TODO: Restrict player - this is a master only action
		virtual void MouseMoved(int deltaX, int deltaY) override;

		/// \brief TODO: Selection stuff
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos) override {}

		/// \brief Change the zoom.
		virtual void MouseWheelMoved(sf::Event::MouseWheelEvent& wheel) override;

		/// \brief Open a chat menu on Enter.
		virtual void KeyPressed(sf::Event::KeyEvent& key) override;
	protected:
		Utils::Falloff m_zoom;		///< Zoom for the current map
		Core::Object* m_selected;	///< A reference to the selected object or nullptr
		tgui::Gui m_chatFrame;		///< A gui to show chat messages.
		std::string m_name;			///< Name of the player used in the chat
		sf::Color m_color;			///< Color of the player in the chat

		/// \brief Checks which actions where done and adjusts the zoom.
		void ZoomView(float delta);


		/// \param [in] _whosePath An object with the Target or Path property
		///		whose path should be drawn.
		void DrawPathOverlay(sf::RenderWindow& win, Core::Object* _whosePath);
	};
}