#pragma once

#include "states/GameState.hpp"

namespace States
{
    /// \brief Universal state for displaying a message and an editbox to the
    ///   user, prompting for a value.
    class PromptState : public GameState
    {
    public:
        /// \brief Creates an empty, unconfigured prompt state
        PromptState();


        void setMessage(std::string& message);
        void SetText(const std::string& text);
        void SetDefaultValue(const std::string& value);
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