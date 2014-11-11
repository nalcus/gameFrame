
#include "PlayerEntity.h"
#include "Randomizer.h"
#include "Game.h"
#include <iostream>
#include "frameData.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include "Projectile.h"
#include "EntityManager.h"
#include "MapManager.h"



using namespace std;

PlayerEntity::PlayerEntity(sf::Texture* pTexture)
    : mFacing(getRandomInt()%2)
    , mFrame(0)
    , mFramesUntilNextAnimationFrame(0)
    , mFramesUntilAction(getRandomInt()%600)
    ,mIsFalling(false)
    ,mIsJumping(false)
    ,mIsMoving(false)
    ,mIsAttacking(false)
    ,mIsDucking(false)
    ,mIsClimbing(false)
    ,mIsHurting(false)
    ,mJustGotHit(false)
    ,mIsRolling(false)
{
    // set entity type
    mType = "PLAYER";

    // player has no parent entity
    mParentEntity=0;

    // we're alive!!
    mActive=true;
    mAge=0;

    mFramesUntilHealthy=0;

    // initial position and momentum
    mPosition.x=30;
    mPosition.y=-32;
    mVelocity=sf::Vector2f(0.f,0.f);
    mAcceleration=sf::Vector2f(0.f,0.f);
    mSize=sf::Vector2f(24.f,48.f);

    // set texture to argument
    mSprite.setTexture(*pTexture);

    // set pixels to large!
    mSprite.setScale(2.f,2.f);


    readFramesDataFromFile("assets/hoodie_spritesheet.xml");

    mFrame=findFrameNamed("idle_1.png");
}


