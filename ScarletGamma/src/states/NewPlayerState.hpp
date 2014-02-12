#pragma once

#include "GameState.hpp"
#include <TGUI/TGUI.hpp>
#include "Prerequisites.hpp"
#include "utils/Random.hpp"

namespace States
{
	class NewPlayerState : public GameState
	{
	public:
		/// \brief Creating a new player also sets the name in the given edit
		///		to communicate the new name.
		NewPlayerState(tgui::EditBox::Ptr _nameEdit, Core::ObjectID* _saveID);
		virtual void Draw(sf::RenderWindow& win) override;
		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override;
	private:
		void Create();		///< Button create method (copies data)
		void CreateNew();	///< Create a new empty player. Called if given ID = 0xffffffff
		void ShowPlayer();	///< Copy the values from the player to the GUI
		void AttributeStrengthChanged();
		void AttributeDexterityChanged();
		void AttributeConstitutionChanged();
		void AttributeIntelligenceChanged();
		void AttributeWisdomChanged();
		void AttributeCharismaChanged();

		sf::Font	m_menuFont;
		tgui::Gui   m_gui;
		Utils::Random m_rand;

		/// \brief Output of the name to the parent state
		tgui::EditBox::Ptr m_nameOutputEdit;
		/// \brief Output of the id to the parent state
		Core::ObjectID* m_newPlayerID;
		Core::Object * m_newPlayer;

		// All the components which contain data
		tgui::EditBox::Ptr m_name;
		tgui::ComboBox::Ptr m_cAttitude;
		tgui::EditBox::Ptr m_eFaith;
		tgui::EditBox::Ptr m_eClass;
		tgui::EditBox::Ptr m_eFolk;
		tgui::EditBox::Ptr m_eHome;
		tgui::EditBox::Ptr m_eSize;
		tgui::EditBox::Ptr m_eAge;
		tgui::ComboBox::Ptr m_cSex;
		tgui::EditBox::Ptr m_eWeight;
		tgui::EditBox::Ptr m_eHair;
		tgui::EditBox::Ptr m_eEye;
		tgui::EditBox::Ptr m_eSt;
		tgui::EditBox::Ptr m_eStMod;
		tgui::EditBox::Ptr m_eGe;
		tgui::EditBox::Ptr m_eGeMod;
		tgui::EditBox::Ptr m_eKo;
		tgui::EditBox::Ptr m_eKoMod;
		tgui::EditBox::Ptr m_eIn;
		tgui::EditBox::Ptr m_eInMod;
		tgui::EditBox::Ptr m_eWe;
		tgui::EditBox::Ptr m_eWeMod;
		tgui::EditBox::Ptr m_eCh;
		tgui::EditBox::Ptr m_eChMod;
		tgui::EditBox::Ptr m_eTP;
		tgui::EditBox::Ptr m_eTPMax;
		tgui::EditBox::Ptr m_eRK;
	};
}