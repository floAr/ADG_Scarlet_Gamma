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
    m_editBox = m_gui.get("EnterText");
    SetGui(&m_gui);
}

void PromptState::OnBegin()
{
    m_editBox->setText("");
    m_editBox->focus();
}

void PromptState::setMessage(std::string& message)
{
}

void PromptState::Draw(sf::RenderWindow& win)
{
    // Draw previous state and apply blur shader
    // TODO: separate shader if the FPS get low here
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
    sf::Vector2u size = g_Game->GetWindow().getSize();
    Resize(sf::Vector2f((float) size.x, (float) size.y));
    GameState::Draw(win);
}

void PromptState::Update(float dt)
{
    m_previousState->Update(dt);
}

void PromptState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
    switch (key.code)
    {
    case sf::Keyboard::Return:
        if (m_editBox->getText().isEmpty())
        {
            m_editBox->focus();
        }
        else
        {
            m_result = m_editBox->getText();
            m_finished = true;
        }
        break;

    case sf::Keyboard::Escape:
        if (m_editBox->isFocused())
        {
            // unfocus edit box
            m_editBox->unfocus();
        }
        else
        {
            m_result = "";
            m_finished = true;
        }
        break;
    }
}

void PromptState::Resize(const sf::Vector2f& _size)
{
    // Adjust edit box size
    auto editBoxPtr = m_editBox.get();
    editBoxPtr->setSize(_size.x - 2 * editBoxPtr->getPosition().x, editBoxPtr->getSize().y);
}

void PromptState::SetText(const std::string& text)
{
    tgui::Label::Ptr message = m_gui.get("Message");
    message->setText(text);
}

void PromptState::SetDefaultValue(const std::string& value)
{
    m_editBox->setText(value);
}

const std::string& PromptState::GetResult()
{
    return m_result;
}
