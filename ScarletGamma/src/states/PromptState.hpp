#pragma once

#include "states/GameState.hpp"
#include <unordered_map>

namespace States
{
    /// \brief Universal state for displaying a message and an editbox to the
    ///   user, prompting for a value.
    class PromptState : public GameState
    {
		typedef void (*callback)(std::string);
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
        virtual void Resize(const sf::Vector2f& _size) override;

			virtual void GuiCallback(tgui::Callback&  args) override;

		void ConfigurePromp(const std::string _message,const bool _textInputRequired);
		void AddButton(const std::string _buttonText,std::function<void(std::string)> _callback);
    private:
		tgui::Button::Ptr m_defaultButton;	///< This is a preloaded button to increase RecalculateGUI performance by a height factor
        tgui::Gui m_gui;
        tgui::EditBox::Ptr m_editBox;
        std::string m_result;
        sf::Shader m_shader;
		
		std::unordered_map<int,std::function<void(std::string)>> m_callbacks;
    };
}