void PlayerEntity::update(sf::Time deltaTime)
{


    Entity::update(deltaTime);

    sf::Vector2f totalForces=sf::Vector2f(0.f,0.f);





    // get the tile under foot.

    int r=mPosition.y/32;
    int c=mPosition.x/32;
    int rOffset=int(mPosition.y)%32;
    int cOffset=int(mPosition.x)%32;

    bool bA=sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool bD=sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    bool bDuckKeyPressed=sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    bool bJumpKeyPressed=sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    bool bAttackKeyPressed=sf::Keyboard::isKeyPressed(sf::Keyboard::Space);


    const float playerRunForce = 0.05f;
    const float playerJumpForce = -3.3f;
    const int climbAgainDelay = 5;
    static int framesSinceClimbing=climbAgainDelay;

    framesSinceClimbing++;
    mFramesUntilHealthy--;

    // we only want to roll until we first hit the ground
    if (!mIsFalling&&mIsRolling)
    {
        mIsRolling=false;
    }

    // we just got injured so run injury script
    if (mJustGotHit)
    {
        mFramesUntilHealthy=120;
        mVelocity=sf::Vector2f(0.f,0.f);
        mAcceleration=sf::Vector2f(0.f,0.f);
        mIsClimbing=false;
        framesSinceClimbing=0;
        totalForces.y+=0.33*playerJumpForce;
        float lateralForce=(mFacing==RIGHT)?playerJumpForce:-playerJumpForce;
        totalForces.x+=0.33*lateralForce;
        mJustGotHit=false;
        mIsRolling=true;
    }


        mIsHurting=(mFramesUntilHealthy>1);



    // ducking is true if we are on the ground and pressing duck. false in either is not true.
    mIsDucking=!mIsFalling&&bDuckKeyPressed&&!mIsClimbing;


    // if we are no longer pressing the attack key and the attack animation is over, set attacking to false
    if(mFrame!=findFrameNamed("attack_1a.png")
       &&mFrame!=findFrameNamed("attack_1b.png")
       &&mFrame!=findFrameNamed("duckattack_1a.png")
       &&mFrame!=findFrameNamed("duckattack_1b.png"))
    {
        mIsAttacking=false;
    }

    static bool bAttackKeyWasPressed=false;

    // check for attack being pressed then attack!!
    if(bAttackKeyPressed&&!bAttackKeyWasPressed&&!mIsAttacking&&!mIsClimbing)
    {
        mIsAttacking=true;
        // in case we were rolling before we won't be now
        mIsRolling=false;
        mFramesUntilNextAnimationFrame=10;
        mFrame=(mIsDucking)
            ?findFrameNamed("duckattack_1a.png")
            :findFrameNamed("attack_1a.png"); // attack frame or duck attack frame

        float projectileOffset=16.f+(mSize.x*0.5f);
        // DO ATTACK
        Projectile * tempsword=new Projectile(0
                                              , 32.f
                                              ,16.f
                                              , mPosition.x+((mFacing==RIGHT)?projectileOffset:-projectileOffset)
                                              ,16.f+mPosition.y-(mSize.y)+((mIsDucking)?16:0)
                                              ,0.f
                                              ,0.f
                                              ,this);
        TheEntityManager::Instance()->pushEntity(tempsword);


    }
    bAttackKeyWasPressed=bAttackKeyPressed;

    // if we are not falling and jump key is not pressed
    if (!bJumpKeyPressed&&!mIsFalling)
    {
        mIsJumping=false;
    }

    static bool bJumpKeyWasPressed=false;
    // delay until the wall can be gripped again



    if (bJumpKeyPressed&&(!mIsFalling||mIsClimbing)&&!bJumpKeyWasPressed&&!mIsDucking&&framesSinceClimbing>climbAgainDelay)
    {

        mIsJumping=true;
        // in case we were rolling before we won't be now
        mIsRolling=false;
        if (mIsClimbing)
        {
            mIsClimbing=false;
            framesSinceClimbing=0;
            totalForces.y+=0.80*playerJumpForce;
            float lateralForce=(mFacing==RIGHT)?playerJumpForce:-playerJumpForce;
            totalForces.x+=0.33*lateralForce;
            mFacing=(mFacing==RIGHT)?LEFT:RIGHT;

        }
        else
        {
            totalForces.y+=playerJumpForce;
        }
    }
    bJumpKeyWasPressed=bJumpKeyPressed;



    // move left
    if (bA&&!bD&&!mIsDucking&&!mIsClimbing&&framesSinceClimbing>climbAgainDelay)
    {
        if (!mIsFalling)
            {
                mFacing=LEFT;
        }
        totalForces.x-=playerRunForce;
    }

    // move right
    if (bD&&!bA&&!mIsDucking&&!mIsClimbing&&framesSinceClimbing>climbAgainDelay)
    {
               if (!mIsFalling)
            {
        mFacing=RIGHT;
            }
        totalForces.x+=playerRunForce;
    }

    // is the origin on a vertically bordering block?
    int vMin=-256;
    int vMax=256+TheMapManager::Instance()->getMapHeight()*32;
    int hMin=0;
    int hMax=-1+TheMapManager::Instance()->getMapWidth()*32;

    float playerHeight = (mIsDucking)?32:48;
    int tileUnderfoot=TheMapManager::Instance()->getClipAtScreenPosition(mPosition.x, mPosition.y);
    int tileUnderHead=TheMapManager::Instance()->getClipAtScreenPosition(mPosition.x, mPosition.y-playerHeight);
    int tileAboveHead=TheMapManager::Instance()->getClipAtScreenPosition(mPosition.x, mPosition.y-64);

    // sam,e check for the right
    if (TheMapManager::Instance()->
            getClipAtScreenPosition(
                mPosition.x+32
                , mPosition.y)==1||

            TheMapManager::Instance()->
            getClipAtScreenPosition(
                mPosition.x+32
                , mPosition.y-playerHeight)==1
       )
    {
        hMax=c*32+32;
    }

    // blocking on the right?
    switch (tileAboveHead)
    {
    case 1: // solid
        vMin=(r*32)+32;
        break;
    }

    switch (tileUnderHead)
    {
    case 1: // solid
        vMin=(r*32)+32;
        break;
    case 15: // upper right corner
        hMax=c*32+16;
    }
    //cout << "tileUNderHead: " <<tileUnderHead << "\t vMin: " << vMin << endl;




    // blocking on the right?
    switch (tileUnderfoot)
    {
    case 3: // right side wall with floor
    // case 5: // corner
    case 6: // right side wall no floor

        hMax=c*32+16;
        break;
    // case 4: corner
    case 7:
    case 8:
        hMin=c*32+16;
        break;
    }

    // look to see if we have solid tile to the left
    if (TheMapManager::Instance()->
            getClipAtScreenPosition(
                mPosition.x-32
                , mPosition.y)==1||

            TheMapManager::Instance()->
            getClipAtScreenPosition(
                mPosition.x-32
                , mPosition.y-playerHeight)==1
       )
    {
        hMin=mPosition.x-cOffset;
    }





    // these block types are vertically supporting
    switch (tileUnderfoot)
    {
    case 1:
        vMax=r*32; // if you somehow got here...
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 8:
        vMax=r*32+16;
    }



// if your y position if lesser than your current vertical limit then you are off the ground are are falling.
    if (mPosition.y<vMax)
    {
        mIsFalling=true;
    }
    else
    {
        mPosition.y=vMax;
        mIsFalling=false;
    }



    const float gravityForce = 0.125;


    // *********************************** ANIMATION *********************************************************

    enum
    {
        IDLE=0,
        IDLE_ATTACK,
        MOVING,
        MOVING_ATTACK,
        FALLING,
        FALLING_ATTACK,
        DUCK,
        DUCK_ATTACK,
        CLIMB
    };

    int state=(mIsClimbing)?CLIMB:(mIsDucking)?(mIsAttacking?DUCK_ATTACK:DUCK):((mIsFalling)?(mIsAttacking?FALLING_ATTACK:FALLING):
              (mVelocity.x<-0.1f||mVelocity.x>0.1f)?(mIsAttacking?MOVING_ATTACK:MOVING) :(mIsAttacking?IDLE_ATTACK:IDLE));


    const int FIRST_ATTACK_FRAMES = 7;
    const int SECOND_ATTACK_FRAMES = 7;


    switch (state)
    {
    case FALLING:
        totalForces.y+=gravityForce;
        mFrame=(mIsRolling)?findFrameNamed("roll_1.png"):findFrameNamed("jump_1.png");
        mFramesUntilNextAnimationFrame=0;
        break;
    case FALLING_ATTACK:
        totalForces.y+=gravityForce;
        if ((mFrame==findFrameNamed("duckattack_1a.png")
             ||mFrame==findFrameNamed("attack_1a.png"))
            &&mFramesUntilNextAnimationFrame<1)
        {
            mFrame=findFrameNamed("attack_1b.png");
            mFramesUntilNextAnimationFrame=FIRST_ATTACK_FRAMES;
        }

        else if ((mFrame==findFrameNamed("duckattack_1b.png")||mFrame==findFrameNamed("attack_1b.png"))&&mFramesUntilNextAnimationFrame<1)
        {
            mFrame=findFrameNamed("jump_1.png"); // falling
            mFramesUntilNextAnimationFrame=SECOND_ATTACK_FRAMES;
        }
        break;
    case IDLE:
        mFrame=findFrameNamed("idle_1.png"); // idle
        mFramesUntilNextAnimationFrame=0;
        break;
    case IDLE_ATTACK:
        if ((mFrame==findFrameNamed("duckattack_1a.png")||mFrame==findFrameNamed("attack_1a.png"))&&mFramesUntilNextAnimationFrame<1)
        {
            mFrame=findFrameNamed("attack_1b.png");
            mFramesUntilNextAnimationFrame=FIRST_ATTACK_FRAMES;
        }

        else if ((mFrame==findFrameNamed("duckattack_1b.png")||mFrame==findFrameNamed("attack_1b.png"))&&mFramesUntilNextAnimationFrame<1)
        {
            mFrame=findFrameNamed("idle_1.png"); //  idle
            mFramesUntilNextAnimationFrame=SECOND_ATTACK_FRAMES;
        }
        break;
    case MOVING:

        // switch running frames every n frames
        if (mFramesUntilNextAnimationFrame<1)
        {
            // running frames
            mFramesUntilNextAnimationFrame=8;
            mFrame++;
            if (mFrame>findFrameNamed("run_1c.png")||mFrame<findFrameNamed("run_1a.png"))
            {
                mFrame=findFrameNamed("run_1a.png"); // running
            }
        }
        break;
    case MOVING_ATTACK:
        if ((mFrame==findFrameNamed("duckattack_1a.png")||mFrame==findFrameNamed("attack_1a.png"))&&mFramesUntilNextAnimationFrame<1)
        {
            mFrame=findFrameNamed("attack_1b.png");
            mFramesUntilNextAnimationFrame=FIRST_ATTACK_FRAMES;
        }

        else if ((mFrame==findFrameNamed("duckattack_1b.png")||mFrame==findFrameNamed("attack_1b.png"))&&mFramesUntilNextAnimationFrame<1)
        {
            mFrame=findFrameNamed("run_1a.png"); // running
            mFramesUntilNextAnimationFrame=SECOND_ATTACK_FRAMES ;
        }
        break;
    case DUCK:
        mFrame=findFrameNamed("duck_1.png"); // duck
        mFramesUntilNextAnimationFrame=0;
        break;
    case DUCK_ATTACK:
        if ((mFrame==findFrameNamed("duckattack_1a.png")||mFrame==findFrameNamed("attack_1a.png"))&&mFramesUntilNextAnimationFrame<1)
        {
            mFrame=findFrameNamed("duckattack_1b.png");
            mFramesUntilNextAnimationFrame=FIRST_ATTACK_FRAMES;
        }

        else if ((mFrame==findFrameNamed("duckattack_1b.png")||mFrame==findFrameNamed("duckattack_1b.png"))&&mFramesUntilNextAnimationFrame<1)
        {
            mFrame=findFrameNamed("duck_1.png"); //  duck
            mFramesUntilNextAnimationFrame=SECOND_ATTACK_FRAMES;
        }
        break;
    case CLIMB:
    {
        // pick a randon frame 2 or 3
        if (mFrame!=findFrameNamed("climb_1a.png")&&mFrame!=findFrameNamed("climb_1b.png"))
        {
             // in case we were rolling before we won't be now

            mFrame=(getRandomInt()%2>0)?findFrameNamed("climb_1a.png"):findFrameNamed("climb_1b.png");
        }
        break;
    }

    }


    // not falling anymore go to idle frame
    // or if not running


    //loco-mote!!
    // friction
    mAcceleration*=0.9f;
    mAcceleration+=totalForces;
    mVelocity*=0.9f;
    mVelocity+=mAcceleration;
    mPosition+=mVelocity;



    // correct horizontal border crossing
    if ((mVelocity.x>0.f)&&mPosition.x>=hMax)
    {
        mPosition.x=hMax;
        mVelocity.x=0;
        mAcceleration.x=0;
        if (mIsFalling&&hMax<(TheGame::Instance()->getDisplayWidth()-1)&&framesSinceClimbing>climbAgainDelay)
        {
            mIsClimbing=true;
            mVelocity.y=0;
            mAcceleration.y=0;
            mIsRolling=false;
            mFacing=RIGHT;
        }
    }
    else if (mVelocity.x<0.f&&mPosition.x<=hMin)
    {
        mPosition.x=hMin;
        mVelocity.x=0;
        mAcceleration.x=0;
        if (mIsFalling&&hMin>0&&framesSinceClimbing>climbAgainDelay)
        {
            mIsClimbing=true;
            mVelocity.y=0;
            mAcceleration.y=0;
            mIsRolling=false;
            mFacing=LEFT;
        }
    }

    // correct if you are just below the ground for some reason
    if (mPosition.y>vMax)
    {
        mPosition.y=vMax;
        mVelocity.y=0;
        mAcceleration.y=0;
    }

    if (mPosition.y<=vMin)
    {
        mPosition.y=vMin;
        mVelocity.y=0;
        mAcceleration.y=0;
    }

    // if we fell off the screen start over
    if (mPosition.y>(128+TheMapManager::Instance()->getMapHeight()*32))
    {
        mPosition.x=30;
        mPosition.y=-32;
        mVelocity*=0.f;
        mAcceleration*=0.f;
    }



    mFramesUntilNextAnimationFrame--;
}

