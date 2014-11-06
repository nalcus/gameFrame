#ifndef __NPCEntity__
#define __NPCEntity__

#include "Entity.h"
#include "frameData.h"
#include <vector>

class NPCEntity : public Entity
{
public:

    NPCEntity(sf::Texture* pTexture);
    sf::Sprite* getSprite()
    {
        return &mSprite;
    }

    virtual void update(sf::Time deltaTime);

    virtual void render();

    virtual void receiveCollision(Entity * SourceEntity);

private:

    sf::Sprite mSprite;
    int mFacing;
    int mFrame;
    int mFramesUntilNextAnimationFrame;
    int mFramesUntilAction;
    bool mFalling;

    std::vector<frameData> mFramesData;
};




#endif // __NPCEntity__
