#include "states/PromptState.hpp"
#include "Game.hpp"
#include "states/StateMachine.hpp"
#include "utils/Content.hpp"
#include "sfutils/View.hpp"
#include "utils/ValueInterpreter.hpp"
#include "utils/Exception.hpp"

using namespace States;

PromptState::PromptState() :
	DismissableDialogState( "media/orb_prompt.png" ),
	m_result(nullptr),
	m_editBox(nullptr),
	m_message(nullptr)
{
	m_defaultButton->load("lib/TGUI-0.6-RC/widgets/Black.conf");

	// Create GUI
	m_gui.setWindow(g_Game->GetWindow());
	m_gui.setGlobalFont( Content::Instance()->LoadFont("media/arial.ttf") );
	m_editBox = tgui::EditBox::Ptr(m_gui);
	m_editBox->load("lib/TGUI-0.6-RC/widgets/Black.conf");
	SetGui(&m_gui);
}

void PromptState::OnBegin()
{
	m_editBox->setText("");
	m_editBox->focus();
}

void PromptState::Draw(sf::RenderWindow& win)
{
	DismissableDialogState::Draw(win);
}

void PromptState::Update(float dt)
{
	DismissableDialogState::Update(dt);
}

void PromptState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
	DismissableDialogState::KeyPressed(key,guiHandled);

	if (key.code == sf::Keyboard::Return && m_editBox->isVisible() && m_editBox->getText().isEmpty())
	{
		// Handle default Return, with priority
		m_editBox->focus();
	}
	else if (key.code == sf::Keyboard::Return && m_buttons.size() == 0)
	{
		if(!m_forceKeepAlive)
			m_finished = true;
		else
			if(m_isMinimizeable)
				SetMinimized(true);
	}
	else if (key.code == sf::Keyboard::Escape && m_buttons.size() == 0)
	{
		m_editBox->setText("");
		if(!m_forceKeepAlive)
			m_finished = true;
		else
			if(m_isMinimizeable)
				SetMinimized(true);
	}
	else if (key.code != sf::Keyboard::Unknown)
	{
		// Find callback if nothing was handled
		for (size_t i = 0; i < m_buttons.size(); ++i)
		{
			if (key.code == m_buttons.at(i).hotkey)
			{
				// Fake a TGUI callback, needs only the ID
				tgui::Callback cb;
				cb.id = i;
				GuiCallback(cb);
			}
		}
	}
}

void PromptState::Resize(const sf::Vector2f& _size)
{
	DismissableDialogState::Resize(_size);

	// Adjust text position
	m_message->setPosition(_size.x / 2.0f - m_message->getSize().x / 2.0f,
		_size.y * 0.4f);

	// Adjust button position
	float totalBtnWidth = 350.0f;
	if (m_buttons.size() > 0)
	{
		float buttonTop = m_editBox->isVisible() ?
			_size.y * 0.4f + 135.0f : _size.y * 0.4f + 75.0f;

		// Pre-calculations for button positions
		float winWidth = _size.x;
		int spacing = 10;
		totalBtnWidth = m_buttons.at(0).button->getSize().x *
			m_buttons.size() + spacing * (m_buttons.size() - 1);

		// Reposition all buttons
		for (size_t i = 0; i < m_buttons.size(); ++i)
		{
			m_buttons.at(i).button->setPosition(winWidth / 2.0f - totalBtnWidth / 2.0f
				+ m_buttons.at(0).button->getSize().x * i + spacing * i, buttonTop);
		}
	}

	// Adjust edit box size and position
	m_editBox->setSize(std::min(std::max(350.0f, totalBtnWidth), _size.x - 20), 40.0f);
	m_editBox->setPosition(_size.x / 2.0f - m_editBox->getSize().x / 2.0f, _size.y * 0.4f + 75.0f);
}

void PromptState::SetText(const std::string& _text)
{
	m_message = tgui::Label::Ptr(m_gui);
	m_message->setTextSize(28);
	m_message->setTextColor( sf::Color(200,200,200) );
	m_message->setAutoSize(true);
	m_message->setText(_text);
	// Make sure 'g' ... are not cut
	m_message->setSize( m_message->getSize().x, 40.0f );
}

void PromptState::SetDefaultValue(const std::string& _value)
{
	m_editBox->setText(_value);
}

void States::PromptState::SetTextInputRequired(bool _value)
{
	if(_value)
		m_editBox->show();
	else
		m_editBox->hide();
}


void States::PromptState::OnEnd()
{
	// Call callback if we have one
	if (m_result != nullptr)
		m_result->function(m_editBox->getText());
}

const std::string PromptState::GetResult()
{
	return m_editBox->getText();
}

void PromptState::AddButton(const std::string _buttonText, std::function<void(std::string)> _callback,
							sf::Keyboard::Key _hotkey, Core::Object* _evaluateObj)
{
	int bID = m_buttons.size();

	tgui::Button::Ptr button = m_defaultButton.clone();
	m_gui.add(button);
	button->setText(_buttonText);
	button->setCallbackId(bID);
	button->bindCallback(tgui::Button::LeftMouseClicked);
	m_buttons.emplace(bID, PromptButton(button, _callback, _evaluateObj, _hotkey));

	Resize(sf::Vector2f((float) g_Game->GetWindow().getSize().x,
		(float) g_Game->GetWindow().getSize().y));
}

void PromptState::GuiCallback(tgui::Callback& args)
{
	if (!m_buttons.count(args.id)) // no callback
		return;

	PromptButton& cb = m_buttons.at(args.id);

	// Evaluation required, but failed?
	if (cb.evaluateObj != 0)
	{
		try
        {
            Utils::EvaluateFormula(m_editBox->getText(), g_Game->RANDOM, cb.evaluateObj);
        }
		catch( Exception::InvalidFormula _e )
        {
			PromptState* prompt = static_cast<PromptState*>(g_Game->GetStateMachine()->PushGameState(GST_PROMPT));
			prompt->SetText("Bitte überprüfe den eingegebenen Wert.\nFehler: " + _e.to_string());
			prompt->Resize((sf::Vector2f)g_Game->GetWindow().getSize());
			prompt->DisableMinimize();
			prompt->SetTextInputRequired(false);
            return;
		}
	}

    // If formula evaluation failed, the function will have returned by now

	// Set result to callback, so OnEnd can handle it
	m_result = &m_buttons.at(args.id);

	if (!m_editBox->isVisible())
		m_editBox->setText(""); // Required if someone uses global PopCallback
	if(!m_forceKeepAlive) {
		m_finished = true;
		// The current state might not be on top -> remove it from Orb-bar manually
		SetMinimized( true );
		RemoveOrb( &m_orb );
	} else
	{
		if (m_result != nullptr) // also handle button here to emit event
			m_result->function(m_editBox->getText());
		if(m_isMinimizeable)
			SetMinimized(true);
	}
}

