#include "PropertyPanel.hpp"
#include "SFML/System\Vector2.hpp"
#include "TGUI/EditBox.hpp"
#include "utils/StringUtil.hpp"
#include "Constants.hpp"
#include "core/Object.hpp"
#include "DragNDrop.hpp"
#include "core/World.hpp"

namespace Interfaces {

PropertyPanel::PropertyPanel() :
	m_basicEdit(nullptr),
	m_basicDeleteButton(nullptr),
	m_basicAddButton(nullptr),
	m_basicScrollBar(nullptr),
	m_basicMiniMaxi(nullptr),
	m_newName(nullptr),
	m_newValue(nullptr),
	m_newAdd(nullptr),
	m_scrollBar(nullptr),
	m_titleBar(nullptr),
	m_miniMaxi(nullptr),
	m_addAble(false),
	m_autoSize(true),
	m_oldScrollValue(0),
	m_numPixelLines(0),
	m_player(0),
	m_objects()
{
	// Use the same basic components as the parent if possible.
	PropertyPanel* parent = dynamic_cast<PropertyPanel*>(m_Parent);
	if( parent )
	{
		m_basicEdit = parent->m_basicEdit;
		m_basicDeleteButton = parent->m_basicDeleteButton;
		m_basicAddButton = parent->m_basicAddButton;
		m_basicScrollBar = parent->m_basicScrollBar;
		m_basicMiniMaxi = parent->m_basicMiniMaxi;
	} else {
		// Otherwise create a set of base components
		m_basicEdit = tgui::EditBox::Ptr();
		m_basicEdit->load("media/Black.conf");
		m_basicEdit->setText("");

		m_basicDeleteButton = tgui::Checkbox::Ptr();
		m_basicDeleteButton->load("media/Black.conf");
		m_basicDeleteButton->setSize(12.0f, 12.0f);

		m_basicAddButton = tgui::Button::Ptr();
		m_basicAddButton->load("media/Black.conf");
		m_basicAddButton->setText("+");
		m_basicAddButton->setSize(20.0f, 20.0f);

		m_basicScrollBar = tgui::Scrollbar::Ptr();
		m_basicScrollBar->load("media/Black.conf");
		m_basicScrollBar->setAutoHide(false);
		m_basicScrollBar->setCallbackId(0xffffffff);
		m_basicScrollBar->bindCallbackEx( &PropertyPanel::Scroll, this, tgui::Scrollbar::ValueChanged );
		m_basicScrollBar->setMaximum(0);

		m_basicMiniMaxi = tgui::AnimatedPicture::Ptr();
		m_basicMiniMaxi->addFrame("media/Black_ArrowRight.png");
		m_basicMiniMaxi->addFrame("media/Black_ArrowDown.png");
		m_basicMiniMaxi->setFrame(1);
		m_basicMiniMaxi->setSize(12.0f, 12.0f);
	}
}


void PropertyPanel::Init( float _x, float _y, float _w, float _h,
		bool _addAble, bool _autoSize, Core::PlayerID _player,
		Interfaces::DragContent** _dragNDropHandler,
		unsigned _pid )
{
	m_addAble = _addAble;
	m_autoSize = _autoSize;
	m_player = _player;
	m_dragNDropHandler = _dragNDropHandler;

	Panel::setPosition(_x, _y + 20.0f);
	if( _autoSize )
		Panel::setSize(_w, 0.0f);
	else Panel::setSize(_w, _h - (m_addAble ? 40.0f : 20.0f));
	Panel::setBackgroundColor( sf::Color(50,50,50,150) );
	Panel::setCallbackId(_pid);
	if(m_dragNDropHandler)
	{
		Panel::bindCallbackEx(&PropertyPanel::StartDrag, this, tgui::Panel::LeftMousePressed);
		Panel::bindCallback(&PropertyPanel::HandleDropEvent, this, tgui::Panel::LeftMouseReleased);
	}

	// Create a scrollbar for long lists.
	m_scrollBar = m_basicScrollBar.clone();
	this->add(m_scrollBar);
	m_scrollBar->setLowValue(unsigned(Panel::getSize().y));
	m_scrollBar->setSize(12.0f, Panel::getSize().y);

	// The whole component can be minimized..
	m_titleBar = m_basicEdit.clone();
	m_Parent->add(m_titleBar);
	m_titleBar->setSize(_w, 20.0f);
	m_titleBar->setPosition(_x, _y);
	m_titleBar->setCallbackId(_pid);
	m_titleBar->bindCallback( &PropertyPanel::RefreshFilter, this, tgui::EditBox::TextChanged );
	m_miniMaxi = m_basicMiniMaxi.clone();
	m_Parent->add(m_miniMaxi);
	m_miniMaxi->setPosition(_x+_w-16.0f, _y+4.0f);
	m_miniMaxi->setCallbackId(_pid);
	m_miniMaxi->bindCallbackEx(&PropertyPanel::MiniMaxi, this, tgui::AnimatedPicture::LeftMouseClicked);

	// Add an edit which creates a new line if changed.
	if( m_addAble )
	{
		float w = Panel::getSize().x * 0.5f - 20.0f;
		m_newName = m_basicEdit.clone();
		m_Parent->add(m_newName);
		m_newName->setSize(w, 20.0f);
		m_newName->setPosition(_x, _y+Panel::getSize().y+20.0f);
		m_newName->setCallbackId(_pid);
		m_newValue = m_basicEdit.clone();
		m_Parent->add(m_newValue);
		m_newValue->setSize(w, 20.0f);
		m_newValue->setPosition(_x+w, _y+Panel::getSize().y+20.0f);
		m_newValue->setCallbackId(_pid);
		m_newAdd = m_basicAddButton.clone();
		m_Parent->add(m_newAdd);
		m_newAdd->setPosition(_x+Panel::getSize().x - 40.0f, _y+Panel::getSize().y+20.0f);
		m_newAdd->bindCallbackEx(&PropertyPanel::AddBtn, this, tgui::Button::LeftMouseClicked);
		m_newAdd->setCallbackId(_pid);
	}
}

void PropertyPanel::Add( const std::string& _left, bool _changable, const std::string& _right, bool _editable )
{
	// Indention
	float x = IsScrollbarVisible() ? 12.0f : 0.0f;

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
			if( next != nullptr && Utils::IStringLess(_left, text) )
			{
				y = std::min( next->getPosition().y, y );
			} else if( Utils::IStringEqual(_left, text) )
			{
				// Key already contained -> just update and return
				if( m_lines[next->getCallbackId()].right->getText() != _right )
					m_lines[next->getCallbackId()].right->setText( _right );
				return;
			}
		}
	}

	Resize(20, int(y));

	// Width of an edit
	float w = (Panel::getSize().x - x) * 0.5f - 6.0f;

	EntryLine entry;

	// Create component on the left side
	entry.left = m_basicEdit.clone();
	this->add(entry.left);
	entry.left->setSize(w, 20.0f);
	entry.left->setPosition(x, y);
	entry.left->setCallbackId(m_lines.size());
	// Always disable to avoid careless changes
	entry.left->disable();
	entry.left->setText(_left);

	// Create the one on the right side
	entry.right = m_basicEdit.clone();
	this->add(entry.right);
	entry.right->setSize(w, 20.0f);
	entry.right->setPosition(w+x, y);
	entry.right->setCallbackId(m_lines.size());
	entry.right->bindCallbackEx(&PropertyPanel::ValueChanged, this, tgui::EditBox::Unfocused);
	if(!_editable)
		entry.right->disable();
	entry.right->setText(_right);

	// Create a remove line button
	if( _changable )
	{
		entry.del = m_basicDeleteButton.clone();
		this->add(entry.del);
		entry.del->setPosition(Panel::getSize().x - 12.0f, y+4.0f);
		entry.del->setCallbackId(m_lines.size());
		entry.del->bindCallbackEx(&PropertyPanel::RemoveBtn, this, tgui::Button::LeftMouseClicked);
	}

	m_lines.push_back(std::move(entry));
}

