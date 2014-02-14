#include "PropertyPanel.hpp"
#include "SFML/System\Vector2.hpp"
#include "TGUI/EditBox.hpp"
#include "utils/StringUtil.hpp"
#include "Constants.hpp"
#include "core/Object.hpp"
#include "DragNDrop.hpp"
#include "core/World.hpp"
#include "core/PredefinedProperties.hpp"
#include "Game.hpp"
#include <algorithm>
#include "core/Property.hpp"

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
	m_listContainer(nullptr),
	m_addAble(false),
	m_autoSize(true),
	m_oldScrollValue(0),
	m_numPixelLines(0),
	m_player(0),
	m_objects()
{
}


void PropertyPanel::Init( float _x, float _y, float _w, float _h,
		bool _addAble, bool _autoSize, Core::PlayerID _player,
		Interfaces::DragContent** _dragNDropHandler,
		unsigned _pid )
{
	// Use the same basic components as the parent if possible.
	PropertyPanel* parent = nullptr;
	if( m_Parent ) parent = dynamic_cast<PropertyPanel*>(m_Parent->getParent());
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
		m_basicScrollBar->setMaximum(0);

		m_basicMiniMaxi = tgui::AnimatedPicture::Ptr();
		m_basicMiniMaxi->addFrame("media/Black_ArrowRight.png");
		m_basicMiniMaxi->addFrame("media/Black_ArrowDown.png");
		m_basicMiniMaxi->setFrame(1);
		m_basicMiniMaxi->setSize(12.0f, 12.0f);
	}

	m_addAble = _addAble;
	m_autoSize = _autoSize;
	m_player = _player;
	m_dragNDropHandler = _dragNDropHandler;

	Panel::setSize(_w, _h);
	Panel::setPosition(_x, _y);
	Panel::setCallbackId(_pid);

	m_listContainer = tgui::Panel::Ptr( *this );
	m_listContainer->setPosition(0.0f, 20.0f);
	if( _autoSize )
		m_listContainer->setSize(_w, 0.0f);
	else m_listContainer->setSize(_w, _h - (m_addAble ? 40.0f : 20.0f));
	m_listContainer->setBackgroundColor( sf::Color(50,50,50,150) );
	if(m_dragNDropHandler)
	{
		m_listContainer->bindCallbackEx(&PropertyPanel::StartDrag, this, tgui::Panel::LeftMousePressed);
		m_listContainer->bindCallbackEx(&PropertyPanel::HandleDropEvent, this, tgui::Panel::LeftMouseReleased);
	}

	// Create a scrollbar for long lists.
	if( !m_autoSize )
	{
		m_scrollBar = m_basicScrollBar.clone();
		m_listContainer->add(m_scrollBar);
		m_scrollBar->setLowValue(unsigned(m_listContainer->getSize().y));
		m_scrollBar->setSize(12.0f, m_listContainer->getSize().y);
		m_scrollBar->bindCallbackEx( &PropertyPanel::Scroll, this, tgui::Scrollbar::ValueChanged );
	}

	// The whole component can be minimized..
	m_titleBar = m_basicEdit.clone();
	this->add(m_titleBar);
	m_titleBar->setSize(_w, 20.0f);
	m_titleBar->setPosition(0.0f, 0.0f);
	m_titleBar->setCallbackId(_pid);
	m_titleBar->bindCallback( &PropertyPanel::RefreshFilter, this, tgui::EditBox::TextChanged );
	m_miniMaxi = m_basicMiniMaxi.clone();
	this->add(m_miniMaxi);
	m_miniMaxi->setPosition(_w-16.0f, 4.0f);
	m_miniMaxi->setCallbackId(_pid);
	m_miniMaxi->bindCallback(&PropertyPanel::MiniMaxi, this, tgui::AnimatedPicture::LeftMouseClicked);

	// Add an edit which creates a new line if changed.
	if( m_addAble )
	{
		float w = m_listContainer->getSize().x * 0.5f - 20.0f;
		m_newName = m_basicEdit.clone();
		this->add(m_newName);
		m_newName->setSize(w, 20.0f);
		m_newName->setPosition(0.0f, m_listContainer->getSize().y+20.0f);
		m_newName->setCallbackId(_pid);
		m_newName->bindCallbackEx(&PropertyPanel::AddBtn, this, tgui::EditBox::ReturnKeyPressed);
		m_newValue = m_basicEdit.clone();
		this->add(m_newValue);
		m_newValue->setSize(w, 20.0f);
		m_newValue->setPosition(w, m_listContainer->getSize().y+20.0f);
		m_newValue->setCallbackId(_pid);
		m_newValue->bindCallbackEx(&PropertyPanel::AddBtn, this, tgui::EditBox::ReturnKeyPressed);
		m_newAdd = m_basicAddButton.clone();
		this->add(m_newAdd);
		m_newAdd->setPosition(m_listContainer->getSize().x - 40.0f, m_listContainer->getSize().y+20.0f);
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
	unsigned index = 0;
	// Search the smallest y where for each item string is greater _left.
	for( ; index < m_lines.size(); ++index )
	{
		tgui::EditBox::Ptr next = m_lines[index].left;
		std::string text = next->getText();
		if( Utils::IStringLess(_left, text) )
		{
			y = next->getPosition().y;
			break;	// Found
		} else if( Utils::IStringEqual(_left, text) )
		{
			// Key already contained -> just update and return
			if( m_lines[next->getCallbackId()].right->getText() != _right )
				m_lines[next->getCallbackId()].right->setText( _right );
			return;
		}
	}

	Resize(20, int(y));

	// Repair the line indices
	auto& widgets = m_listContainer->getWidgets();
	for( size_t i=1; i<widgets.size(); ++i )
	{
		if( widgets[i]->getCallbackId() >= index )
		{
			// Things after the inserted element now have line index + 1
			widgets[i]->setCallbackId( widgets[i]->getCallbackId() + 1 );
		}
	}

	// Width of an edit
	float w = (Panel::getSize().x - x) * 0.5f - 6.0f;

	EntryLine entry;

	// Create component on the left side
	entry.left = m_basicEdit.clone();
	m_listContainer->add(entry.left);
	entry.left->setSize(w, 20.0f);
	entry.left->setPosition(x, y);
	entry.left->setCallbackId(index);
	// Always disable to avoid careless changes
	entry.left->disable();
	entry.left->setText(_left);

	// Create the one on the right side
	entry.right = m_basicEdit.clone();
	m_listContainer->add(entry.right);
	entry.right->setSize(w, 20.0f);
	entry.right->setPosition(w+x, y);
	entry.right->setCallbackId(index);
	entry.right->bindCallbackEx(&PropertyPanel::ValueChanged, this, tgui::EditBox::Unfocused);
	entry.right->bindCallbackEx(&PropertyPanel::ValueEntered, this, tgui::EditBox::ReturnKeyPressed);
	if(!_editable)
		entry.right->disable();
	entry.right->setText(_right);

	// Create a remove line button
	if( _changable )
	{
		entry.del = m_basicDeleteButton.clone();
		m_listContainer->add(entry.del);
		entry.del->setPosition(Panel::getSize().x - 12.0f, y+4.0f);
		entry.del->setCallbackId(index);
		entry.del->bindCallbackEx(&PropertyPanel::RemoveBtn, this, tgui::Button::LeftMouseClicked);
	}

	m_lines.insert(m_lines.begin() + index, std::move(entry));
}

