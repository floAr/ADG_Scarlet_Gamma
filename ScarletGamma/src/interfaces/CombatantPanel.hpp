#pragma  once

#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include "Prerequisites.hpp"

namespace Interfaces {

	/// \brief A container to show a list of editable values.
	class CombatantPanel : public tgui::Panel
	{
	public:
		typedef tgui::SharedWidgetPtr<CombatantPanel> Ptr;

		CombatantPanel();
		void Resize( sf::Vector2f _size, float _width = 0);

		void UpdateCombatants( std::list<Core::ObjectID>& m_participants );
		void SetTurn( int _id );
	protected:
		void RoundDoneBtnClicked(const tgui::Callback& _call);
		tgui::Button::Ptr  m_roundDoneBtn;
		tgui::ListBox::Ptr m_combatantList;
	};

} // namespace