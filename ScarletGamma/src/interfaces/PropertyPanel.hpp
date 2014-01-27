#pragma  once

#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include "Prerequisites.hpp"

namespace Interfaces {

	/// \brief A container to show a list of editable values.
	class PropertyPanel: public tgui::Panel
	{
	public:
		typedef tgui::SharedWidgetPtr<PropertyPanel> Ptr;

		PropertyPanel();

		/// \param [in] _addAble Add and delete buttons for lines are shown.
		/// \param [in] _player The player (id) who looks at the properties - to apply rights system.
		/// \param [in] _autoSize If true the component is resized to the required
		///		height. Otherwise the hight is kept and a scrollbar is shown.
		///	\param [in] _pid An id set by a parent PropertyPanel - must be 0xffffffff!
		void Init( float _x, float _y, float _w, float _h,
			bool _addAble, bool _autoSize, Core::PlayerID _player,
			Interfaces::DragContent** _dragNDropHandler,
			unsigned _pid=0xffffffff);

		/// \brief Extract all properties of an object recursively.
		void Show( Core::Object* _object );

		/// \brief Extract all common properties of many object without recursion.
		void Show( Core::World* _world, const Core::ObjectList& _objects );

		bool IsMinimized() const { return m_miniMaxi->getCurrentFrame() == 0; }

		virtual void setSize(float _width, float _height) override;
		virtual sf::Vector2f getSize() const override;
		virtual void setPosition(float _x, float _y) override;
		virtual void unfocus() override;
		virtual void hide() override;
		virtual void show() override;

		/// \brief If an object is dragged to this panel all properties which
		///		are not existent are copied and the same holds for single properties.
		void HandleDropEvent();
	protected:
		// A list of preloaded objects to increase creation time by a very large
		// factor.
		tgui::EditBox::Ptr m_basicEdit;
		tgui::Checkbox::Ptr m_basicDeleteButton;
		tgui::Button::Ptr m_basicAddButton;
		tgui::Scrollbar::Ptr m_basicScrollBar;
		tgui::AnimatedPicture::Ptr m_basicMiniMaxi;

		tgui::EditBox::Ptr m_newName;
		tgui::EditBox::Ptr m_newValue;
		tgui::Button::Ptr m_newAdd;
		tgui::Scrollbar::Ptr m_scrollBar;
		tgui::EditBox::Ptr m_titleBar;			///< Contains the object name in minimized state and is the searchbar if maximized
		tgui::AnimatedPicture::Ptr m_miniMaxi;

		/// \brief A struct to reference all components of a line in one place.
		/// \details This struct is used for faster searches.
		struct EntryLine {
			tgui::EditBox::Ptr left;
			tgui::EditBox::Ptr right;
			tgui::Checkbox::Ptr del;
			PropertyPanel::Ptr subNode;

			EntryLine() : left(nullptr), right(nullptr), del(nullptr), subNode(nullptr)	{}
		};

		/// \brief A map of references to find the 2/3 objects belonging to
		///		the same line.
		///	\details The callbackId of each element is its line index in this
		///		vector.
		std::vector<EntryLine> m_lines;
		
		bool m_addAble;
		bool m_autoSize;
		int m_oldScrollValue;		///< The damned scrollbar-change does not send the previous value.
		int m_numPixelLines;		///< Total number of pixels covered by elements inside the list.
		Core::PlayerID m_player;	///< The accessing player to apply rights system
		std::vector<Core::Object*> m_objects;			///< Associated objects which are visualized
		Interfaces::DragContent** m_dragNDropHandler;	///< A pointer to a pointer which must be filled if a mouse down event occures.

		void RemoveBtn(const tgui::Callback& _call);
		void AddBtn(const tgui::Callback& _call);
		void Scroll(const tgui::Callback& _call);
		void MiniMaxi(const tgui::Callback& _call);
		void ValueEntered(const tgui::Callback& _call);
		void ValueChanged(const tgui::Callback& _call);
		void StartDrag(const tgui::Callback& _call);

		/// \brief Add or remove vertical space.
		/// \details If auto size is enabled just change container height.
		///		Otherwise the scrollbar is adapted.
		///	\param [in] _addLines How many pixels of space.
		///	\param [in] _where Space can be added between components. All
		///		components after where are moved down.
		void Resize(int _addLines, int _where);

		/// \brief Check if scrollbar is used. In auto size mode it is not.
		bool IsScrollbarVisible();

		/// \brief Compute total height: Panel+TitleBar+AddLine
		float GetHeight() const;

		/// \brief Add or replace a line. The left side is the unique key.
		void Add( const std::string& _left, bool _changable, const std::string& _right, bool _editable );

		typedef int NodeID;

		/// \brief Add a separating line which can be closed/opened and
		///		can contain sub elements.
		///	\param [in] _parentName The name of the property/line which should
		///		be the parent of this node.
		Ptr AddNode(const std::string&  _parentName);

		/// \brief Remove all properties from the list
		void Clear();

		/// \brief Remove all lines and then show the once remaining after
		///		filtering.
		void RefreshFilter();
	};

} // namespace Interfaces