#pragma once

#include "states/GameState.hpp"

namespace States
{
    class PromptState : public GameState
    {
    public:
        PromptState();
        void setMessage(std::string& message);
        void SetText(const std::string& text);
        const std::string& GetResult();

        void OnBegin() override;
        virtual void Draw(sf::RenderWindow& win) override;
        virtual void Update(float dt) override;
        virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override;
    private:
        tgui::Gui m_gui;
        tgui::EditBox::Ptr m_editBox;
        std::string m_result;
        sf::Shader m_shader;
    };
}