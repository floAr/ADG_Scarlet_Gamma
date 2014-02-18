#include "states/PromptState.hpp"
#include "Game.hpp"
#include "utils/Content.hpp"
#include "sfutils/View.hpp"

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

void PromptState::setMessage(std::string& message)
{
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
	switch (key.code)
	{
	case sf::Keyboard::Return:
		if (m_editBox->getText().isEmpty())
		{
			m_editBox->focus();
		}
		else
		{
			if(m_callbacks.size()==0){//Only finish on enter when there is no button 
				m_result = m_editBox->getText();
				m_finished = true;
			}
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
	DismissableDialogState::Resize(_size);
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


void PromptState::ConfigurePromp(const std::string _message,const bool _textInputRequired){
	if(!_textInputRequired)
		m_editBox->hide();
	tgui::Label::Ptr message = m_gui.get("Message");
	message->setText(_message);

}

void PromptState::AddButton(const std::string _buttons,std::function<void(std::string)> _callback,const sf::Vector2f _position){
	int bID=m_callbacks.size();
	tgui::Button::Ptr button = m_defaultButton.clone();
	m_gui.add(button);
	button->setPosition(_position);
	button->setText(_buttons);
	button->setCallbackId(bID);
	button->bindCallback(tgui::Button::LeftMouseClicked);
	m_callbacks[bID]=_callback;
}


void PromptState::GuiCallback(tgui::Callback&  args){
	if(!m_callbacks.count(args.id))//no callback
		return;
	auto cb=(m_callbacks[args.id]);
	if(m_editBox->isVisible()){
		m_result=m_editBox->getText();
		cb(m_result);

	}
	else{
		m_result="";
		cb("");
	}
	m_finished=true;
	//	callback=(void*)m_callbacks[args.id];

}