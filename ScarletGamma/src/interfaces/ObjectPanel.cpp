#include "ObjectPanel.hpp"
#include "SFML/System/Vector2.hpp"
#include "utils/StringUtil.hpp"
#include "Constants.hpp"
#include "core/Object.hpp"
#include "core/World.hpp"
#include "DragNDrop.hpp"
#include "core/PredefinedProperties.hpp"
#include "Game.hpp"

using namespace Core;

namespace Interfaces {

ObjectPanel::ObjectPanel() :
	m_newName(nullptr),
	m_newAdd(nullptr),
	m_scrollBar(nullptr),
	m_titleBar(nullptr),
	m_miniMaxi(nullptr),
	m_addAble(false),
	m_oldScrollValue(0),
	m_numPixelLines(0),
	m_viewer(nullptr),
	m_dragNDropSource(DragContent::OBJECT_PANEL),
	m_selected(nullptr),
	m_lastSelected(nullptr)
{
	m_defaultEdit ->load("media/Black.conf");

	m_defaultDelete->load("media/Black.conf");
	m_defaultDelete->setSize(12.0f, 12.0f);
}


void ObjectPanel::Init( float _x, float _y, float _w, float _h,
		bool _addAble, Interfaces::DragContent::Sources _source,
		Interfaces::DragContent** _dragNDropHandler,
		Interfaces::PropertyPanel::Ptr _viewer )
{
	m_addAble = _addAble;
	m_dragNDropHandler = _dragNDropHandler;
	m_dragNDropSource = _source;
	m_viewer = _viewer;

	Panel::setPosition(_x, _y + 20.0f);
	Panel::setSize(_w, _h - (m_addAble ? 40.0f : 20.0f));
	Panel::setBackgroundColor( sf::Color(50,50,50,150) );
	if(m_dragNDropHandler)
		Panel::bindCallbackEx( &ObjectPanel::StartDrag, this, tgui::Panel::LeftMousePressed );
	Panel::bindCallbackEx( &ObjectPanel::SelectObject, this, tgui::EditBox::LeftMouseClicked );
	Panel::bindCallback( &ObjectPanel::Drop, this, tgui::Panel::LeftMouseReleased );

	// Create a scrollbar for long lists.
	m_scrollBar = tgui::Scrollbar::Ptr( *this );
	m_scrollBar->load("media/Black.conf");
	m_scrollBar->setAutoHide(false);
	m_scrollBar->setLowValue(unsigned(Panel::getSize().y));
	m_scrollBar->setSize(12.0f, Panel::getSize().y);
	m_scrollBar->setMaximum(0);
	m_scrollBar->setCallbackId(INVALID_ID);
	m_scrollBar->bindCallbackEx( &ObjectPanel::Scroll, this, tgui::Scrollbar::ValueChanged );

	// The whole component can be minimized..
	m_titleBar = tgui::EditBox::Ptr( *m_Parent );
	m_titleBar->load("media/Black.conf");
	m_titleBar->setSize(_w, 20.0f);
	m_titleBar->setPosition(_x, _y);
	m_titleBar->setText( "" );
	m_titleBar->bindCallback( &ObjectPanel::RefreshFilter, this, tgui::EditBox::TextChanged );
	m_miniMaxi = tgui::AnimatedPicture::Ptr( *m_Parent );
	m_miniMaxi->setPosition(_x+_w-16.0f, _y+4.0f);
	m_miniMaxi->addFrame("media/Black_ArrowRight.png");
	m_miniMaxi->addFrame("media/Black_ArrowDown.png");
	m_miniMaxi->setFrame(1);
	m_miniMaxi->setSize(12.0f, 12.0f);
	m_miniMaxi->bindCallbackEx(&ObjectPanel::MiniMaxi, this, tgui::AnimatedPicture::LeftMouseClicked);

	// Add an edit which creates a new line if changed.
	if( m_addAble )
	{
		float w = Panel::getSize().x - 40.0f;
		m_newName = tgui::EditBox::Ptr( *m_Parent );
		m_newName->load("media/Black.conf");
		m_newName->setSize(w, 20.0f);
		m_newName->setPosition(_x, _y+Panel::getSize().y+20.0f);
		m_newName->setText(STR_EMPTY);
		m_newName->bindCallbackEx(&ObjectPanel::AddBtn, this, tgui::EditBox::ReturnKeyPressed);
		m_newAdd = tgui::Button::Ptr( *m_Parent );
		m_newAdd->load("media/Black.conf");
		m_newAdd->setPosition(_x+Panel::getSize().x - 40.0f, _y+Panel::getSize().y+20.0f);
		m_newAdd->setSize(20.0f, 20.0f);
		m_newAdd->bindCallbackEx(&ObjectPanel::AddBtn, this, tgui::Button::LeftMouseClicked);
		m_newAdd->setText("+");
	}

	RefreshFilter();
}

void ObjectPanel::Add( ObjectID _object )
{
	// Indention for scrollbar
	float x = 12.0f;

	// Get the name of the object
	Object* obj = g_Game->GetWorld()->GetObject(_object);
	std::string name = "     " + obj->GetName();
	const std::string& sprite = obj->GetProperty(STR_PROP_SPRITE).Value();

	// y coordinate where to insert inside panel
	float y = (float)m_numPixelLines;
	unsigned i = 0;
	// Search the smallest y where for each item string is greater _left.
	for( size_t i=1; i<m_Widgets.size(); ++i )
	{
		// Only in left row
		if( abs(m_Widgets[i]->getPosition().x-x) < 1.0f )
		{
			tgui::EditBox::Ptr next = m_Widgets[i];
			std::string text = next->getText();
			if( next != nullptr && Utils::IStringLess(name, text) )
			{
				y = std::min( next->getPosition().y, y );
			} else if( next->getCallbackId() == _object )
			{
				// Already contained -> return
				return;
			}
		}
	}

	Resize(20, int(y));

	// Width of an edit
	float w = Panel::getSize().x - x - (m_addAble ? 12.0f : 0.0f);

	// Create component on the left side
	tgui::EditBox::Ptr nameEdit = m_defaultEdit.clone(); this->add( nameEdit );
	nameEdit->setSize(w, 20.0f);
	nameEdit->setPosition(x, y);
	nameEdit->setCallbackId(_object);
	nameEdit->disable();
	nameEdit->setText(name);
	// Highlight selected object (can happen if refreshing the filter)
	if( m_selected == obj ) HighlightObjectEdit(nameEdit);

	// A preview of this sprite
	if( !sprite.empty() )
	{
		tgui::Picture::Ptr preview( *this );
		preview->load(sprite); // Would be nice if Resource-manager could be used - flaw of gui lib.
		preview->setSize(16.0f, 16.0f);
		preview->setPosition(x+2.0f, y+2.0f);
		preview->setCallbackId(_object);
	}

	// Create a remove line button
	if( m_addAble )
	{
		tgui::Checkbox::Ptr del = m_defaultDelete.clone(); this->add( del );
		del->setPosition(Panel::getSize().x - 12.0f, y+4.0f);
		del->setCallbackId(_object);
		del->bindCallbackEx(&ObjectPanel::RemoveBtn, this, tgui::Button::LeftMouseClicked);
	}
}


void ObjectPanel::setSize( float _width, float _height )
{
	Panel::setSize(_width, ceil(std::max(0.0f, _height - (m_addAble ? 40.0f : 20.0f))));
	m_titleBar->setSize( _width, 20.0f );
	float w = ceil(Panel::getSize().x - 40.0f);
	if( m_addAble )
	{
		m_newName->setSize(w, 20.0f);
		m_newName->setPosition(ceil(Panel::getPosition().x),
			ceil(Panel::getPosition().y + Panel::getSize().y));
		m_newAdd->setPosition(ceil(Panel::getPosition().x + Panel::getSize().x - 40.0f),
			ceil(Panel::getPosition().y + Panel::getSize().y));
	}
	m_scrollBar->setLowValue( (unsigned)Panel::getSize().y );
	m_scrollBar->setSize( 12.0f, Panel::getSize().y );
}

sf::Vector2f ObjectPanel::getSize() const
{
	return sf::Vector2f(Panel::getSize().x, Panel::getSize().y + (m_addAble ? 40.0f : 20.0f));
}

void ObjectPanel::setPosition(float _x, float _y)
{
	_x = ceil(_x);
	_y = ceil(_y);
	Panel::setPosition(_x, _y + 20.0f);
	m_titleBar->setPosition(_x, _y);
	m_miniMaxi->setPosition(_x+Panel::getSize().x-16.0f, _y+4.0f);
	if( m_addAble )
	{
		m_newName->setPosition(_x, _y + 20.0f + Panel::getSize().y);
		m_newAdd->setPosition(_x + Panel::getSize().x - 40.0f,
			_y + 20.0f + Panel::getSize().y);
	}
}


void ObjectPanel::Scroll( int _delta )
{
	// Check if this component must react
	sf::Vector2i mousePos = sf::Mouse::getPosition(g_Game->GetWindow());
	if( mouseOnWidget((float)mousePos.x, (float)mousePos.y) )
	{
		int val = m_scrollBar->getValue() - 5 * _delta;
		val = std::min( val, (int)m_scrollBar->getMaximum() );
		val = std::max( val, (int)m_scrollBar->getMinimum() );
		m_scrollBar->setValue( val );
	}
}


void ObjectPanel::RemoveBtn(const tgui::Callback& _call)
{
	int posY = (int)_call.widget->getPosition().y;
	unsigned delLine = _call.id;
	// Remove object from world
	if( m_selected && m_selected->ID() == delLine ) m_selected = nullptr;
	g_Game->GetWorld()->RemoveObject( delLine );
	
	// First element is always the scrollbar
	for( size_t i=1; i<m_Widgets.size(); ++i )
	{
		// Search and destroy
		if( (m_Widgets[i]->getCallbackId() == delLine) )
		{
			m_Widgets.erase( m_Widgets.begin() + i );
			--i;
		}
	}

	Resize(-20, posY);
}


void ObjectPanel::AddBtn( const tgui::Callback& _call )
{
	if( !m_newName->getText().isEmpty() )
	{
		ObjectID objectID;
		if( m_dragNDropSource == DragContent::MODULES_PANEL )
			objectID = g_Game->GetWorld()->NewModuleTemplate( STR_EMPTY );
		else objectID = g_Game->GetWorld()->NewObjectTemplate( STR_EMPTY );
		g_Game->GetWorld()->GetObject( objectID )->Add( PROPERTY::NAME ).SetValue( m_newName->getText() );
		Add( objectID );
		m_newName->setText("");
	}
}


void ObjectPanel::Scroll( const tgui::Callback& _call )
{
	int dif = m_oldScrollValue - _call.value;
	m_oldScrollValue = _call.value;

	// Move everything accordingly
	for( size_t i=1; i<m_Widgets.size(); ++i )
		m_Widgets[i]->move(0.0f, float(dif));
}


void ObjectPanel::MiniMaxi( const tgui::Callback& _call )
{
	// Switch frame
	tgui::AnimatedPicture* button = (tgui::AnimatedPicture*)_call.widget;
	button->setFrame(1 - button->getCurrentFrame());
	bool hide = button->getCurrentFrame() == 0;

	// If it was the title bar toggle whole components on/of.
	if( hide )
	{
		Panel::hide();
		if(m_newName != nullptr) m_newName->hide();
		if(m_newAdd != nullptr) m_newAdd->hide();

		// "Do not filter"
		m_titleBar->setText("");
		m_titleBar->disable();
	} else {
		float y0 = button->getPosition().y + 16.0f;
		Panel::show();
		// As long as they were hidden the 4 components were not moved.
		Panel::setPosition(Panel::getPosition().x, y0);
		y0 += Panel::getSize().y;

		if(m_newName != nullptr) {
			m_newName->show();
			m_newName->setPosition(m_newName->getPosition().x, y0);
		}
		if(m_newAdd != nullptr) {
			m_newAdd->show();
			m_newAdd->setPosition(m_newAdd->getPosition().x, y0);
		}
		// Use as filter
		m_titleBar->enable();
	}
}


void ObjectPanel::StartDrag(const tgui::Callback& _call)
{
	// mouseOnWhichWidget does not work for disabled components to search manually
	for( size_t i=1; i<m_Widgets.size(); ++i )
	{
		// Element should not be the delete "button".
		if( !dynamic_cast<tgui::Checkbox*>(m_Widgets[i].get())
			&& m_Widgets[i]->mouseOnWidget((float)_call.mouse.x, (float)_call.mouse.y) )
		{
			// Overwrite the last referenced content if it was not handled.
			if( !*m_dragNDropHandler ) *m_dragNDropHandler = new Interfaces::DragContent();
			(*m_dragNDropHandler)->from = m_dragNDropSource;
			(*m_dragNDropHandler)->object = g_Game->GetWorld()->GetObject( m_Widgets[i]->getCallbackId() );
			(*m_dragNDropHandler)->prop = nullptr;
			return;
		}
	}
}

void ObjectPanel::Drop()
{
	if( !*m_dragNDropHandler ) delete *m_dragNDropHandler;
	*m_dragNDropHandler = nullptr;
}


void ObjectPanel::SelectObject(const tgui::Callback& _call)
{
	// Find the clicked object
	for( size_t i=1; i<m_Widgets.size(); ++i )
	{
		tgui::EditBox* ptr = dynamic_cast<tgui::EditBox*>(m_Widgets[i].get());

		if( ptr!=nullptr && ptr->mouseOnWidget((float)_call.mouse.x, (float)_call.mouse.y) )
		{
			m_selected = g_Game->GetWorld()->GetObject(ptr->getCallbackId());
			m_viewer->Show( g_Game->GetWorld(), m_selected );
			HighlightObjectEdit(m_Widgets[i]);
		}
	}
}

void ObjectPanel::HighlightObjectEdit(const tgui::EditBox::Ptr _edit)
{
	// Remove old selection highlight
	if( m_lastSelected != nullptr )
	{
		m_lastSelected->setTextColor( sf::Color(200,200,200) );
		m_lastSelected->setTextSize( 13 );
	}
	// Highlight new component
	_edit->setTextColor( sf::Color(60,60,200) );
	_edit->setTextSize( 15 );
	m_lastSelected = _edit;
}


void ObjectPanel::Resize( int _addLines, int _where )
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

	// Move all the other stuff on line adding
	m_scrollBar->setMaximum(m_numPixelLines);
}


void ObjectPanel::Clear()
{
	m_Widgets.erase( m_Widgets.begin() + 1, m_Widgets.end() );
	Resize(-m_numPixelLines, 0);
}


void ObjectPanel::RefreshFilter()
{
	if(IsMinimized()) return;
	
	Clear();
	m_lastSelected = nullptr;

	const Core::ObjectList* allObjects = nullptr;
	if( m_dragNDropSource == DragContent::MODULES_PANEL )
		allObjects = &g_Game->GetWorld()->GetModuleBase();
	else allObjects = &g_Game->GetWorld()->GetTemplateBase();

	for( int i=0; i<allObjects->Size(); ++i )
	{
		// Test if the name contains the correct part
		if( Utils::IStringContains(g_Game->GetWorld()->GetObject((*allObjects)[i])->GetName(), m_titleBar->getText()) )
			Add( (*allObjects)[i] );
	}
}

} // namespace Graphics