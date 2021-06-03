#include "Agent.h"
#include "SFML/Graphics/RenderTarget.hpp"
#include <iostream>
#include <set>

auto cmp = [](Agent* a, Agent* b) {return a->getF() < b->getF(); };

const static float sweep = 10;
const static sf::Vector2f dirs[] = {
	{-sweep, 0},		{sweep, 0},
	{0, sweep},			{0, -sweep},
	{-sweep, sweep},	{sweep, sweep},
	{-sweep, -sweep},	{sweep, -sweep},
};

void renderAgents(const std::vector<Agent*>& v, sf::RenderWindow* win) {
	for (Agent* a : v)
		a->render(win);
}

bool inClosed(const Agent* agent, const std::vector<Agent*>& v) {
	for (Agent* a : v)
		if (*a == *agent)
			return true;
	return false;
}

Agent* inOpen(const Agent* agent, const std::set<Agent*, decltype(cmp)>& s) {
	for (Agent* a : s)
		if (*a == *agent)
			return a;
	return nullptr;
}

void reset(int& count, std::vector<Agent*>& states, std::vector<Agent*>& closed, std::set<Agent*, decltype(cmp)>& open) {
	count = 0;
	closed.clear();
	states.clear();
	open.clear();
}

bool inRange(float x, float y, const sf::Image& img) {
	return ((x >= 0 && x < img.getSize().x) && (y >= 0 && y < img.getSize().y));
}

bool notAccessible(float x, float y, const sf::Image& img) {
	return (img.getPixel(x, y) == sf::Color::Black);
}

bool AStar(std::vector<Agent*>& states, std::vector<Agent*>& closed, std::set<Agent*, decltype(cmp)>& open, int sweep, sf::RenderWindow* win, sf::Image& img, std::vector<Agent*>& path) {

	if (open.size() > 0) {
		Agent* curr = *open.begin();
		open.erase(open.begin());
		curr->setColor(sf::Color::Red);
		closed.push_back(curr);

		if (*curr == *states[1]) {
			Agent* hold = curr;
			path.push_back(states[0]);
			while (hold) {
				hold->setColor(sf::Color::Green);
				path.push_back(hold);
				hold = hold->getParent();
			}
			path.push_back(states[1]);
			return true;
		}

		for (int i = 0; i < 8; i++) {
			Agent* n = new Agent(*curr);
			n->setPosition(curr->getPosition() + dirs[i]);
			float x = n->getPosition().x, y = n->getPosition().y;
			if (inClosed(n, closed) || !inRange(x, y, img) || notAccessible(x, y, img)) {
				delete n;
				continue;
			}

			Agent* iOpen;
			if (!(iOpen = inOpen(n, open)) || n->getF() < curr->getF()) {
				n->setG(curr->getG());
				n->setH(states[1]->getPosition());
				n->setParent(curr);
				if (iOpen)
					open.erase(iOpen);
				open.insert(n);
			}
		}
	}

	return false;
}

int main() {

	sf::Image img;
	if (!img.loadFromFile("map.png"))
		return -1;

	sf::Texture tex;
	tex.loadFromImage(img);
	tex.setSmooth(true);
	sf::Sprite sp;
	sp.setTexture(tex, true);

	sf::RenderWindow window(sf::VideoMode(sp.getTexture()->getSize().x, sp.getTexture()->getSize().y), "AStar");
	
	std::vector<Agent*> agents;
	std::vector<Agent*> path;
	bool flag = true;
	int count = 0;

	std::vector<Agent*> closed;
	std::set<Agent*, decltype(cmp)> open(cmp);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::MouseButtonPressed && count < 2) {
				float mX = event.touch.x;
				float mY = event.touch.y;
				if (!notAccessible(mX, mY, img)) {
					if (count == 0) {
						agents.push_back(new Agent(sf::Color::Blue, sf::Vector2f(mX, mY), sweep));
						open.insert(agents[0]);
					}
					else
						agents.push_back(new Agent(sf::Color::Cyan, sf::Vector2f(mX, mY), sweep));
					count++;
				}
			}
		}

		if (count == 2) {
			if (path.size() > 0)
				path.clear();
			if (AStar(agents, closed, open, 8, &window, img, path))
				reset(count, agents, closed, open);
		}

		if (!open.size() && count == 2)
			reset(count, agents, closed, open);

		window.clear();
		window.draw(sp);
		renderAgents(agents, &window);
		if(closed.size() > 0)
			renderAgents(closed, &window);
		else if(path.size() > 0)
			renderAgents(path, &window);
		window.display();
	}

	return 0;
}