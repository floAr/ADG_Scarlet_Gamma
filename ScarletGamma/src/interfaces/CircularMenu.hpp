#pragma once

#include "Prerequisites.hpp"
#include "TGUI/TGUI.hpp"

namespace Interfaces {

	class CircularMenu
	{
	public:
		/// \brief The constructor takes the gui in which the menu should be
		///		created.
		CircularMenu( tgui::Gui& _gui );

		/// \brief Removes the remaining buttons from the circular menu.
		~CircularMenu();

		struct ObjInfo
		{
			Core::ObjectID id;	///< The object id determines the button callback-id
			bool highlight;		///< This button should be better visible than others
			std::string text;	///< What should be written on the button

			ObjInfo( Core::ObjectID _id, bool _highlight, const std::string& _text ) :
				id(_id), highlight(_highlight), text(_text)	{}
		};

		/// \brief Create as many buttons as necessary and order them in a
		///		circle.
		void Show( const sf::Vector2f& _position, const std::vector<ObjInfo>& _objects );

		/// \brief Find out if one of the buttons is below th position
		/// \return INVALID_ID on failure otherwise the callback id given during creation.
		Core::ObjectID GetClickedItem(float _x, float _y);
	private:
		/// \brief A list of the created buttons to remove them from gui.
		std::vector<tgui::Widget::Ptr> m_buttons;

		tgui::Gui& m_gui;
		tgui::Button::Ptr m_defaultButton;	///< One reference button to be copied for speed up
		tgui::Picture::Ptr m_circle;		///< The circle in the background

		/// \brief Remove all buttons from the gui
		void Clear();

		/// \brief Compute the position for a given button
		void PositionButton(const sf::Vector2f& _center, tgui::Button::Ptr _button, float _angle, float _radius);
	};

} // namespace Interfaces