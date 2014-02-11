#pragma once

#include "Prerequisites.hpp"
#include "states/CommonState.hpp"
#include "core/ObjectList.hpp"
#include "interfaces/PropertyPanel.hpp"
#include "interfaces/ObjectPanel.hpp"
#include "interfaces/Toolbar.hpp"
#include "tools/brush.hpp"

namespace States
{
	class MasterState : public CommonState
	{
	public:
		MasterState( const std::string& _loadFile );
		virtual void OnBegin() override;
		virtual void OnEnd() override;
		virtual void Update(float dt) override;
		virtual void Draw(sf::RenderWindow& win) override;
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos,
			bool guiHandled) override;
		virtual void MouseButtonReleased(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos,
			float time, bool guiHandled) override;
		/// \brief Update gui elements + common state actions
		virtual void MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled) override;
		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override;
		virtual void Resize(const sf::Vector2f& _size) override;

		/// \brief CommonState::MouseMoved + action depending on the mode (select / paint)
		virtual void MouseMoved(int deltaX, int deltaY, bool guiHandled) override;

		virtual Core::Map* GetCurrentMap() override;

		/// \brief Check if a layer is currently visible
		bool IsLayerVisible( int _layer )	{ return m_hiddenLayers[_layer] != 0; }

		virtual void BeginCombat(Core::ObjectID _object) override;

	private:
		const std::string m_worldFileName;	///< This is the name of the world which was loaded in constructor

		/// \brief Object builder attribute template panel
		Interfaces::PropertyPanel::Ptr m_propertyPanel;
		/// \brief Object builder modules panel
		Interfaces::ObjectPanel::Ptr m_modulePanel;
		/// \brief Object builder objects panel
		Interfaces::ObjectPanel::Ptr m_objectsPanel;
		/// \brief Panel used to show either a module or an object to be edited.
		Interfaces::PropertyPanel::Ptr m_viewPanel;
		/// \brief Showing all (common) properties of a selection.
		Interfaces::PropertyPanel::Ptr m_selectionView;
		/// \brief Editor toolbar on top
		Interfaces::Toolbar::Ptr m_toolbar;
		Interfaces::MapToolbox::Ptr m_mapTool;
		Interfaces::ModeToolbox::Ptr m_modeTool;
		Interfaces::PlayersToolbox::Ptr m_playerTool;

		/// \brief A permanent brush object.
		/// \details Brushes contain an array for flags. So over the whole
		///		process of one mouse move it must exist permanent. Further
		///		creating objects on mouse down and deleting them on mouse
		///		up would be unsafe.
		Tools::Brush m_brush;

		/// \brief Some components fill this with content if mouse is pushed.
		///		On release an according action should be done.
		Interfaces::DragContent* m_draggedContent;

		/// \brief Helper to blend the different layers on and off
		void BlendLayer(int layerId);

		sf::Vector2f m_rectSelectionStart;	///< Starting point where the DM began selection
		bool m_rectSelection;

		bool m_firstLayerSelection;			///< bool to indicate if this is the first layer selected with alt (to hide the rest)
		std::vector<char> m_hiddenLayers;	///< List of currently hidden layers

		void TestButtonCallback(std::string feedback);
	};
}