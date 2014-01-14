#pragma once

#include "Prerequisites.hpp"
#include "states/CommonState.hpp"
#include "states/SelectionState.hpp"

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

	};
}