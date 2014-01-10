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
		/// \param [in] _autosize If true the component is resized to the required
		///		height. Otherwise the hight is kept and a scrollbar is shown.
		EditList( tgui::Gui& _gui, float _x, float _y, float _w, float _h,
			bool _addNdel, bool _leftEditable, bool _rightEditable, bool _autosize,
			const std::string& _title);

		void Add( const std::string& _left, const std::string& _right );

		typedef int NodeID;

		/// \brief Add a separating line which can be closed/opened and
		///		can contain sub elements.
		NodeID AddNode(const std::string& _title);
	private:
		tgui::Panel::Ptr m_list;
		tgui::EditBox::Ptr m_newName;
		tgui::EditBox::Ptr m_newValue;
		tgui::Button::Ptr m_newAdd;
		tgui::Scrollbar::Ptr m_scrollBar;
		tgui::Gui& m_gui;
	//	std::unordered_map<NodeID, int> m_nodes;	// Maps to an attribute line.
		
		bool m_addNdel;
		bool m_leftEditable;
		bool m_rightEditable;
		int m_oldScrollValue;		///< The damned scrollbar-change does not send the previous value.
		int m_numLines;				///< Total number of lines inside the list.

		void RemoveBtn(const tgui::Callback& _call);
		void AddBtn(const tgui::Callback& _call);
		void Scroll(const tgui::Callback& _call);
		void MiniMaxi(const tgui::Callback& _call);
		void ScrollbarVisibilityChanged();
	};

} // namespace Graphics