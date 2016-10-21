#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#define SCALE 30 //pass from pixels to box2D positions

void createGround(b2World& World, float X, float Y) {
  b2BodyDef BodyDef;
  BodyDef.position = b2Vec2(X / 30.f, Y / 30.f);
  BodyDef.type = b2_staticBody;
  b2Body* Body = World.CreateBody(&BodyDef);
  b2PolygonShape Shape;
  Shape.SetAsBox((800.f / 2) / SCALE, (16.f / 2) / SCALE); // Creates a box shape. Divide your desired width and height by 2.
  b2FixtureDef FixtureDef;
  FixtureDef.density = 0.f;  // Sets the density of the body
  FixtureDef.shape = &Shape; // Sets the shape
  Body->CreateFixture(&FixtureDef); // Apply the fixture definition
}

void createBox(b2World& World, float mouseX, float mouseY) {
  b2BodyDef BodyDef;
  BodyDef.position = b2Vec2(mouseX / SCALE, mouseY / SCALE);
  BodyDef.type = b2_dynamicBody;
  b2Body* Body = World.CreateBody(&BodyDef);

  b2PolygonShape Shape;
  Shape.SetAsBox((32.f / 2) / SCALE, (32.f / 2) / SCALE);
  b2FixtureDef FixtureDef;
  FixtureDef.density = 1.f;
  FixtureDef.friction = 0.7f;
  FixtureDef.shape = &Shape;
  Body->CreateFixture(&FixtureDef);
}

int main() {
  // Create the main window
  sf::RenderWindow window(sf::VideoMode(800, 600, 32), "SFML window"); //width, height, bits per pixel

  window.setFramerateLimit(60);

  //preparing world

  b2Vec2 gravity(0.0f, 9.8f);
  b2World world(gravity);

  sf::Texture GroundTexture;
  sf::Texture BoxTexture;
  //GroundTexture.loadFromFile("../../../../assets/images/ground.png");
  //BoxTexture.loadFromFile("../../../../assets/images/box.png");
  GroundTexture.loadFromFile("assets/images/ground.png");
  BoxTexture.loadFromFile("assets/images/box.png");

  createGround(world, 400.f, 500.f);

  while (window.isOpen()) {
    // Process events
    sf::Event event;
    while (window.pollEvent(event)) {
      // Close window: exit
      if (event.type == sf::Event::Closed)
        window.close();
    }

    // Clear screen
    window.clear(sf::Color::White);
    
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      int MouseX = sf::Mouse::getPosition(window).x;
      int MouseY = sf::Mouse::getPosition(window).y;
      createBox(world, (float)MouseX, (float)MouseY);
    }

    for (b2Body* BodyIterator = world.GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext()) {
      if (BodyIterator->GetType() == b2_dynamicBody) {
        sf::Sprite Sprite;
        Sprite.setTexture(BoxTexture);
        Sprite.setOrigin(16.f, 16.f);
        Sprite.setPosition(SCALE * BodyIterator->GetPosition().x, SCALE * BodyIterator->GetPosition().y);
        Sprite.setRotation(BodyIterator->GetAngle() * 180 / b2_pi);
        window.draw(Sprite);
      } else {
        sf::Sprite GroundSprite;
        GroundSprite.setTexture(GroundTexture);
        GroundSprite.setOrigin(400.f, 8.f);
        GroundSprite.setPosition(BodyIterator->GetPosition().x * SCALE, BodyIterator->GetPosition().y * SCALE);
        GroundSprite.setRotation(180 / b2_pi * BodyIterator->GetAngle());
        window.draw(GroundSprite);
      }
    }
    /** Simulate the world */
    world.Step(1 / 60.f, 8, 3);

    // Update the window
    window.display();
  }
  return EXIT_SUCCESS;
}