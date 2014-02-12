#pragma once

#include <TGUI/TGUI.hpp>
#include "Prerequisites.hpp"
#include "tools/brush.hpp"
#include "DragNDrop.hpp"

namespace Interfaces {

	/// \brief A class to create arbitrary tool bars containing a list of toolboxes.
	class Toolbar: public tgui::Panel
	{
	public:
		typedef tgui::SharedWidgetPtr<Toolbar> Ptr;

		Toolbar();

		/// \brief Create all components of a standard toolbar
		/// \param [in] _x Top left position (x) in pixels.
		/// \param [in] _y Top left position (y) in pixels.
		/// \param [in] _w Width in pixels.
		/// \param [in] _h Height in pixels.
		void Init(float _x, float _y, float _w, float _h);

		/// \brief Add a box which is assumed to fit into this toolbar.
		/// \details \see Container::add
		/// 
		///		The box is appended at the right end of the other boxes.
		///	\return The index where the element was added.
		int AddToolbox( tgui::Widget::Ptr _box );

		/// \brief Scroll while mouse is down on one of the two buttons.
		void Update( float _dt );

		/// \brief Hide/Show a box and reset the positions of the following ones.
		/// \details Newly added boxes are visible per default.
		void SetBoxVisiblity( int _index, bool _visible );

		virtual void unfocus() override;
		virtual void setSize(float _width, float _height) override;
	private:
		tgui::Panel::Ptr m_scrollPanel;
		tgui::Picture::Ptr m_scrollLeft;
		tgui::Picture::Ptr m_scrollRight;

		float m_widthSum;		///< The width of all toolboxes together
		float m_scroll;			///< Direction if one of the buttons is pressed, otherwise 0.

		void BeginScroll(const tgui::Callback& _call);
		void EndScroll(const tgui::Callback& _call);
	};

	/// \brief Toolbox base class.
	class Toolbox: public tgui::Panel
	{
	public:
		typedef tgui::SharedWidgetPtr<Toolbox> Ptr;

		/// \brief Create all subcomponents here.
		/// \details The creation of subcomponents must be postponed
		///		until this element has a true parent. That why Init
		///		is required and elements can not be constructed in Ctor.
		virtual void Init() = 0;

		/// \brief Continuously update of the box which implements this function
		virtual void Update( float _dt ) {}

		virtual void unfocus() override;
	};

	/// \brief Toolbox for map selection and creation.
	class MapToolbox: public Toolbox
	{
	public:
		typedef tgui::SharedWidgetPtr<MapToolbox> Ptr;

		// Create all elements in the map toolbox.
		MapToolbox();

		virtual void Init() override;

		/// \brief Which map is selected?
		Core::MapID GetSelectedMap() const		{ return m_maps.size()>0 ? m_maps[m_selected] : 0; }
	private:
		/// \brief A mapping from list indices to map ids.
		std::vector<Core::MapID> m_maps;
		int m_selected;

		tgui::ListBox::Ptr m_mapList;
		tgui::EditBox::Ptr m_newNameEdit;

		void AddMap();
	};

	/// \brief Toolbox for map selection and creation.
	class BrushToolbox: public Toolbox
	{
	public:
		typedef tgui::SharedWidgetPtr<BrushToolbox> Ptr;

		BrushToolbox();

		/// \brief Create all elements in the brush toolbox.
		virtual void Init() override;

		/// \brief Poll the selected brush diameter
		int GetDiameter() const;
		/// \brief Poll the selected target layer
		int GetLayer() const;
		/// \brief Poll the selected brush mode
		Tools::Brush::Mode GetMode() const;

	private:
		tgui::ComboBox::Ptr m_actionChoice;
		tgui::ComboBox::Ptr m_layer;
		tgui::Slider::Ptr m_diameter;
	};

	/// \brief Toolbox to switch between Selecting and Editing mode in the master tool.
	class ModeToolbox: public Toolbox
	{
	public:
		typedef tgui::SharedWidgetPtr<ModeToolbox> Ptr;

		ModeToolbox();

		enum Mode {
			SELECTION = 0,
			BRUSH = 1,
			ACTION = 2
		};

		// Create all elements in the mode toolbox.
		virtual void Init() override;

		Mode GetMode() const			{ return Mode(m_selected); }
		BrushToolbox::Ptr Brush() const { return m_brushBox; }
	private:
		tgui::ListBox::Ptr m_actionList;	///< List of possible modes
		int m_selected;	///< Store last selected modus

		// Other toolboxes shown depending on the mode
		BrushToolbox::Ptr m_brushBox;
		int m_brushIndex;	// index to reference the toolbox in hide/show

		void SelectMode(const tgui::Callback& _call);
	};

	/// \brief Toolbox for map selection and creation.
	class PlayersToolbox: public Toolbox
	{
	public:
		typedef tgui::SharedWidgetPtr<PlayersToolbox> Ptr;

		// Create all elements in the map toolbox.
		PlayersToolbox();

		virtual void Init() override;

		void SetDragNDropHandler( DragContent** _dragNDropHandler )	{ m_dragNDropHandler = _dragNDropHandler; }

		/// \brief Refresh the list if new players are added or a player died
		virtual void Update( float _dt ) override;

		Core::Object* GetPlayer( float _x, float _y );
	private:
		float m_lastUpdate;					///< Counter to avoid to many updates
		tgui::ListBox::Ptr m_playerList;	///< List of players
		DragContent** m_dragNDropHandler;	///< Access to the global drag&drop handler

		void DragPlayer(const tgui::Callback& _caller);
	};

	/// \brief Toolbox for map selection and creation.
	class NPCToolbox: public Toolbox
	{
	public:
		typedef tgui::SharedWidgetPtr<NPCToolbox> Ptr;

		// Create all elements in the map toolbox.
		NPCToolbox();

		virtual void Init() override;
	};

} // namespace Interfaces