void PlayerEntity::render()
{
    int x=0, y=0, w=0, h=0, oX=0, oY=0; // these we'll eventually read from xml
    int spriteOffsetX=0, spriteOffsetY=-28;

    if (mFacing==RIGHT)
    {
        spriteOffsetX = -24;

        x=mFramesData.at(mFrame).x;
        y=mFramesData.at(mFrame).y;
        w=mFramesData.at(mFrame).w;
        h=mFramesData.at(mFrame).h;
        oX=mFramesData.at(mFrame).oX;
        oY=mFramesData.at(mFrame).oY;
    }

    else if (mFacing==LEFT)
    {
        spriteOffsetX=0;

        x=mFramesData.at(mFrame).x+mFramesData.at(mFrame).w;
        y=mFramesData.at(mFrame).y;
        w=0-mFramesData.at(mFrame).w;
        h=mFramesData.at(mFrame).h;
        oX=-mFramesData.at(mFrame).oX+mFramesData.at(mFrame).mX;
        oY=mFramesData.at(mFrame).oY;
    }

    sf::IntRect rect = sf::IntRect(x,y,w,h);
    mSprite.setTextureRect(rect);



    mSprite.setPosition(-TheGame::Instance()->mCameraOffset.x+
                        spriteOffsetX*2+mPosition.x+ oX*2,
                        -TheGame::Instance()->mCameraOffset.y+
                        spriteOffsetY*2+mPosition.y+oY*2);

    if (!(mIsHurting&&(mAge%10<5))) {
    TheGame::Instance()->getRenderTexture()->draw(mSprite);
    }
    //TheGame::Instance()->drawMarker(mPosition.x, mPosition.y-64);
}

void PlayerEntity::receiveCollision(Entity * SourceEntity)
{
    std::string En="ENEMY";

    if (*SourceEntity->getType()==En&&!mIsHurting)
    {
        mJustGotHit=true;
    }
}


