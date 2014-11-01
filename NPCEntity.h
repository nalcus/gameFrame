#ifndef __NPCEntity__
#define __NPCEntity__

#include "Entity.h"

enum
{
    LEFT=0, RIGHT
};


class NPCEntity : public Entity
{
public:

    NPCEntity(sf::Texture* pTexture);
    sf::Sprite* getSprite()
    {
        return &mSprite;
    }

    virtual void update(sf::Time deltaTime);
    virtual int getZ()
    {
        return mZ;
    }
    virtual void render();

private:
    sf::Vector2f mPosition;
    sf::Vector2f mVelocity;
    sf::Sprite mSprite;
    int mFacing;
    int mFrame;
    int mFramesUntilNextFrame;
    int mFramesUntilAction;
    bool mFalling;
    int mZ; // this is for z sort
};




#endif // __NPCEntity__
