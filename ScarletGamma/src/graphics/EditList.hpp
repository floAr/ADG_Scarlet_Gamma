#pragma  once

#include <TGUI/TGUI.hpp>

namespace Graphics {

	/// \brief A container to show a list of editable values.
	class EditList
	{
	public:
		/// \param [in] _gui Parent gui to create this container.
		/// \param [in] _addNdel Add and delete buttons for lines are shown.
		/// \param [in] _leftEditable The edit boxes on the left side are editable.
		/// \param [in] _rightEditable The edit boxes on the left side are editable.
		EditList( tgui::Gui& _gui, float _x, float _y, float _w, float _h,
			bool _addNdel, bool _leftEditable, bool _rightEditable );

		void Add( const std::string& _left, const std::string& _right );
	private:
		tgui::Panel::Ptr m_list;
		tgui::EditBox::Ptr m_newName;
		tgui::EditBox::Ptr m_newValue;
		tgui::Scrollbar::Ptr m_scrollBar;
		tgui::Gui& m_gui;
		
		bool m_addNdel;
		bool m_leftEditable;
		bool m_rightEditable;
		int m_oldScrollValue;

		void RemoveBtn(const tgui::Callback& _call);
		void AddBtn(const tgui::Callback& _call);
		void Scroll(const tgui::Callback& _call);
	};

} // namespace Graphics