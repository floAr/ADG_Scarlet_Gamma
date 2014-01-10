#include "EditList.hpp"
#include "SFML\System\Vector2.hpp"
#include "TGUI\EditBox.hpp"

namespace Graphics {

	EditList::EditList( tgui::Gui& _gui, float _x, float _y, float _w, float _h,
		bool _addNdel, bool _leftEditable, bool _rightEditable, bool _autosize, const std::string& _title ) :
	m_gui(_gui),
	m_list(_gui),
	m_newName(),
	m_newValue(),
	m_addNdel(_addNdel),
	m_leftEditable(_leftEditable),
	m_rightEditable(_rightEditable),
	m_oldScrollValue(0),
	m_numLines(0)
{
	m_list->setPosition(_x, _y + 20.0f);
	m_list->setSize(_w, _h - (m_addNdel ? 40.0f : 20.0f));
	m_list->setBackgroundColor( sf::Color(50,50,50,150) );

	// Create a scrollbar for long lists.
	m_scrollBar = tgui::Scrollbar::Ptr( dynamic_cast<tgui::Container&>(*m_list) );
	m_scrollBar->load("media/Black.conf");
	m_scrollBar->setAutoHide(true);
	m_scrollBar->setLowValue(unsigned(m_list->getSize().y/20.0f));
	m_scrollBar->setSize(12.0f, m_list->getSize().y);
	m_scrollBar->setMaximum(0);
	m_scrollBar->bindCallbackEx( &EditList::Scroll, this, tgui::Scrollbar::ValueChanged );

	// The whole component can be minimized..
	tgui::EditBox::Ptr title( m_gui );
	title->load("media/Black.conf");
	title->setSize(_w, 20.0f);
	title->setPosition(_x, _y);
	title->disable();
	title->setText("    " + _title);
	tgui::AnimatedPicture::Ptr minimize(m_gui);
	minimize->setPosition(_x+4.0f, _y+4.0f);
	minimize->addFrame("media/Black_ArrowRight.png");
	minimize->addFrame("media/Black_ArrowDown.png");
	minimize->setFrame(1);
	minimize->setCallbackId(0);
	minimize->setSize(12.0f, 12.0f);
	minimize->bindCallbackEx(&EditList::MiniMaxi, this, tgui::AnimatedPicture::LeftMouseClicked);

	// Add an edit which creates a new line if changed.
	if( m_addNdel )
	{
		float w = m_list->getSize().x * 0.5f - 20.0f;
		m_newName = tgui::EditBox::Ptr(_gui);
		m_newName->load("media/Black.conf");
		m_newName->setSize(w, 20.0f);
		m_newName->setPosition(_x, _y+m_list->getSize().y+20.0f);
		m_newName->setText("");
		m_newValue = tgui::EditBox::Ptr(_gui);
		m_newValue->load("media/Black.conf");
		m_newValue->setSize(w, 20.0f);
		m_newValue->setPosition(_x+w, _y+m_list->getSize().y+20.0f);
		m_newValue->setText("");
		m_newAdd = tgui::Button::Ptr( _gui );
		m_newAdd->load("media/Black.conf");
		m_newAdd->setPosition(_x+m_list->getSize().x - 40.0f, _y+m_list->getSize().y+20.0f);
		m_newAdd->setSize(20.0f, 20.0f);
		m_newAdd->setCallbackId( 1 );
		m_newAdd->bindCallbackEx(&EditList::AddBtn, this, tgui::Button::LeftMouseClicked);
		m_newAdd->setText("+");
	}
}

void EditList::Add( const std::string& _left, const std::string& _right )
{
	// y coordinate where to insert inside panel
	unsigned line = m_numLines;
	float y = (line - m_scrollBar->getValue()) * 20.0f;

	// Move all the other stuff on line adding
	bool scrollVisib = m_scrollBar->getLowValue()<m_scrollBar->getMaximum();
	m_scrollBar->setMaximum(++m_numLines);
	if( scrollVisib != (m_scrollBar->getLowValue()<m_scrollBar->getMaximum()) )
		ScrollbarVisibilityChanged();

	// indention
	float x = (m_scrollBar->getLowValue()<m_scrollBar->getMaximum()) ? 12.0f : 0.0f;
	// Width of a edit
	float w = (m_list->getSize().x - x) * 0.5f - (m_addNdel ? 6.0f : 0.0f);

	// Create component on the left side
	tgui::EditBox::Ptr left( dynamic_cast<tgui::Container&>(*m_list) );
	left->load("media/Black.conf");
	left->setSize(w, 20.0f);
	left->setPosition(x, y);
	if(!m_leftEditable) left->disable();
	left->setText(_left);

	// Create the one on the right side
	tgui::EditBox::Ptr right( dynamic_cast<tgui::Container&>(*m_list) );
	right->load("media/Black.conf");
	right->setSize(w, 20.0f);
	right->setPosition(w+x, y);
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
}

EditList::NodeID EditList::AddNode( const std::string& _title )
{
	// y coordinate where to insert inside panel
	unsigned line = m_numLines;
	float y = (line - m_scrollBar->getValue()) * 20.0f;

	// Move all the other stuff on line adding
	bool scrollVisib = m_scrollBar->getLowValue()<m_scrollBar->getMaximum();
	m_scrollBar->setMaximum(++m_numLines);
	if( scrollVisib != (m_scrollBar->getLowValue()<m_scrollBar->getMaximum()) )
		ScrollbarVisibilityChanged();

	// Level in hierarchy (indention)
	float x = 12.0f + ((m_scrollBar->getLowValue()<m_scrollBar->getMaximum()) ? 12.0f : 0.0f);
	// Width of a edit
	float w = m_list->getSize().x - (m_addNdel ? 12.0f : 0.0f) - x;

	// Create a title bar
	tgui::EditBox::Ptr title( dynamic_cast<tgui::Container&>(*m_list) );
	title->load("media/Black.conf");
	title->setSize(w, 20.0f);
	title->setPosition(x, y);
	title->disable();
	title->setText("    " + _title);
	tgui::AnimatedPicture::Ptr minimize( dynamic_cast<tgui::Container&>(*m_list) );
	minimize->setPosition(4.0f + x, y+4.0f);
	minimize->addFrame("media/Black_ArrowRight.png");
	minimize->addFrame("media/Black_ArrowDown.png");
	minimize->setFrame(0);
	minimize->setCallbackId(1);
	minimize->setSize(12.0f, 12.0f);
	minimize->bindCallbackEx(&EditList::MiniMaxi, this, tgui::AnimatedPicture::LeftMouseClicked);

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

	return 0;
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

	bool scrollVisib = m_scrollBar->getLowValue()<m_scrollBar->getMaximum();
	m_scrollBar->setMaximum(--m_numLines);
	if( scrollVisib != (m_scrollBar->getLowValue()<m_scrollBar->getMaximum()) )
		ScrollbarVisibilityChanged();
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

void EditList::MiniMaxi( const tgui::Callback& _call )
{
	// Switch frame
	tgui::AnimatedPicture* button = (tgui::AnimatedPicture*)_call.widget;
	button->setFrame(1 - button->getCurrentFrame());
	bool hide = button->getCurrentFrame() == 0;

	// If it was the title bar toggle whole components on/of.
	if( _call.id == 0 )
	{
		if( hide )
		{
			m_list->hide();
			m_newName->hide();
			m_newValue->hide();
			m_newAdd->hide();
		} else {
			m_list->show();
			m_newName->show();
			m_newValue->show();
			m_newAdd->show();
		}
	}
}

void EditList::ScrollbarVisibilityChanged()
{
	float offset = -12.0f;
	if( m_scrollBar->getLowValue()<m_scrollBar->getMaximum() )
		offset = 12.0f;

	float w = (m_list->getSize().x - (m_addNdel ? 12.0f : 0.0f) - offset) * 0.5f;
	float aspect = (m_list->getSize().x - (m_addNdel ? 12.0f : 0.0f) - offset) / (m_list->getSize().x - (m_addNdel ? 12.0f : 0.0f));

	// Move everything accordingly
	auto&  list = m_list->getWidgets();
	for( size_t i=1; i<list.size(); ++i )
	{
		// Do not move delete buttons (right aligned)
		if( list[i]->getPosition().x < m_list->getSize().x-12.0f )
		{
			list[i]->scale(aspect, 1.0f);
//			list[i]->setPosition(aspect * list[i]->getPosition().x, list[i]->getPosition().y);
	//		list[i]->move(offset, 0.0f);
	//		list[i]->setSize(w, 20.0f);
			if( list[i]->getPosition().x < w )
				list[i]->setPosition(list[i]->getPosition().x+offset, list[i]->getPosition().y);
			else list[i]->setPosition(list[i]->getPosition().x+offset*0.5f, list[i]->getPosition().y);
		}
	}
}

} // namespace Graphics