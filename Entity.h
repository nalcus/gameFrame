#ifndef __Entity__
#define __Entity__

#include <SFML/Graphics.hpp>
#include <string>

enum
{
    LEFT=0, RIGHT
};

class Entity : public sf::Transformable
{
public:

    Entity();
    virtual sf::Sprite* getSprite()=0;
    virtual void update(sf::Time deltaTime)=0;
    virtual void render()=0;
    virtual int getZ()=0;

protected:
    std::string EntityType;
};



#endif // __Entity__
