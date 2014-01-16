#pragma  once

#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include "Prerequisites.hpp"

namespace Interfaces {

	/// \brief A container to show a list of editable values.
	class ObjectPanel: public tgui::Panel
	{
	public:
		typedef tgui::SharedWidgetPtr<ObjectPanel> Ptr;

		ObjectPanel();

		/// \param [in] _addAble Add and delete buttons for lines are shown.
		void Init( float _x, float _y, float _w, float _h,
			bool _addAble, Core::World* _world,
			Interfaces::DragContent** _dragNDropHandler );

		bool IsMinimized() const { return m_miniMaxi->getCurrentFrame() == 0; }

	protected:
		tgui::EditBox::Ptr m_newName;
		tgui::Button::Ptr m_newAdd;
		tgui::Scrollbar::Ptr m_scrollBar;
		tgui::EditBox::Ptr m_titleBar;			///< Contains the object name in minimized state and is the searchbar if maximized
		tgui::AnimatedPicture::Ptr m_miniMaxi;
		
		bool m_addAble;
		int m_oldScrollValue;		///< The damned scrollbar-change does not send the previous value.
		int m_numPixelLines;		///< Total number of pixels covered by elements inside the list.
		Core::World* m_world;
		Interfaces::DragContent** m_dragNDropHandler;	///< A pointer to a pointer which must be filled if a mouse down event occures.

		void RemoveBtn(const tgui::Callback& _call);
		void AddBtn(const tgui::Callback& _call);
		void Scroll(const tgui::Callback& _call);
		void MiniMaxi(const tgui::Callback& _call);
		void StartDrag(const tgui::Callback& _call);

		/// \brief Extract all properties of an object recursively.
		void Add( Core::ObjectID _object );

		/// \brief Add or remove vertical space.
		///	\param [in] _addLines How many pixels of space.
		///	\param [in] _where Space can be added between components. All
		///		components after where are moved down.
		void Resize(int _addLines, int _where);

		/// \brief Remove all properties from the list
		void Clear();

		/// \brief Remove all lines and then show the once remaining after
		///		filtering.
		void RefreshFilter();
	};

} // namespace Interfaces