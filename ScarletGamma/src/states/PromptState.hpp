#pragma once

#include "states/GameState.hpp"

namespace States
{
    class PromptState : public GameState
    {
    public:
        PromptState() : m_counter(0) { }
        virtual void Draw(sf::RenderWindow& win);
        virtual void Update(float dt);
    private:
        float m_counter;
    };
}