PropertyPanel::Ptr PropertyPanel::AddNode( EntryLine& _parent )
{
	// Determine y coordinate where to insert inside panel.
	float y = _parent.left->getPosition().y + _parent.left->getSize().y;
	Resize(m_addAble ? 40 : 20, (int)y);

	// Level in hierarchy (indention)
	float x = 12.0f + (IsScrollbarVisible() ? 12.0f : 0.0f);
	// Width of a edit
	float w = m_listContainer->getSize().x - 12.0f - x;

	PropertyPanel::Ptr subNode = PropertyPanel::Ptr( *m_listContainer );
	subNode->Init( x, y, w, 0.0f,
		m_addAble, true, m_player,
		m_dragNDropHandler, _parent.left->getCallbackId() );

	return subNode;
}

void PropertyPanel::RemoveBtn(const tgui::Callback& _call)
{
	unsigned delLine = _call.id;

	// Remove from object(s)
	auto name = m_lines[delLine].left->getText();
	for( size_t i=0; i<m_objects.size(); ++i )
		m_objects[i]->Remove( name );

	Remove( delLine );
}

	
void PropertyPanel::Remove( unsigned _line )
{
	int posY = (int)m_lines[_line].left->getPosition().y;
	int minY = posY, maxY = posY + (int)m_lines[_line].left->getSize().y;

	// First element is always the scrollbar
	auto& widgets = m_listContainer->getWidgets();
	Panel::unfocusWidgets();
	for( size_t i=1; i<widgets.size(); ++i )
	{
		// Search and destroy
		if( (widgets[i]->getCallbackId() == _line) )
		{
			minY = std::min((int)widgets[i]->getPosition().y, minY);
			maxY = std::max((int)(widgets[i]->getPosition().y + std::min(widgets[i]->getSize().y, 20.0f)), maxY);
			m_listContainer->remove( widgets[i] );
			--i;
		} else if( widgets[i]->getCallbackId() > _line )
		{
			// Things after the deleted element now have line index - 1
			widgets[i]->setCallbackId( widgets[i]->getCallbackId() - 1 );
		}
	}

	// Repair m_lines
	m_lines.erase( m_lines.begin() + _line );

	Resize(minY - maxY, posY);
}