PropertyPanel::Ptr PropertyPanel::AddNode( const std::string&  _parentName )
{
	// Determine y coordinate where to insert inside panel.
	EntryLine* parent = nullptr;
	for( size_t i=0; i<m_lines.size(); ++i )
	{
		// Find an edit with _parentName - Text
		if( m_lines[i].left->getText() == _parentName )
		{
			parent = &m_lines[i];
			break;
		}
	}
	assert(parent != nullptr);

	float y = parent->left->getPosition().y + parent->left->getSize().y;
	Resize(40, (int)y);

	// Level in hierarchy (indention)
	float x = 12.0f + (IsScrollbarVisible() ? 12.0f : 0.0f);
	// Width of a edit
	float w = Panel::getSize().x - 12.0f - x;

	parent->subNode = PropertyPanel::Ptr( *this );
	parent->subNode->Init( x, y, w, 0.0f,
		m_addAble, true, m_player,
		m_dragNDropHandler, parent->left->getCallbackId() );

	return parent->subNode;
}

void PropertyPanel::RemoveBtn(const tgui::Callback& _call)
{
	int posY = (int)_call.widget->getPosition().y;
	int minY = posY, maxY = posY + (int)_call.widget->getSize().y;
	unsigned delLine = _call.id;

	// Remove from object(s)
	auto name = m_lines[delLine].left->getText();
	for( size_t i=0; i<m_objects.size(); ++i )
		m_objects[i]->Remove( name );
	
	// First element is always the scrollbar
	for( size_t i=1; i<m_Widgets.size(); ++i )
	{
		// Search and destroy
		if( (m_Widgets[i]->getCallbackId() == delLine) )
		{
			minY = std::min((int)m_Widgets[i]->getPosition().y, minY);
			maxY = std::max((int)(m_Widgets[i]->getPosition().y + std::min(m_Widgets[i]->getSize().y, 20.0f)), maxY);
			m_Widgets.erase( m_Widgets.begin() + i );
			--i;
		}
	}

	// Repair m_lines
	m_lines.erase( m_lines.begin() + delLine );
	for( size_t i=delLine; i<m_lines.size(); ++i )
	{
		m_lines[i].left->setCallbackId(i);
		m_lines[i].right->setCallbackId(i);
		if(m_lines[i].del != nullptr) m_lines[i].del->setCallbackId(i);
		if(m_lines[i].subNode != nullptr)
		{
			m_lines[i].subNode->setCallbackId(i);
			m_lines[i].subNode->m_newName->setCallbackId(i);
			m_lines[i].subNode->m_newValue->setCallbackId(i);
			m_lines[i].subNode->m_newAdd->setCallbackId(i);
			m_lines[i].subNode->m_scrollBar->setCallbackId(i);
			m_lines[i].subNode->m_titleBar->setCallbackId(i);
			m_lines[i].subNode->m_miniMaxi->setCallbackId(i);
		}
	}

	Resize(minY - maxY, posY);
}

