#pragma once

#include "Prerequisites.hpp"
#include "states/CommonState.hpp"
#include "core/ObjectList.hpp"
#include "graphics/PropertyPanel.hpp"

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

	private:
		Core::Object* m_player;		///< The one and only player object

		Core::World* m_database;	///< A container with object templates.

		/// \brief Object builder attribute template panel
		Graphics::PropertyPanel::Ptr m_propertyPanel;
		/// \brief one object which contains all properties.
		Core::Object* m_propertyObject;

		/// \brief Creates the initial template database in case of a
		///		loading failure.
		void CreateDefaultDatabase();
	};
}