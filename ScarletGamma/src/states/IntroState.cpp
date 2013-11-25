#include "states/IntroState.hpp"

States::IntroState::IntroState(std::string& filename, float duration) :
	GameState(), m_maxTime(duration), m_totalTime(0)
{
	if (m_logoTexture.loadFromFile(filename))
	{
		// Scale texture
		m_logoTexture.setSmooth(true);
		m_logoSprite.setTexture(m_logoTexture);
		m_logoSprite.setScale(0.5f, 0.5f);

		// Center origin for later on-screen centering (see draw())
		sf::FloatRect rect = m_logoSprite.getLocalBounds();
		m_logoSprite.setOrigin(rect.left + rect.width/2.0f,
			rect.top  + rect.height/2.0f);
	}
}

void States::IntroState::Update(float dt)
{
	// Aggregate time
	m_totalTime += dt;

	// Leave state after 2 seconds
	if (m_totalTime > 2.0f)
		m_finished = true;
}

void States::IntroState::Draw(sf::RenderWindow& win)
{
	// Center on screen (every time, window size might change)
	static bool centered = false; // OMG hackyyyy
	if (!centered)
	{
		sf::Vector2u size = win.getSize();
		m_logoSprite.setPosition(sf::Vector2f(size.x/2.0f,size.y/2.0f));
		centered = true;
	}

	// White background
	win.clear(sf::Color(224,224,224));

	// Get interpolation factor [0.0..1.0]
	float interp = m_totalTime / (m_maxTime / 2.f);
	unsigned char alpha;
	if (interp > 1.0f)
		alpha = 255;
	else
		alpha = (unsigned char)(255 * (1 - cos(interp*3.1415)) / 2);

	// Draw with cosine interpolation
	m_logoSprite.setColor(sf::Color(255, 255, 255, alpha));
	win.draw(m_logoSprite);
}