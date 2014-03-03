#pragma  once

#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include "Prerequisites.hpp"
#include "PropertyPanel.hpp"
#include "DragNDrop.hpp"

namespace Interfaces {

	/// \brief A container to show a list of editable values.
	class ObjectPanel: public tgui::Panel
	{
	public:
		typedef tgui::SharedWidgetPtr<ObjectPanel> Ptr;

		ObjectPanel();

		/// \param [in] _x Top left position (x) in pixels.
		/// \param [in] _y Top left position (y) in pixels.
		/// \param [in] _w Width in pixels.
		/// \param [in] _h Height in pixels.
		/// \param [in] _addAble Add and delete buttons for lines are shown.
		/// \param [in] _source As what should this GUI fill the DragNDrop handler.
		void Init( float _x, float _y, float _w, float _h,
			bool _addAble, Interfaces::DragContent::Sources _source,
			Interfaces::DragContent** _dragNDropHandler,
			Interfaces::PropertyPanel::Ptr _viewer );

		bool IsMinimized() const { return m_miniMaxi->getCurrentFrame() == 0; }

		virtual void setSize(float _width, float _height) override;
		virtual sf::Vector2f getSize() const override;
		virtual void setPosition(float _x, float _y) override;

		/// \brief Just ignore things dragged to the object list
		void HandleDropEvent()	{ delete *m_dragNDropHandler; *m_dragNDropHandler = nullptr; }

		/// \brief Get the object which was clicked last.
		const Core::Object* GetSelected() const	{ return m_selected; }

		/// \brief Call this if mouse is scrolled. It will move the scrollbar
		///		if the cursor is on this component.
		void Scroll( int _delta );

		/// \brief Remove all lines and then show the once remaining after
		///		filtering.
		void RefreshFilter();
	protected:
		tgui::EditBox::Ptr m_newName;
		tgui::Button::Ptr m_newAdd;
		tgui::Scrollbar::Ptr m_scrollBar;
		tgui::EditBox::Ptr m_titleBar;			///< Contains the object name in minimized state and is the searchbar if maximized
		tgui::AnimatedPicture::Ptr m_miniMaxi;
		tgui::EditBox::Ptr m_defaultEdit;
		tgui::Checkbox::Ptr m_defaultDelete;
		
		bool m_addAble;
		int m_oldScrollValue;		///< The damned scrollbar-change does not send the previous value.
		int m_numPixelLines;		///< Total number of pixels covered by elements inside the list.
	//	clock_t m_lastClick;		///< Timestamp when was the last mouse click? Required to detect double click.
		Interfaces::DragContent** m_dragNDropHandler;	///< A pointer to a pointer which must be filled if a mouse down event occures.
		Interfaces::DragContent::Sources m_dragNDropSource;		///< As what should this GUI fill the DragNDrop handler.
		Interfaces::PropertyPanel::Ptr m_viewer;		///< A panel to view a selected object.
		Core::Object* m_selected;

		void RemoveBtn(const tgui::Callback& _call);
		void AddBtn(const tgui::Callback& _call);
		void Scroll(const tgui::Callback& _call);
		void MiniMaxi(const tgui::Callback& _call);
		void StartDrag(const tgui::Callback& _call);
		void Drop();
		void SelectObject(const tgui::Callback& _call);

		/// \brief Extract all properties of an object recursively.
		void Add( Core::ObjectID _object );

		/// \brief Add or remove vertical space.
		///	\param [in] _addLines How many pixels of space.
		///	\param [in] _where Space can be added between components. All
		///		components after where are moved down.
		void Resize(int _addLines, int _where);

		/// \brief Remove all properties from the list
		void Clear();
	};

} // namespace Interfaces