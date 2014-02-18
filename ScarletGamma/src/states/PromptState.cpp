#include "states/PromptState.hpp"
#include "Game.hpp"
#include "states/StateMachine.hpp"
#include "utils/Content.hpp"
#include "sfutils/View.hpp"
#include "utils/ValueInterpreter.hpp"

using namespace States;

PromptState::PromptState()
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

	// Draw previous state and apply blur shader
	// TODO: separate shader if the FPS get low here
//	m_previousState->Draw(win);
	

	// Draw GUI
	sf::Vector2u size = g_Game->GetWindow().getSize();
	Resize(sf::Vector2f((float) size.x, (float) size.y));
	//GameState::Draw(win);
}

void PromptState::Update(float dt)
{
	DismissableDialogState::Update(dt);
}

void PromptState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
	if (key.code == sf::Keyboard::Return && m_editBox->isVisible() && m_editBox->getText().isEmpty())
	{
		// Handle default Return, with priority
		m_editBox->focus();
	}
	else if (key.code == sf::Keyboard::Return && m_callbacks.size() == 0)
	{
		m_finished = true;
	}
	else if (key.code == sf::Keyboard::Escape && m_callbacks.size() == 0)
	{
		m_editBox->setText("");
		m_finished = true;
	}
	else if (key.code != sf::Keyboard::Unknown)
	{
		// Find callback if nothing was handled
		for (size_t i = 0; i < m_callbacks.size(); ++i)
		{
			if (key.code == m_callbacks.at(i).hotkey)
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
	// Adjust edit box size
	auto editBoxPtr = m_editBox.get();
	editBoxPtr->setSize(_size.x - 2 * editBoxPtr->getPosition().x, editBoxPtr->getSize().y);
}

void PromptState::SetText(const std::string& _text)
{
	tgui::Label::Ptr message = m_gui.get("Message");
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

bool States::PromptState::CheckEvaluate(Core::Object* _object) const
{
	try
	{
		Utils::EvaluateFormula(m_editBox->getText(), g_Game->RANDOM, _object);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

const std::string PromptState::GetResult()
{
	return m_editBox->getText();
}

void PromptState::AddButton(const std::string _buttonText, std::function<void(std::string)> _callback,
							sf::Keyboard::Key _hotkey, Core::Object* _evaluateObj)
{
	int bID = m_callbacks.size();

	tgui::Button::Ptr button = m_defaultButton.clone();
	m_gui.add(button);

	button->setText(_buttonText);
	button->setCallbackId(bID);
	button->bindCallback(tgui::Button::LeftMouseClicked);
	m_callbacks.emplace(bID, PromptButton(_callback, _evaluateObj, _hotkey));
}

void PromptState::GuiCallback(tgui::Callback& args)
{
	if (!m_callbacks.count(args.id)) // no callback
		return;

	auto cb = m_callbacks.at(args.id);

	// Evaluation required, but failed?
	if (cb.evaluateObj != 0 && !CheckEvaluate(cb.evaluateObj))
	{
		PromptState* prompt = static_cast<PromptState*>(g_Game->GetStateMachine()->PushGameState(GST_PROMPT));
		prompt->SetText("Bitte überprüfe den eingegebenen Wert.\n");
		prompt->SetTextInputRequired(false);
	}
	else
	{
		if (m_editBox->isVisible())
		{
			cb.function(m_editBox->getText());
		}
		else
		{
			m_editBox->setText(""); // Required if someone uses global PopCallback
			cb.function("");
		}

		m_finished = true;
	}
}

