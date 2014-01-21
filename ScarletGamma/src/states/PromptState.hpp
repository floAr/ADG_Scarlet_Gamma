#pragma once

#include "states/GameState.hpp"

namespace States
{
    class PromptState : public GameState
    {
    public:
        PromptState();
        void setMessage(std::string& message);
        void GuiAccept(tgui::Callback& callback);
        void GuiCancel(tgui::Callback& callback);
        virtual void Draw(sf::RenderWindow& win) override;
        virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override;
    private:
        tgui::Gui m_gui;
        sf::Shader m_shader;
    };
}