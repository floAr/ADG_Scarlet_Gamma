#pragma once

#include <TGUI/TGUI.hpp>
#include "Prerequisites.hpp"

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
		void AddToolbox( tgui::Widget::Ptr _box );

		/// \scroll while mouse is down on one of the two buttons.
		void Update( float _dt );

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
		Core::MapID GetSelectedMap() const		{ return m_selected; }
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

		// Create all elements in the map toolbox.
		BrushToolbox();

		virtual void Init() override;
	};

	/// \brief Toolbox for map selection and creation.
	class PlayersToolbox: public Toolbox
	{
	public:
		typedef tgui::SharedWidgetPtr<PlayersToolbox> Ptr;

		// Create all elements in the map toolbox.
		PlayersToolbox();

		virtual void Init() override;
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