void PropertyPanel::AddBtn( const tgui::Callback& _call )
{
	if( !m_newName->getText().isEmpty() )
	{
		// Add to object(s)
		auto name = m_newName->getText();
		for( size_t i=0; i<m_objects.size(); ++i )
			m_objects[i]->Add( Core::PROPERTY::Custom(name) );
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

	if( dif != 0 )
	{
		// Move everything accordingly
		for( size_t i=1; i<m_listContainer->getWidgets().size(); ++i )
			m_listContainer->getWidgets()[i]->move(0.0f, float(dif));
	}
}


void PropertyPanel::MiniMaxi()
{
	// Switch frame
	m_miniMaxi->setFrame(1 - m_miniMaxi->getCurrentFrame());
	bool hide = m_miniMaxi->getCurrentFrame() == 0;

	// If it was the title bar toggle whole components on/of.
	if( hide )
	{
		if( m_autoSize ) Panel::setSize(Panel::getSize().x, 20.0f );
		m_listContainer->hide();
		if(m_newName != nullptr) m_newName->hide();
		if(m_newValue != nullptr) m_newValue->hide();
		if(m_newAdd != nullptr) m_newAdd->hide();
		PropertyPanel* parent = nullptr;
		if( m_Parent ) parent = dynamic_cast<PropertyPanel*>(m_Parent->getParent());
		if( parent )
			parent->Resize( -(m_numPixelLines + (m_addAble?20:0)), (int)Panel::getPosition().y+1 );

		// If minimized show component name in the title bar
		if( m_objects.size()==1 )
			m_titleBar->setText( m_objects[0]->GetName() );
		else if( m_objects.size()>1 )
			m_titleBar->setText( STR_MULTISELECTION );
		m_titleBar->disable();
	} else {
		if( m_autoSize ) Panel::setSize(Panel::getSize().x, float(m_numPixelLines + (m_addAble ? 40.0f : 20.0f)) );
		PropertyPanel* parent = nullptr;
		if( m_Parent ) parent = dynamic_cast<PropertyPanel*>(m_Parent->getParent());
		if( parent )
			parent->Resize( m_numPixelLines + (m_addAble?20:0), (int)Panel::getPosition().y+1 );
		m_listContainer->show();

		if(m_newName != nullptr)	m_newName->show();
		if(m_newValue != nullptr)	m_newValue->show();
		if(m_newAdd != nullptr) 	m_newAdd->show();

		// Use as filter
		m_titleBar->enable();
		m_titleBar->setText( "" );
	}
}


void PropertyPanel::ValueEntered(const tgui::Callback& _call)
{
	m_listContainer->unfocus();
	_call.widget->unfocus();
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
	// mouseOnWhichWidget does not work for disabled components so search manually
	int line = FindLine( (float)_call.mouse.x, (float)_call.mouse.y );
	if( line != -1 )
	{
		// Overwrite the last referenced content if it was not handled.
		if( !*m_dragNDropHandler ) *m_dragNDropHandler = new Interfaces::DragContent();
		(*m_dragNDropHandler)->from = DragContent::PROPERTY_PANEL;
		(*m_dragNDropHandler)->object = nullptr;
		// Each object must have this property!
		(*m_dragNDropHandler)->prop = &m_objects[0]->GetProperty( m_lines[line].left->getText() );
		return;
	}

	// Drag objects (nodes) away.
	for( size_t i = 0; i < m_listContainer->getWidgets().size(); ++i )
	{
		 // Also search for node drags of children from this line
		 PropertyPanel* child = dynamic_cast<PropertyPanel*>(m_listContainer->getWidgets()[i].get());
		 if( child && child->m_titleBar->mouseOnWidget(_call.mouse.x - child->getPosition().x, _call.mouse.y - child->getPosition().y) )
		 {
			 // Overwrite the last referenced content if it was not handled.
			 if( !*m_dragNDropHandler ) *m_dragNDropHandler = new Interfaces::DragContent();
			 (*m_dragNDropHandler)->from = DragContent::PROPERTY_PANEL;
			 (*m_dragNDropHandler)->object = child->m_objects[0];
			 // The property which contains this object can be found by the
			 // callback-id because it always contains the line index
			 (*m_dragNDropHandler)->prop = &m_objects[0]->GetProperty( m_lines[child->getCallbackId()].left->getText() );
			 return;
		 }
	 }

	// Nothing will be dragged remove old stuff
	delete *m_dragNDropHandler;
	*m_dragNDropHandler = nullptr;
}


void PropertyPanel::Resize( int _addLines, int _where )
{
	if( _addLines == 0 ) return;

	// If _where is not at the end move elements after that mark
	for( size_t i=1; i<m_listContainer->getWidgets().size(); ++i )
	{
		// Do not move delete buttons (right aligned)
		if( (int)(m_listContainer->getWidgets()[i]->getPosition().y+0.5f) >= _where &&
			m_listContainer->getWidgets()[i]->isVisible() )
			m_listContainer->getWidgets()[i]->move(0.0f, float(_addLines) );
	}

	m_numPixelLines += _addLines;
	if( m_autoSize )
	{
		// Just rescale the area
		m_listContainer->setSize(Panel::getSize().x, float(m_numPixelLines));
		Panel::setSize(Panel::getSize().x, float(m_numPixelLines+20) );

		// If parent is another PropertyPanel it has to be resized too
		PropertyPanel* parent = nullptr;
		if( m_Parent ) parent = dynamic_cast<PropertyPanel*>(m_Parent->getParent());
		if( parent )
			parent->Resize( _addLines, (int)Panel::getPosition().y + m_numPixelLines - _addLines + 20 );

		// Move the 3 things below
		if( m_addAble )
		{
			float y = m_listContainer->getPosition().y + m_numPixelLines;
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
	return m_listContainer->getSize().y + (m_addAble ? 40.0f : 20.0f);
}

void PropertyPanel::setSize( float _width, float _height )
{
	Panel::setSize(_width, _height);
	m_listContainer->setSize(_width, ceil(std::max(0.0f, _height - (m_addAble ? 40.0f : 20.0f))));
	m_titleBar->setSize( _width, 20.0f );
	float w = ceil(_width * 0.5f - 20.0f);
	if( m_addAble )
	{
		m_newName->setSize(w, 20.0f);
		m_newName->setPosition(0.0f, ceil(20.0f + m_listContainer->getSize().y));
		m_newValue->setSize(w, 20.0f);
		m_newValue->setPosition(w, ceil(20.0f + m_listContainer->getSize().y));
		m_newAdd->setPosition(ceil(m_listContainer->getSize().x - 40.0f),
			ceil(20.0f + m_listContainer->getSize().y));
	}
	m_scrollBar->setLowValue( (unsigned)m_listContainer->getSize().y );
	m_scrollBar->setSize( 12.0f, m_listContainer->getSize().y );
}


void PropertyPanel::unfocus()
{
	Panel::unfocus();
	m_Parent->unfocus();
}


void PropertyPanel::HandleDropEvent(const tgui::Callback& _call)
{
	if( !(*m_dragNDropHandler) ) return;

	// Do not handle things which are targeted to a children
	for( size_t i = 0; i < m_listContainer->getWidgets().size(); ++i )
	{
		// Also search for node drags of children from this line
		PropertyPanel* child = dynamic_cast<PropertyPanel*>(m_listContainer->getWidgets()[i].get());
		if( child && child->mouseOnWidget((float)_call.mouse.x, (float)_call.mouse.y) )
			return;
	}

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
		// Insert a single property or move an object
		if( (*m_dragNDropHandler)->object)
		{
			if( (*m_dragNDropHandler)->object->HasProperty(STR_PROP_ITEM) )
			{
				if( m_objects.size() != 1 ) throw STR_AMBIGIOUS_DRAG;
				int line = FindLine((float)_call.mouse.x, (float)_call.mouse.y);
				if( line != -1 )
				{
					// Take away from the source object
					(*m_dragNDropHandler)->prop->RemoveObject((*m_dragNDropHandler)->object->ID());

					// Insert
					std::string propName = m_lines[line].left->getText();
					m_objects[0]->GetProperty( propName ).AddObject( (*m_dragNDropHandler)->object->ID() );
					addedSomething = true;
				}
			}
		} else
		for( size_t i=0; i<m_objects.size(); ++i )
		{
			// Check if the property is new and do not overwrite if it already exists.
			if( !m_objects[i]->HasProperty( (*m_dragNDropHandler)->prop->Name() ) )
			{
				m_objects[i]->Add( *(*m_dragNDropHandler)->prop );		
				addedSomething = true;
			}
		}
	} else if( (*m_dragNDropHandler)->from == DragContent::OBJECT_PANEL )
	{
		// Find out on which property the element was dropped
		int line = FindLine( (float)_call.mouse.x, (float)_call.mouse.y );
		if( line != -1 )
		{
			// Found the property
			std::string propName = m_lines[line].left->getText();
			// Add a clone into all objects
			for( size_t i=0; i<m_objects.size(); ++i )
			{
				// One clone for everybody
				Core::ObjectID id = m_world->NewObject( (*m_dragNDropHandler)->object );
				// Every new added object is now flagged as item (can be
				// dragged away again)
				m_world->GetObject(id)->Add( Core::PROPERTY::ITEM );
				m_objects[i]->GetProperty( propName ).AddObject( id );
				addedSomething = true;
			}
		}
	}
	delete *m_dragNDropHandler;
	*m_dragNDropHandler = nullptr;

	// Update gui - there are new properties
	if( addedSomething )
		RefreshFilter();
}


void PropertyPanel::Scroll( int _delta )
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


void PropertyPanel::Show( Core::World* _world, Core::Object* _object )
{
	assert( _object );
	m_world = _world;

	m_objects.clear();
	m_objects.push_back(_object);

	// Use the name property in title bar in minimized mode.
	if( !IsMinimized() ) RefreshFilter();
	else m_titleBar->setText( _object->GetName() );
}


void PropertyPanel::Show( Core::World* _world, const Core::ObjectList& _objects )
{
	m_world = _world;

	// Compare the two lists - if nothing changed do just an update
	bool listChanged = m_objects.size() != _objects.Size();
	if( !listChanged )
	{
		for( size_t i=0; i<m_objects.size(); ++i )
			if( m_objects[i] != _world->GetObject(_objects[i]) ) {
				listChanged = true;
				break;
			} else if( m_objects[i]->ID() != _objects[i] ) {
				listChanged = true;
				break;
			}
	}

	// First copy the entries.
	m_objects.clear();
	for( int i=0; i<_objects.Size(); ++i )
		m_objects.push_back( _world->GetObject(_objects[i]) );

	if( !IsMinimized() ) RefreshFilter();
	else {
		// Use placeholder title for multiple objects.
		if( _objects.Size() == 1 )
			m_titleBar->setText( _world->GetObject(_objects[0])->GetName() );
		else m_titleBar->setText( STR_MULTISELECTION );
	}
}


void PropertyPanel::Clear()
{
	if( m_scrollBar != nullptr )
		m_listContainer->getWidgets().erase( m_listContainer->getWidgets().begin() + 1, m_listContainer->getWidgets().end() );
	else m_listContainer->getWidgets().erase( m_listContainer->getWidgets().begin(), m_listContainer->getWidgets().end() );
	m_lines.clear();
	Resize(-m_numPixelLines, 0);
}


void PropertyPanel::RefreshFilter()
{
	if(IsMinimized()) return;

	Panel::unfocusWidgets();
	
	if( m_objects.size() == 0 ) { Clear(); return; }

	// Get all possible properties from the first object
	auto allProperties = m_objects[0]->FilterByName( m_titleBar->getText() );

	// Sort by name
	std::sort( allProperties.begin(), allProperties.end(),
		[](const Core::Property* _1, const Core::Property* _2)
		{
			return Utils::IStringLess(_1->Name(), _2->Name());
		}
	);

	size_t numAdded = 0;

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
			{
				// Now update / add or remove a property.
				// Assert: All properties seen up to now are equal the seen ones
				//		from allProperties.
				// Case 1: The current list is just shorter -> current one is new.
				// Case 2: The current line and the observed property are equal ->
				//		update value.
				// Case 3a: Unequal and newName < lineName -> we must add a new element.
				// Case 3b: Unequal and newName > lineName -> delete the current line.
				Reevaluate:
				if( m_lines.size() <= numAdded )
				{
					AddLine( numAdded, allProperties[i] );
				} else {
					std::string lineName = m_lines[numAdded].left->getText();
					if(lineName == allProperties[i]->Name())
					{
						RefreshLine( numAdded, allProperties[i] );
					} else if( Utils::IStringLess( allProperties[i]->Name(), lineName ) )
					{
						AddLine( numAdded, allProperties[i] );
					} else {
						Remove( numAdded );
						goto Reevaluate;
					}
				}

				++numAdded;
			}
		}
	}
}


