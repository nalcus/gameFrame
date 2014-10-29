#ifndef __Entity__
#define __Entity__

#include <SFML/Graphics.hpp>


class Entity : public sf::Transformable
{
public:

    Entity();
    sf::Sprite* getSprite(){return 0;}
    virtual void update(sf::Time deltaTime);
virtual void render();
 virtual int getZ() {return 0;}
};



#endif // __Entity__