void PropertyPanel::AddBtn( const tgui::Callback& _call )
{
	if( !m_newName->getText().isEmpty() )
	{
		// Add to object(s)
		auto name = m_newName->getText();
		for( size_t i=0; i<m_objects.size(); ++i )
			m_objects[i]->Add( Core::Property(m_objects[i]->ID(), Core::Property::R_VCEV0EV00, name, STR_EMPTY) );
		// Add to gui
		Add( m_newName->getText(), true, m_newValue->getText(), true );
		m_newName->setText("");
		m_newValue->setText("");
	}
}


void PropertyPanel::Scroll( const tgui::Callback& _call )
{
	int dif = m_oldScrollValue - _call.value;
	m_oldScrollValue = _call.value;

	// Move everything accordingly
	for( size_t i=1; i<m_Widgets.size(); ++i )
		m_Widgets[i]->move(0.0f, float(dif));
}


void PropertyPanel::MiniMaxi( const tgui::Callback& _call )
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
		if(m_newValue != nullptr) m_newValue->hide();
		if(m_newAdd != nullptr) m_newAdd->hide();
		PropertyPanel* parent = dynamic_cast<PropertyPanel*>(m_Parent);
		if( parent )
			parent->Resize( -(m_numPixelLines+20), (int)Panel::getPosition().y-1 );

		// If minimized show component name in the title bar
		if( m_objects.size()==1 )
			m_titleBar->setText( m_objects[0]->GetName() );
		else if( m_objects.size()>1 )
			m_titleBar->setText( STR_MULTISELECTION );
		m_titleBar->disable();
	} else {
		PropertyPanel* parent = dynamic_cast<PropertyPanel*>(m_Parent);
		float y0 = button->getPosition().y + 16.0f;
		if( parent )
			parent->Resize( m_numPixelLines+20, (int)y0-1 );
		Panel::show();
		// As long as they were hidden the 4 components were not moved.
		Panel::setPosition(Panel::getPosition().x, y0);
		y0 += Panel::getSize().y;

		if(m_newName != nullptr) {
			m_newName->show();
			m_newName->setPosition(m_newName->getPosition().x, y0);
		}
		if(m_newValue != nullptr) {
			m_newValue->show();
			m_newValue->setPosition(m_newValue->getPosition().x, y0);
		}
		if(m_newAdd != nullptr) {
			m_newAdd->show();
			m_newAdd->setPosition(m_newAdd->getPosition().x, y0);
		}
		// Use as filter
		m_titleBar->enable();
		m_titleBar->setText( "" );
	}
}


