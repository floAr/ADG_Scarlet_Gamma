#include "EditList.hpp"
#include "SFML\System\Vector2.hpp"
#include "TGUI\EditBox.hpp"
#include "utils\StringUtil.hpp"

namespace Graphics {

EditList::EditList() :
	m_newName(),
	m_newValue(),
	m_addNdel(false),
	m_leftEditable(false),
	m_rightEditable(false),
	m_autoSize(true),
	m_oldScrollValue(0),
	m_numPixelLines(0),
	m_nextId(1)
{
}


void EditList::Init( const std::string& _title,
	float _x, float _y, float _w, float _h,
	bool _addNdel, bool _leftEditable, bool _rightEditable, bool _autosize,
	unsigned _pid )
{
	m_addNdel = _addNdel;
	m_leftEditable = _leftEditable;
	m_rightEditable = _rightEditable;
	m_autoSize = _autosize;

	Panel::setPosition(_x, _y + 20.0f);
	if( _autosize )
		Panel::setSize(_w, 0.0f);
	else Panel::setSize(_w, _h - (m_addNdel ? 40.0f : 20.0f));
	Panel::setBackgroundColor( sf::Color(50,50,50,150) );
	Panel::setCallbackId(_pid);

	// Create a scrollbar for long lists.
	m_scrollBar = tgui::Scrollbar::Ptr( *this );
	m_scrollBar->load("media/Black.conf");
	m_scrollBar->setAutoHide(false);
	m_scrollBar->setLowValue(unsigned(Panel::getSize().y));
	m_scrollBar->setSize(12.0f, Panel::getSize().y);
	m_scrollBar->setMaximum(0);
	m_scrollBar->bindCallbackEx( &EditList::Scroll, this, tgui::Scrollbar::ValueChanged );

	// The whole component can be minimized..
	tgui::EditBox::Ptr title( *m_Parent );
	title->load("media/Black.conf");
	title->setSize(_w, 20.0f);
	title->setPosition(_x, _y);
	title->setCallbackId(_pid);
	title->disable();
	title->setText("    " + _title);
	tgui::AnimatedPicture::Ptr minimize( *m_Parent );
	minimize->setPosition(_x+4.0f, _y+4.0f);
	minimize->addFrame("media/Black_ArrowRight.png");
	minimize->addFrame("media/Black_ArrowDown.png");
	minimize->setFrame(1);
	minimize->setCallbackId(_pid);
	minimize->setSize(12.0f, 12.0f);
	minimize->bindCallbackEx(&EditList::MiniMaxi, this, tgui::AnimatedPicture::LeftMouseClicked);

	// Add an edit which creates a new line if changed.
	if( m_addNdel )
	{
		float w = Panel::getSize().x * 0.5f - 20.0f;
		m_newName = tgui::EditBox::Ptr( *m_Parent );
		m_newName->load("media/Black.conf");
		m_newName->setSize(w, 20.0f);
		m_newName->setPosition(_x, _y+Panel::getSize().y+20.0f);
		m_newName->setText("");
		m_newName->setCallbackId(_pid);
		m_newValue = tgui::EditBox::Ptr( *m_Parent );
		m_newValue->load("media/Black.conf");
		m_newValue->setSize(w, 20.0f);
		m_newValue->setPosition(_x+w, _y+Panel::getSize().y+20.0f);
		m_newValue->setText("");
		m_newValue->setCallbackId(_pid);
		m_newAdd = tgui::Button::Ptr( *m_Parent );
		m_newAdd->load("media/Black.conf");
		m_newAdd->setPosition(_x+Panel::getSize().x - 40.0f, _y+Panel::getSize().y+20.0f);
		m_newAdd->setSize(20.0f, 20.0f);
		m_newAdd->bindCallbackEx(&EditList::AddBtn, this, tgui::Button::LeftMouseClicked);
		m_newAdd->setText("+");
		m_newAdd->setCallbackId(_pid);
	}
}

void EditList::Add( const std::string& _left, const std::string& _right )
{
	// Indention
	float x = IsScrollbarVisible() ? 12.0f : 0.0f;

	// y coordinate where to insert inside panel
	float y = m_numPixelLines;
	unsigned i = 0;
	// Search the smallest y where for each item string is greater _left.
	for( size_t i=1; i<m_Widgets.size(); ++i )
	{
		// Only in left row
		if( abs(m_Widgets[i]->getPosition().x-x) < 1.0f )
		{
			tgui::EditBox::Ptr next = m_Widgets[i];
			if( next != nullptr && Utils::IStringLess(_left, next->getText()) )
			{
				y = std::min( next->getPosition().y, y );
			}
		}
	}

	Resize(20, int(y + m_scrollBar->getValue()));

	// Width of a edit
	float w = (Panel::getSize().x - x) * 0.5f - (m_addNdel ? 6.0f : 0.0f);

	// Create component on the left side
	tgui::EditBox::Ptr left( *this );
	left->load("media/Black.conf");
	left->setSize(w, 20.0f);
	left->setPosition(x, y);
	left->setCallbackId(m_nextId);
	if(!m_leftEditable) left->disable();
	left->setText(_left);

	// Create the one on the right side
	tgui::EditBox::Ptr right( *this );
	right->load("media/Black.conf");
	right->setSize(w, 20.0f);
	right->setPosition(w+x, y);
	right->setCallbackId(m_nextId);
	if(!m_rightEditable) right->disable();
	right->setText(_right);

	// Create a remove line button
	if( m_addNdel )
	{
		tgui::Checkbox::Ptr del( *this );
		del->load("media/Black.conf");
		del->setPosition(Panel::getSize().x - 12.0f, y+4.0f);
		del->setSize(12.0f, 12.0f);
		del->setCallbackId(m_nextId);
		del->bindCallbackEx(&EditList::RemoveBtn, this, tgui::Button::LeftMouseClicked);
	}

	++m_nextId;
}

EditList::Ptr EditList::AddNode( const std::string&  _parentName, const std::string& _title )
{
	// Determine y coordinate where to insert inside panel.
	tgui::EditBox::Ptr parent = nullptr;
	for( size_t i=1; i<m_Widgets.size(); ++i )
	{
		// Find an edit with _parentName - Text
		tgui::EditBox::Ptr ptr = m_Widgets[i];
		if( (ptr!=nullptr) && (ptr->getText() == _parentName) )
		{
			parent = ptr;
			break;
		}
	}
	assert(parent != nullptr);

	float y = parent->getPosition().y + parent->getSize().y;
	Resize(40, (int)y);

	// Level in hierarchy (indention)
	float x = 12.0f + (IsScrollbarVisible() ? 12.0f : 0.0f);
	// Width of a edit
	float w = Panel::getSize().x - (m_addNdel ? 12.0f : 0.0f) - x;

	EditList::Ptr newNode( *this );
	newNode->Init( _title, x, y, w, 0.0f,
		m_addNdel, m_leftEditable, m_rightEditable, true,
		parent->getCallbackId() );

	return newNode;
}

void EditList::RemoveBtn(const tgui::Callback& _call)
{
	int posY = (int)_call.widget->getPosition().y;
	int minY = posY, maxY = posY + (int)_call.widget->getSize().y;
	unsigned delLine = _call.widget->getCallbackId();
	
	// First element is always the scrollbar
	for( size_t i=1; i<m_Widgets.size(); ++i )
	{
		// Search and destroy
		if( (m_Widgets[i]->getCallbackId() == delLine) )
		{
			minY = std::min((int)m_Widgets[i]->getPosition().y, minY);
			maxY = std::max((int)(m_Widgets[i]->getPosition().y + m_Widgets[i]->getSize().y), maxY);
			m_Widgets.erase( m_Widgets.begin() + i );
			--i;
		}
	}

	Resize(minY - maxY, posY);
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
	int dif = m_oldScrollValue - _call.value;
	m_oldScrollValue = _call.value;

	// Move everything accordingly
	for( size_t i=1; i<m_Widgets.size(); ++i )
		m_Widgets[i]->move(0.0f, float(dif));
}

void EditList::MiniMaxi( const tgui::Callback& _call )
{
	// Switch frame
	tgui::AnimatedPicture* button = (tgui::AnimatedPicture*)_call.widget;
	button->setFrame(1 - button->getCurrentFrame());
	bool hide = button->getCurrentFrame() == 0;

	// If it was the title bar toggle whole components on/of.
	//if( _call.id == 0 )
	{
		if( hide )
		{
			Panel::hide();
			m_newName->hide();
			m_newValue->hide();
			m_newAdd->hide();
			EditList* parent = dynamic_cast<EditList*>(m_Parent);
			if( parent )
				parent->Resize( -(m_numPixelLines+20), (int)Panel::getPosition().y-1 );
		} else {
			EditList* parent = dynamic_cast<EditList*>(m_Parent);
			float y0 = button->getPosition().y + 16.0f;
			if( parent )
				parent->Resize( m_numPixelLines+20, (int)y0-1 );
			Panel::show();
			m_newName->show();
			m_newValue->show();
			m_newAdd->show();
			// As long as they were hidden the 4 components were not moved.
			Panel::setPosition(Panel::getPosition().x, y0);
			y0 += Panel::getSize().y;
			m_newName->setPosition(m_newName->getPosition().x, y0);
			m_newValue->setPosition(m_newValue->getPosition().x, y0);
			m_newAdd->setPosition(m_newAdd->getPosition().x, y0);
		}
	}
}

/*void EditList::ScrollbarVisibilityChanged()
{
	float offset = -12.0f;
	if( m_scrollBar->getLowValue()<m_scrollBar->getMaximum() )
		offset = 12.0f;

	float w = (Panel::getSize().x - (m_addNdel ? 12.0f : 0.0f) - offset) * 0.5f;
	float aspect = (Panel::getSize().x - (m_addNdel ? 12.0f : 0.0f) - offset) / (Panel::getSize().x - (m_addNdel ? 12.0f : 0.0f));

	// Move and scale everything inside the panel
	for( size_t i=1; i<m_Widgets.size(); ++i )
	{
		// Do not move delete buttons (right aligned)
		if( m_Widgets[i]->getPosition().x < Panel::getSize().x-12.0f )
		{
			m_Widgets[i]->scale(aspect, 1.0f);
			if( m_Widgets[i]->getPosition().x < w )
				m_Widgets[i]->setPosition(m_Widgets[i]->getPosition().x+offset, m_Widgets[i]->getPosition().y);
			else m_Widgets[i]->setPosition(m_Widgets[i]->getPosition().x+offset*0.5f, m_Widgets[i]->getPosition().y);
		}
	}
}*/

void EditList::Resize( int _addLines, int _where )
{
	// If _where is not at the end move elements after that mark
	for( size_t i=1; i<m_Widgets.size(); ++i )
	{
		// Do not move delete buttons (right aligned)
		if( (int)(m_Widgets[i]->getPosition().y+0.5f) >= _where &&
			m_Widgets[i]->isVisible() )
			m_Widgets[i]->move(0.0f, float(_addLines) );
	}

	m_numPixelLines += _addLines;
	if( m_autoSize )
	{
		// Just rescale the area
		Panel::setSize(Panel::getSize().x, float(m_numPixelLines));

		// If parent is another EditList it has to be resized too
		EditList* parent = dynamic_cast<EditList*>(m_Parent);
		if( parent )
			parent->Resize( _addLines, (int)Panel::getPosition().y + m_numPixelLines - _addLines + 20 );

		// Move the 3 things below
		if( m_addNdel )
		{
			float y = Panel::getPosition().y + m_numPixelLines;
			m_newName->setPosition(m_newName->getPosition().x, y);
			m_newValue->setPosition(m_newValue->getPosition().x, y);
			m_newAdd->setPosition(m_newAdd->getPosition().x, y);
		}
	} else {
		// Move all the other stuff on line adding
		//bool scrollVisib = IsScrollbarVisible();
		m_scrollBar->setMaximum(m_numPixelLines);
	//	if( scrollVisib != IsScrollbarVisible() )
//			ScrollbarVisibilityChanged();
	}
}

bool EditList::IsScrollbarVisible()
{
	if( m_autoSize ) return false;
	return true;//m_scrollBar->getLowValue()<m_scrollBar->getMaximum();
}

float EditList::GetHeight() const
{
	return Panel::getSize().y + (m_addNdel ? 40.0f : 20.0f);
}

void EditList::setSize( float width, float height )
{
	// Method untested / does not run correctly
	assert(false);

	Panel::setSize(width, std::max(0.0f, height - (m_addNdel ? 40.0f : 20.0f)));
	float w = Panel::getSize().x * 0.5f - 20.0f;
	m_newName->setSize(w, 20.0f);
	m_newName->setPosition(Panel::getPosition().x,
		Panel::getPosition().y + Panel::getSize().y);
	m_newValue->setSize(w, 20.0f);
	m_newValue->setPosition(Panel::getPosition().x + w,
		Panel::getPosition().y + Panel::getSize().y);
	m_newAdd->setPosition(Panel::getPosition().x + Panel::getSize().x - 40.0f,
		Panel::getPosition().y + Panel::getSize().y);
}

sf::Vector2f EditList::getSize() const
{
	return sf::Vector2f(Panel::getSize().x, GetHeight());
}

} // namespace Graphics