#pragma once

#include "Prerequisites.hpp"
#include "states/CommonState.hpp"
#include "core/ObjectList.hpp"
#include "interfaces/PropertyPanel.hpp"
#include "interfaces/ObjectPanel.hpp"

namespace States
{
	class MasterState : public CommonState
	{
	public:
		MasterState( const std::string& _loadFile );
		virtual void OnBegin();
		virtual void OnEnd();
		virtual void OnPause() {}
		virtual void OnResume() {}
		virtual void Update(float dt);
		virtual void Draw(sf::RenderWindow& win);
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos);
		virtual void MouseButtonReleased(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, float time);

	private:
		Core::Object* m_player;			///< The one and only player object

		Core::World* m_dbProperties;	///< A container with one object containing property templates.
		Core::World* m_dbModules;		///< A container with objects interpreted as modules.
		Core::World* m_dbTemplates;		///< A container with object templates

		/// \brief Object builder attribute template panel
		Interfaces::PropertyPanel::Ptr m_propertyPanel;
		/// \brief Object builder modules panel
		Interfaces::ObjectPanel::Ptr m_modulePanel;
		/// \brief Object builder objects panel
		Interfaces::ObjectPanel::Ptr m_objectsPanel;

		/// \brief Some components fill this with content if mouse is pushed.
		///		On release an according action should be done.
		Interfaces::DragContent* m_draggedContent;

		/// \brief Creates the initial property database in case of a
		///		loading failure.
		void CreateDefaultPropertyBase();
		/// \brief Creates the initial module database in case of a
		///		loading failure.
		void CreateDefaultModuleBase();
		/// \brief Creates the initial template database in case of a
		///		loading failure.
		void CreateDefaultTemplateBase();
	};
}