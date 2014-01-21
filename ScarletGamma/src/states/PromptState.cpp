#include "states/PromptState.hpp"
#include "Game.hpp"
#include "utils/Content.hpp"
#include "sfutils/View.hpp"

using namespace States;

PromptState::PromptState()
{
    // Load shader from file
    // TODO: include shaders in content manager?
    m_shader.loadFromFile("media/Prompt.frag", sf::Shader::Fragment);

    // Create GUI
    m_gui.setWindow(g_Game->GetWindow());
    m_gui.setGlobalFont( Content::Instance()->LoadFont("media/arial.ttf") );
    m_gui.loadWidgetsFromFile("media/Prompt.gui");
    tgui::EditBox::Ptr enterTextEdit = m_gui.get( "EnterText" );
    SetGui(&m_gui);
}

void PromptState::setMessage(std::string& message)
{
}

void PromptState::Draw(sf::RenderWindow& win)
{
    // Draw previous state and apply blur shader
    m_previousState->Draw(win);
    sf::View& backup = sfUtils::View::SetDefault(&win);
    sf::Texture screenBuffer;
    screenBuffer.create(win.getSize().x, win.getSize().y);
    screenBuffer.update(win);
    m_shader.setParameter("texture", sf::Shader::CurrentTexture);
    m_shader.setParameter("blur_radius_x", 1.0f / (float) win.getSize().x);
    m_shader.setParameter("blur_radius_y", 1.0f / (float) win.getSize().y);
    win.draw(sf::Sprite(screenBuffer), &m_shader);
    win.setView(backup);

    // Draw GUI
    GameState::Draw(win);
}

void PromptState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
    switch (key.code)
    {
    case sf::Keyboard::Return:
        // TODO accept value
        //Accept();
        break;
    case sf::Keyboard::Escape:
        // TODO reject value
        //Cancel();
        break;
    }
}