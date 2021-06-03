#pragma once

#include <SFML/Graphics.hpp>

class Agent {

	sf::Color col;
	sf::CircleShape c;
	sf::Vector2f pos;

	float g, h, sweep;

	Agent* parent;
public:
	Agent(sf::Color color, sf::Vector2f pos, float sweep) : col(color), pos(pos), c(5, 100), g(0.0f), h(0.0f), sweep(sweep), parent(nullptr) {
		c.setPosition(pos.x-5, pos.y-5);
	}

	Agent(const Agent& agent) : col(agent.getColor()), pos(agent.getPosition()), c(5, 100), g(agent.getG()), h(agent.getH()), sweep(agent.getSweep()), parent(agent.getParent()) {
		c.setPosition(pos.x - 5, pos.y - 5);
	}

	void render(sf::RenderWindow* win) {
		c.setFillColor(col);
		win->draw(c);
	}

	inline float getG() const { return g; }
	inline float getH() const { return h; }
	inline float getF() const { return g + h; }
	inline float getSweep() const { return sweep; }
	inline sf::Color getColor() const { return col; }
	inline sf::Vector2f getPosition() const { return pos; }
	inline Agent* getParent() const { return parent; }

	inline void setColor(sf::Color _col) { col = _col; }
	inline void setG(float _g) { g = _g + sweep; }
	inline void setH(sf::Vector2f _pos) { h = dist(_pos); }
	inline void setParent(Agent* a) { parent = a; }
	inline void setPosition(sf::Vector2f _pos) { pos = _pos; }

	bool operator==(const Agent& other) { return dist(other.pos) < sweep; }

	float dist(sf::Vector2f p) {
		return sqrtf(pow((p.x - pos.x), 2) + pow((p.y - pos.y), 2));
	}

	
};