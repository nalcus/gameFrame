#ifndef __PlayerEntity__
#define __PlayerEntity__

#include "Entity.h"
#include "frameData.h"
#include <vector>

class PlayerEntity : public Entity
{
    public:

    PlayerEntity(sf::Texture* pTexture);
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
    std::vector<frameData> mFramesData;
};


#endif // __PlayerEntity__
