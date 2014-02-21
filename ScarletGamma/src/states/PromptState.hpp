#pragma once

#include "states/GameState.hpp"
#include "states/DismissableDialogState.hpp"
#include "Prerequisites.hpp"
#include <unordered_map>

namespace States
{
	/// \brief Universal state for displaying a message and an editbox to the
	///   user, prompting for a value.
	class PromptState : public DismissableDialogState
	{
		typedef void (*callback)(std::string);
	public:
		/// \brief Creates an empty, unconfigured prompt state
		PromptState();

		void SetText(const std::string& _text);
		void SetDefaultValue(const std::string& _value);
		void SetTextInputRequired(bool _value);
		const std::string GetResult();

		void OnBegin() override;
		void OnEnd() override;
		virtual void Draw(sf::RenderWindow& win) override;
		virtual void Update(float dt) override;
		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override;
		virtual void Resize(const sf::Vector2f& _size) override;

		virtual void GuiCallback(tgui::Callback&  args) override;

		/// \brief Adds a button to the Prompt state.
		/// \brief _buttonText  [in]  Caption of the button
		/// \brief _callback    [in]  Function that is called when the button is pressed
		/// \brief _evaluateObj [in]  Object used for evaluation check, or 0 for none
		void AddButton(const std::string _buttonText, std::function<void(std::string)> _callback,
			sf::Keyboard::Key _hotkey = sf::Keyboard::Unknown, Core::Object* _evaluateObj = 0);

	private:

		struct PromptButton
		{
			PromptButton(tgui::Button::Ptr _button, std::function<void(std::string)> _function,
				Core::Object* _evaluateObj = 0, sf::Keyboard::Key _hotkey = sf::Keyboard::Unknown)
				: button(_button), function(_function), evaluateObj(_evaluateObj), hotkey(_hotkey) {}

			tgui::Button::Ptr				 button;
			std::function<void(std::string)> function;
			Core::Object*					 evaluateObj;
			sf::Keyboard::Key				 hotkey;
		};

		tgui::Button::Ptr m_defaultButton;	///< This is a preloaded button to increase RecalculateGUI performance by a height factor
		tgui::Gui m_gui;
		tgui::EditBox::Ptr m_editBox;
		tgui::Label::Ptr m_message;
		PromptButton* m_result;

		std::unordered_map<int, PromptButton> m_buttons;

		
	};
}