void PropertyPanel::AddLine( unsigned _line, const Core::Property* _property )
{
	Add( _property->Name(), _property->CanChange(m_player),
		_property->Value(), _property->CanEdit(m_player) );

	// Add inventory recursively
	if( _property->IsObjectList() && m_objects.size() == 1 )
	{
		for( int j=0; j<_property->GetObjects().Size(); ++j )
		{
			Ptr node = AddNode( m_lines[_line] );
			node->show();
			node->Show( m_world, m_world->GetObject( _property->GetObjects()[j] ) );
			node->MiniMaxi();
		}
	}
}


void PropertyPanel::RefreshLine( unsigned _line, const Core::Property* _property )
{
	// Update except it is focused (than somebody is editing)
	if( !m_lines[_line].right->isFocused() )
		m_lines[_line].right->setText( _property->Value() );
	// The difficult part of updates - find the sub nodes
	// and refresh them too.
	if( m_objects.size() == 1 )
	{
		// Make a copy of the list
		Core::ObjectList tmpObjects = _property->GetObjects();
		auto& widgets = m_listContainer->getWidgets();
		for( size_t i=0; i<widgets.size(); ++i )
		{
			// Only test objects which belong to the current line
			if( widgets[i]->getCallbackId() == _line )
			{
				// Is it a sub node?
				PropertyPanel* subNode = dynamic_cast<PropertyPanel*>(widgets[i].get());
				if( subNode != nullptr )
				{
					// Now me must check for update/delete of the current node.
					if( tmpObjects.Contains( subNode->m_objects[0]->ID() ) )
					{
						// Update
						subNode->RefreshFilter();
						tmpObjects.Remove( subNode->m_objects[0]->ID() );
					} else { // Delete
						// Kick only the node - the line is still there.
						Resize( -(int)widgets[i]->getSize().y, (int)widgets[i]->getPosition().y );
						widgets.erase( widgets.begin() + i );
					}
				}
			}
		}

		// Add new objects (the ones on the list which are not found).
		for( int i=0; i<tmpObjects.Size(); ++i )
		{
			Ptr node = AddNode( m_lines[_line] );
			node->show();
			node->Show( m_world, m_world->GetObject( tmpObjects[i] ) );
			node->MiniMaxi();
		}
	}
}


int PropertyPanel::FindLine( float _x, float _y )
{
	// Find out on which line the mouse is
	for( size_t i=0; i<m_lines.size(); ++i )
	{
		if( m_lines[i].left->mouseOnWidget( _x, _y )
			|| m_lines[i].right->mouseOnWidget( _x, _y ) )
		{
			// Found
			return i;
		}
	}

	return -1;
}

} // namespace Graphics