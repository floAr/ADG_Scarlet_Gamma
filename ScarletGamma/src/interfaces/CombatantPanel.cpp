#include "interfaces/CombatantPanel.hpp"
#include "Game.hpp"
#include "states/CommonState.hpp"
#include "gamerules/Combat.hpp"
#include "utils/Content.hpp"
#include "network/Messenger.hpp"

using Interfaces::CombatantPanel;


Interfaces::CombatantPanel::CombatantPanel()
{
	setSize(0, 240);

	// Button (next turn)
	m_roundDoneBtn = tgui::Button::Ptr(*this);
	m_roundDoneBtn->load("media/Black.conf");
	m_roundDoneBtn->setTextSize(16);
	// doesn't get the global font for some reason
	m_roundDoneBtn->setTextFont(Content::Instance()->LoadFont("media/arial.ttf"));
	m_roundDoneBtn->bindCallbackEx(&CombatantPanel::RoundDoneBtnClicked, this, tgui::Button::LeftMouseClicked);
	m_roundDoneBtn->setText("Runde beenden");

	// List of combatants
	m_combatantList = tgui::ListBox::Ptr(*this);
	m_combatantList->load("media/Black.conf");
	m_combatantList->setItemHeight( 19 );
	m_combatantList->disable();

	Resize(sf::Vector2f((float) g_Game->GetWindow().getSize().x,
						(float) g_Game->GetWindow().getSize().y));
}

void Interfaces::CombatantPanel::RoundDoneBtnClicked(const tgui::Callback& _call)
{
	// Skip turn, if the object belongs to me
	States::CommonState* common = g_Game->GetCommonState();
	if (common->InCombat())
	{
		GameRules::Combat* combat = common->GetCombat();
		if ( combat->HasStarted() && Network::Messenger::IsServer() || common->OwnsObject(combat->GetTurn()) )
			combat->EndTurn();
	}
}

void Interfaces::CombatantPanel::UpdateCombatants( std::list<Core::ObjectID>& m_participants )
{
	m_combatantList->removeAllItems();
	for (Core::ObjectID id: m_participants)
		m_combatantList->addItem(g_Game->GetWorld()->GetObject(id)->GetName());
	m_combatantList->deselectItem();
}

void Interfaces::CombatantPanel::SetTurn( int _id )
{
	m_combatantList->setSelectedItem( _id );
}

void Interfaces::CombatantPanel::Resize( sf::Vector2f _size, float _width )
{
	if (_width == 0)
		_width = getSize().x;

	setSize(_width, getSize().y);
	setPosition(0, _size.y - getSize().y);
	
	float btnHeight = 30;
	m_combatantList->setPosition(0, 0);
	m_combatantList->setSize(getSize().x, getSize().y - btnHeight);
	m_roundDoneBtn->setSize(getSize().x, btnHeight);
	m_roundDoneBtn->setPosition(0, getSize().y - btnHeight);
}
