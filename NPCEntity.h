#ifndef __NPCEntity__
#define __NPCEntity__

#include "Entity.h"

enum {
    SOUTH=0, NORTH, WEST, EAST
};


class NPCEntity : public Entity
{
public:

    NPCEntity(sf::Texture* pTexture);
    sf::Sprite* getSprite(){return &mSprite;}

    virtual void update(sf::Time deltaTime);
    virtual int getZ() {return mZ;}
    virtual void render();

private:
    sf::Vector2f mPosition;
    sf::Sprite mSprite;
    int mRow;
    int mCol;
    int mFacing;
    int mFrame;
    int mFramesUntilNextFrame;
    int mFramesUntilAction;

    int mZ; // this is for z sort
};




#endif // __NPCEntity__
