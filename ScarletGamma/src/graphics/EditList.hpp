#pragma  once

#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include "Prerequisites.hpp"

namespace Graphics {

	/// \brief A container to show a list of editable values.
	class EditList: public tgui::Panel
	{
	public:
		typedef tgui::SharedWidgetPtr<EditList> Ptr;

		EditList();

		/// \param [in] _addNdel Add and delete buttons for lines are shown.
		/// \param [in] _leftEditable The edit boxes on the left side are editable.
		/// \param [in] _rightEditable The edit boxes on the left side are editable.
		/// \param [in] _autosize If true the component is resized to the required
		///		height. Otherwise the hight is kept and a scrollbar is shown.
		///	\param [in] _pid An id set by a parent EditList - must be 0xffffffff!
		void Init( const std::string& _title,
			float _x, float _y, float _w, float _h,
			bool _addNdel, bool _leftEditable, bool _rightEditable, bool _autosize,
			unsigned _pid=0xffffffff);

		/// \brief Extract all properties of an object recursively.
		void Show( Core::Object* _object );


		virtual void setSize(float width, float height);
		virtual sf::Vector2f getSize() const;
	protected:
		tgui::EditBox::Ptr m_newName;
		tgui::EditBox::Ptr m_newValue;
		tgui::Button::Ptr m_newAdd;
		tgui::Scrollbar::Ptr m_scrollBar;
		tgui::EditBox::Ptr m_titleBar;
		tgui::AnimatedPicture::Ptr m_miniMaxi;

		/// \brief A struct to reference all components of a line in one place.
		/// \details This struct is used for faster searches.
		struct EntryLine {
			tgui::EditBox::Ptr left;
			tgui::EditBox::Ptr right;
			tgui::Checkbox::Ptr del;
			EditList::Ptr subNode;

			EntryLine() : left(nullptr), right(nullptr), del(nullptr), subNode(nullptr)	{}
		};

		/// \brief A map of references to find the 2/3 objects belonging to
		///		the same line.
		///	\details The callbackId of each element is its line index in this
		///		vector.
		std::vector<EntryLine> m_lines;
		
		bool m_addNdel;
		bool m_leftEditable;
		bool m_rightEditable;
		bool m_autoSize;
		int m_oldScrollValue;		///< The damned scrollbar-change does not send the previous value.
		int m_numPixelLines;		///< Total number of pixels covered by elements inside the list.

		void RemoveBtn(const tgui::Callback& _call);
		void AddBtn(const tgui::Callback& _call);
		void Scroll(const tgui::Callback& _call);
		void MiniMaxi(const tgui::Callback& _call);
		//void ScrollbarVisibilityChanged();

		/// \brief Add or remove vertical space.
		/// \details If autosize is enabled just change container height.
		///		Otherwise the scrollbar is adapted.
		///	\param [in] _addLines How many pixels of space.
		///	\param [in] _where Space can be added between components. All
		///		components after where are moved down.
		void Resize(int _addLines, int _where);

		bool IsScrollbarVisible();
		float GetHeight() const;

		/// \brief Add or replace a line. The left side is the unique key.
		void Add( const std::string& _left, const std::string& _right );

		typedef int NodeID;

		/// \brief Add a separating line which can be closed/opened and
		///		can contain sub elements.
		///	\param [in] _parentName The name of the property/line which should
		///		be the parent of this node.
		///	\param [in] _title What should be written in the separating line?
		Ptr AddNode(const std::string&  _parentName, const std::string& _title);
	};

} // namespace Graphics