void PropertyPanel::ValueChanged(const tgui::Callback& _call)
{
	// Find the property in the object over its name.
	auto name = m_lines[_call.id].left->getText();
	// Convert the target value once
	std::string value = m_lines[_call.id].right->getText();

	for( size_t i=0; i<m_objects.size(); ++i )
	{
		m_objects[i]->SetPropertyValue( name, value );
	}
}


void PropertyPanel::StartDrag(const tgui::Callback& _call)
{
	// mouseOnWhichWidget does not work for disabled components to search manually
	for( size_t i=0; i<m_lines.size(); ++i )
	{
		bool onThisLine = false;
		onThisLine |= m_lines[i].left->mouseOnWidget((float)_call.mouse.x, (float)_call.mouse.y);
		onThisLine |= m_lines[i].right->mouseOnWidget((float)_call.mouse.x, (float)_call.mouse.y);
		if( onThisLine )
		{
			// Overwrite the last referenced content if it was not handled.
			if( !*m_dragNDropHandler ) *m_dragNDropHandler = new Interfaces::DragContent();
			(*m_dragNDropHandler)->from = DragContent::PROPERTY_PANEL;
			(*m_dragNDropHandler)->object = nullptr;
			// Each object must have this property!
			(*m_dragNDropHandler)->prop = &m_objects[0]->GetProperty( m_lines[i].left->getText() );
			return;
		}
	}
	// TODO: drag objects (nodes) away.
}


void PropertyPanel::Resize( int _addLines, int _where )
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
		PropertyPanel* parent = dynamic_cast<PropertyPanel*>(m_Parent);
		if( parent )
			parent->Resize( _addLines, (int)Panel::getPosition().y + m_numPixelLines - _addLines + 20 );

		// Move the 3 things below
		if( m_addAble )
		{
			float y = Panel::getPosition().y + m_numPixelLines;
			m_newName->setPosition(m_newName->getPosition().x, y);
			m_newValue->setPosition(m_newValue->getPosition().x, y);
			m_newAdd->setPosition(m_newAdd->getPosition().x, y);
		}
	} else {
		// Move all the other stuff on line adding
		m_scrollBar->setMaximum(m_numPixelLines);
	}
}

bool PropertyPanel::IsScrollbarVisible()
{
	if( m_autoSize ) return false;
	return true;
}

float PropertyPanel::GetHeight() const
{
	return Panel::getSize().y + (m_addAble ? 40.0f : 20.0f);
}

void PropertyPanel::setSize( float _width, float _height )
{
	Panel::setSize(_width, ceil(std::max(0.0f, _height - (m_addAble ? 40.0f : 20.0f))));
	m_titleBar->setSize( _width, 20.0f );
	float w = ceil(Panel::getSize().x * 0.5f - 20.0f);
	if( m_addAble )
	{
		m_newName->setSize(w, 20.0f);
		m_newName->setPosition(ceil(Panel::getPosition().x),
			ceil(Panel::getPosition().y + Panel::getSize().y));
		m_newValue->setSize(w, 20.0f);
		m_newValue->setPosition(ceil(Panel::getPosition().x + w),
			ceil(Panel::getPosition().y + Panel::getSize().y));
		m_newAdd->setPosition(ceil(Panel::getPosition().x + Panel::getSize().x - 40.0f),
			ceil(Panel::getPosition().y + Panel::getSize().y));
	}
	m_scrollBar->setLowValue( (unsigned)Panel::getSize().y );
	m_scrollBar->setSize( 12.0f, Panel::getSize().y );
}

