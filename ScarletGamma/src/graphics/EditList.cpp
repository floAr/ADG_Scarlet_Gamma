#include "EditList.hpp"
#include "SFML\System\Vector2.hpp"
#include "TGUI\EditBox.hpp"

namespace Graphics {

	EditList::EditList( tgui::Gui& _gui, float _x, float _y, float _w, float _h,
		bool _addNdel, bool _leftEditable, bool _rightEditable ) :
	m_gui(_gui),
	m_list(_gui),
	m_newName(),
	m_newValue(),
	m_addNdel(_addNdel),
	m_leftEditable(_leftEditable),
	m_rightEditable(_rightEditable),
	m_oldScrollValue(0)
{
	m_list->setPosition(_x, _y);
	m_list->setSize(_w, _h - (m_addNdel ? 20.0f : 0.0f));
	m_list->setBackgroundColor( sf::Color(50,50,50,150) );

	// Create a scrollbar for long lists.
	m_scrollBar = tgui::Scrollbar::Ptr( dynamic_cast<tgui::Container&>(*m_list) );
	m_scrollBar->load("media/Black.conf");
	m_scrollBar->setAutoHide(false);
	m_scrollBar->setLowValue(unsigned(m_list->getSize().y/20.0f));
	m_scrollBar->setSize(12.0f, m_list->getSize().y);
	m_scrollBar->bindCallbackEx( &EditList::Scroll, this, tgui::Scrollbar::ValueChanged );

	// Add an edit which creates a new line if changed.
	if( m_addNdel )
	{
		float w = m_list->getSize().x * 0.5f - 20.0f;
		m_newName = tgui::EditBox::Ptr(_gui);
		m_newName->load("media/Black.conf");
		m_newName->setSize(w, 20.0f);
		m_newName->setPosition(_x, _y+m_list->getSize().y);
		m_newName->setText("");
		m_newValue = tgui::EditBox::Ptr(_gui);
		m_newValue->load("media/Black.conf");
		m_newValue->setSize(w, 20.0f);
		m_newValue->setPosition(_x+w, _y+m_list->getSize().y);
		m_newValue->setText("");
		tgui::Button::Ptr newAdd( _gui );
		newAdd->load("media/Black.conf");
		newAdd->setPosition(_x+m_list->getSize().x - 40.0f, _y+m_list->getSize().y);
		newAdd->setSize(20.0f, 20.0f);
		newAdd->setCallbackId( 1 );
		newAdd->bindCallbackEx(&EditList::AddBtn, this, tgui::Button::LeftMouseClicked);
		newAdd->setText("+");
	}
}

void EditList::Add( const std::string& _left, const std::string& _right )
{
	// y coordinate where to insert inside panel
	unsigned line = (m_list->getWidgets().size() - 1) / 3;
	float y = (line - m_scrollBar->getValue()) * 20.0f;
	// Width of a edit
	float w = m_list->getSize().x * 0.5f - (m_addNdel ? 12.0f : 6.0f);

	// Create component on the left side
	tgui::EditBox::Ptr left( dynamic_cast<tgui::Container&>(*m_list) );
	left->load("media/Black.conf");
	left->setSize(w, 20.0f);
	left->setPosition(12.0f, y);
	if(!m_leftEditable) left->disable();
	left->setText(_left);

	// Create the one on the right side
	tgui::EditBox::Ptr right( dynamic_cast<tgui::Container&>(*m_list) );
	right->load("media/Black.conf");
	right->setSize(w, 20.0f);
	right->setPosition(w+12.0f, y);
	if(!m_rightEditable) right->disable();
	right->setText(_right);

	// Create a remove line button
	if( m_addNdel )
	{
		tgui::Checkbox::Ptr del( dynamic_cast<tgui::Container&>(*m_list) );
		del->load("media/Black.conf");
		del->setPosition(m_list->getSize().x - 12.0f, y+4.0f);
		del->setSize(12.0f, 12.0f);
		del->setCallbackId( line );
		del->bindCallbackEx(&EditList::RemoveBtn, this, tgui::Button::LeftMouseClicked);
	}

	m_scrollBar->setMaximum(line+1);
}

void EditList::RemoveBtn(const tgui::Callback& _call)
{
	auto&  list = m_list->getWidgets();
	int delLine = int(_call.widget->getPosition().y / 20.0f);
	
	// First element is always the scrollbar
	for( size_t i=1; i<list.size(); ++i )
	{
		int elemLine = int(list[i]->getPosition().y / 20.0f);
		// Search and destroy
		if( (elemLine == delLine) )
		{
			list.erase( list.begin() + i );
			--i;
		} else if( elemLine > delLine )
		{
			// Move object below that one. Since they are not revisited
			// they are not going to be deleted if they should be in the
			// delLine afterwards
			list[i]->move(0.0f, -20.0f);
		}
	}

	m_scrollBar->setMaximum(m_scrollBar->getMaximum()-1);
}

void EditList::AddBtn( const tgui::Callback& _call )
{
	if( !m_newName->getText().isEmpty() )
	{
		Add( m_newName->getText(), m_newValue->getText() );
		m_newName->setText("");
		m_newValue->setText("");
	}
}

void EditList::Scroll( const tgui::Callback& _call )
{
//	tgui::Scrollbar* bar = (tgui::Scrollbar*)_call.widget;
	int dif = m_oldScrollValue - _call.value;
	m_oldScrollValue = _call.value;
	// Move everything accordingly
	auto&  list = m_list->getWidgets();
	for( size_t i=1; i<list.size(); ++i )
	{
		list[i]->move(0.0f, dif * 20.0f);
	}
}

} // namespace Graphics