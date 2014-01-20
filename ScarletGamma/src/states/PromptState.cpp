#include "states/PromptState.hpp"

using namespace States;

void PromptState::Draw(sf::RenderWindow& win)
{
    // TODO: Color blend for testing, remove
    unsigned char tone = (unsigned char) (m_counter/2.0f * 255);
    win.clear(sf::Color(tone, tone , tone , 255));
}

void PromptState::Update(float dt)
{
    // TODO: Counter for testing, remove
    m_counter += dt;
    if (m_counter >= 2)
        m_finished = true;
}