sf::Vector2f PropertyPanel::getSize() const
{
	return sf::Vector2f(Panel::getSize().x, GetHeight());
}


void PropertyPanel::setPosition(float _x, float _y)
{
	_x = ceil(_x);
	_y = ceil(_y);
	Panel::setPosition(_x, _y + 20.0f);
	m_titleBar->setPosition(_x, _y);
	m_miniMaxi->setPosition(_x+Panel::getSize().x-16.0f, _y+4.0f);
	if( m_addAble )
	{
		m_newName->setPosition(_x, _y + 20.0f + Panel::getSize().y);
		m_newValue->setPosition(ceil(Panel::getPosition().x + Panel::getSize().x * 0.5f - 20.0f),
			_y + 20.0f + Panel::getSize().y);
		m_newAdd->setPosition(_x + Panel::getSize().x - 40.0f,
			_y + 20.0f + Panel::getSize().y);
	}
}


void PropertyPanel::HandleDropEvent()
{
	if( !(*m_dragNDropHandler) ) return;

	bool addedSomething = false;
	if( (*m_dragNDropHandler)->from == DragContent::MODULES_PANEL )
	{
		// Go over all properties of the reference object
		const Core::Object* obj = (*m_dragNDropHandler)->object;
		for( int i=0; i<obj->GetNumElements(); ++i )
		{
			const Core::Property* prop = obj->At(i);
			for( size_t i=0; i<m_objects.size(); ++i )
			{
				// Check if the property is new and do not overwrite if it already exists.
				if( !m_objects[i]->HasProperty( prop->Name() ) )
				{
					m_objects[i]->Add( *prop );		
					addedSomething = true;
				}
			}
		}
	} else if( (*m_dragNDropHandler)->from == DragContent::PROPERTY_PANEL )
	{
		for( size_t i=0; i<m_objects.size(); ++i )
		{
			// Check if the property is new and do not overwrite if it already exists.
			if( !m_objects[i]->HasProperty( (*m_dragNDropHandler)->prop->Name() ) )
			{
				m_objects[i]->Add( *(*m_dragNDropHandler)->prop );		
				addedSomething = true;
			}
		}
	}
	// Update gui - there are new properties
	if( addedSomething )
		RefreshFilter();
}


void PropertyPanel::Show( Core::Object* _object )
{
	m_objects.clear();
	m_objects.push_back(_object);

	// Use the name property in title bar in minimized mode.
	if( !IsMinimized() ) RefreshFilter();
	else m_titleBar->setText( _object->GetName() );
}


void PropertyPanel::Show( Core::World* _world, const Core::ObjectList& _objects )
{
	// First copy the entries.
	m_objects.clear();
	for( int i=0; i<_objects.Size(); ++i )
		m_objects.push_back( _world->GetObject(_objects[i]) );

	// Use placeholder title for multiple objects.
	if( !IsMinimized() ) RefreshFilter();
	else m_titleBar->setText( STR_MULTISELECTION );
}


void PropertyPanel::Clear()
{
	m_Widgets.erase( m_Widgets.begin() + 1, m_Widgets.end() );
	m_lines.clear();
	Resize(-m_numPixelLines, 0);
}


void PropertyPanel::RefreshFilter()
{
	if(IsMinimized()) return;
	
	Clear();

	// Get all possible properties from the first object
	auto allProperties = m_objects[0]->FilterByName( m_titleBar->getText() );

	for( size_t i=0; i<allProperties.size(); ++i )
	{
		if( allProperties[i]->CanSee(m_player) )
		{
			// Find largest common set by removing the ones which does not exists in other objects.
			bool commonProperty = true;
			for( size_t j=1; j<m_objects.size(); ++j )
				commonProperty &= m_objects[j]->HasProperty( allProperties[i]->Name() );

			// Skip non-common properties
			if( commonProperty )
				Add( allProperties[i]->Name(), allProperties[i]->CanChange(m_player),
					 allProperties[i]->Value(), allProperties[i]->CanEdit(m_player) );
		}
	}
}

} // namespace Graphics