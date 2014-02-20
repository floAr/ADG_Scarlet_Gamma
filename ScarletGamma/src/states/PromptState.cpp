#include "states/PromptState.hpp"
#include "Game.hpp"
#include "states/StateMachine.hpp"
#include "utils/Content.hpp"
#include "sfutils/View.hpp"
#include "utils/ValueInterpreter.hpp"
#include "utils/Exception.hpp"

using namespace States;

PromptState::PromptState()
	: m_result(nullptr)
{
	m_defaultButton->load("lib/TGUI-0.6-RC/widgets/Black.conf");
	// Load shader from file
	//m_shader=Content::Instance()->LoadShader("media/Prompt.frag",sf::Shader::Type::Fragment);
	m_orb=sf::Sprite(Content::Instance()->LoadTexture("media/orb_prompt.png"));
	float orbScale=DismissableDialogState::ORB_WIDTH/m_orb.getLocalBounds().width;
	m_orb.setScale(sf::Vector2f(orbScale,orbScale));

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

void PromptState::Draw(sf::RenderWindow& win)
{
	DismissableDialogState::Draw(win);

	// Draw GUI
	//sf::Vector2u size = g_Game->GetWindow().getSize();
	//Resize(sf::Vector2f((float) size.x, (float) size.y));
	//GameState::Draw(win);
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
	tgui::Label::Ptr message = m_gui.get("Message");
	message->setPosition(_size.x / 2.0f - message->getSize().x / 2.0f,
		message->getPosition().y);

	// Adjust edit box size and position
	auto editBoxPtr = m_editBox.get();
	editBoxPtr->setSize(std::min(920.f, _size.x - 20), editBoxPtr->getSize().y);
	editBoxPtr->setPosition(_size.x / 2.0f - editBoxPtr->getSize().x / 2.0f, editBoxPtr->getPosition().y);

	// Adjust button position
	if (m_buttons.size() > 0)
	{
		float buttonTop = m_editBox->isVisible() ?
			m_editBox->getPosition().y + m_editBox->getSize().y + 20 :
			m_editBox->getPosition().y;

		// Pre-calculations for button positions
		float winWidth = _size.x;
		int spacing = 10;
		float totalBtnWidth = m_buttons.at(0).button->getSize().x *
			m_buttons.size() + spacing * (m_buttons.size() - 1);

		// Reposition all buttons
		for (size_t i = 0; i < m_buttons.size(); ++i)
		{
			m_buttons.at(i).button->setPosition(winWidth / 2.0f - totalBtnWidth / 2.0f
				+ m_buttons.at(0).button->getSize().x * i + spacing * i, buttonTop);
		}
	}
}

void PromptState::SetText(const std::string& _text)
{
	tgui::Label::Ptr message = m_gui.get("Message");
	message->setAutoSize(true);
	message->setText(_text);
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
	if(!m_forceKeepAlive)
		m_finished = true;
	else
	{
		if (m_result != nullptr) // also handle button here to emit event
			m_result->function(m_editBox->getText());
		if(m_isMinimizeable)
			SetMinimized(true);
	}
}

