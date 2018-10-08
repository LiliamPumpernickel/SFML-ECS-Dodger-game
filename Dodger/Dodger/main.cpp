#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <math.h>

using namespace std;

class GameObject;
class Component;


typedef vector<Component*> componentVector;
typedef vector<GameObject*> GOVector;


class Component {
	GameObject* parentGameObject;
	
public:
	virtual ~Component() {};
	void setParentGameObject(GameObject& parentGO) {
		parentGameObject = &parentGO;
	};
	virtual void Update(float deltaTime) {

	}
	GameObject* getParent() {
		return parentGameObject;
	}
};

class GameObject{
	private:
		componentVector componentArray;
	public:
		string name;
		GameObject(string&& string) :
			name(string) {};

		void addComponent(Component* component) {
			component->setParentGameObject(*this);
			componentArray.push_back(component);
		}

		void Update(float deltaTime) {
			for (auto c : componentArray) {
				c->Update(deltaTime);
			}
		}

		template<typename T>
		T* GetComponent()
		{
			for (auto i : componentArray)
			{
				T* c = dynamic_cast<T*>(i);
				if (c != nullptr)
					return c;
			}
			return nullptr;
		}


};





class Transform : public Component {
	public:
		float x, y;
};

class Sprite : public Component {
	
	sf::Texture _texture;
	public:
		sf::Sprite sprite;
		Sprite(sf::Texture *texture) : _texture(*texture) { sprite.setTexture(_texture); };
		void setTexture(sf::Texture&& texture) {
			_texture = texture;
		}
		void UpdateTexture() {
			sprite.setTexture(_texture);
		}


};

class Fall : public Component {
	public:
		void Update(float deltaTime) {
			Transform* t = getParent()->GetComponent<Transform>();
			t->y += 200.f*deltaTime;
			if (t->y > 800) {
				t->y = (rand() % 700 + 100)*-1; t->x = (rand() % 760 + 1);
			}
			Sprite* s = getParent()->GetComponent<Sprite>();
			s->sprite.setPosition(t->x, t->y);
		}
};


void moveAll(componentVector* fallComponents, float deltaTime) {
	for (auto fc : *fallComponents) {
		fc->Update(deltaTime);
	}
}



int main()
{
	sf::Clock clock;
	GOVector gameObjects;

	sf::RenderWindow window(sf::VideoMode(800, 800), "SFML works!");
	
	sf::Texture texture;
	texture.loadFromFile("Heal Totem Sprite Sheet.png", sf::IntRect(0, 0, 32, 32));
	GameObject go("player");
	Sprite sprite(&texture);
	go.addComponent(&sprite);
	Sprite* mysprite = go.GetComponent<Sprite>();
	mysprite->sprite.setScale(sf::Vector2f(3.f, 3.f));

	Transform transform;
	transform.x = 450.f;
	transform.y = 700.f;

	go.addComponent(&transform);
	Transform* tr = go.GetComponent<Transform>();

	mysprite->sprite.setPosition(tr->x, tr->y);

	gameObjects.push_back(&go);

	texture.loadFromFile("11_fire_spritesheet.png", sf::IntRect(40, 40, 50, 50));

	componentVector fallComponents;


	for ( int i = 0; i < 10000; i++)
	{
		GameObject* gameObject = new GameObject("obstacle");
		gameObject->addComponent(new Sprite(&texture));
		Transform* t = new Transform();
		t->x = rand() % 760 + 1;
		t->y = (rand() % 700 + 100)*-1;
		gameObject->addComponent(t);
		Sprite* s = gameObject->GetComponent<Sprite>();
		s->sprite.setScale(sf::Vector2f(1.f, 1.f));
		s->sprite.setPosition(t->x, t->y);
		Fall* fall = new Fall();
		gameObject->addComponent(fall);
		gameObjects.push_back(gameObject);
		fallComponents.push_back(fall);
	}

	sf::Time time;

	while (window.isOpen())
	{
		time = clock.restart();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				tr = go.GetComponent<Transform>();
				tr->x -= 20.f ;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				tr = go.GetComponent<Transform>();
				tr->x += 20.f;
			}
		}


		




		mysprite->sprite.setPosition(tr->x, tr->y);

		window.clear();
		window.draw(mysprite->sprite);
		moveAll(&fallComponents, time.asSeconds());
		for (auto go : gameObjects) {
			if (go->name == "obstacle") {
				Sprite* s = go->GetComponent<Sprite>();
				window.draw(s->sprite);
			}
		}
		
		
		window.display();

	}

	return 0;
}