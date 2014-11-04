
#include "PlayerEntity.h"
#include "Randomizer.h"
#include "Game.h"
#include "MapManager.h"
#include <iostream>
#include "frameData.h"
#include <SFML/Graphics/RectangleShape.hpp>


const bool PLAYER_SHOW_MARKERS =1;

using namespace std;

PlayerEntity::PlayerEntity(sf::Texture* pTexture)
    : mFacing(getRandomInt()%2)
    , mFrame(8)
    , mFramesUntilNextFrame(0)
    , mFramesUntilAction(getRandomInt()%600)
    ,mZ(0)
    ,mPosition(30,
               -32)
    ,mVelocity(0.f,0.f)
    ,mAcceleration(0.f,0.f)
    ,mIsFalling(false)
    ,mIsJumping(false)
    ,mIsMoving(false)
    ,mIsAttacking(false)
    ,mIsDucking(false)
    ,mIsClimbing(false)
{



    mSprite.setTexture(*pTexture);
    // set pixels to large!
    mSprite.setScale(2.f,2.f);


    // read different frame values from file.
    mFramesData.clear();
    tinyxml2::XMLDocument doc;
    doc.LoadFile( "assets/hoodie_spritesheet.xml" ); // open the map

    if(!checkDocError(doc))
    {
        return ;
    }

    tinyxml2::XMLElement* mapElement = doc.FirstChildElement( "TextureAtlas" );
    if (mapElement==0)
    {
        std::cout << "textureatlas not found!" << std::endl;
        return;
    }

    tinyxml2::XMLElement* nextElement = mapElement->FirstChildElement("sprite");
    if (mapElement==0)
    {
        std::cout << "sprite not found!" << std::endl;
        return;
    }

    std::string n;
    int x, y, w, h, oX, oY, oW, oH, mX;
    float pX, pY;
    while (nextElement!=0)
    {
        n = nextElement->Attribute("n");
        nextElement->QueryIntAttribute( "x", &x);
        nextElement->QueryIntAttribute( "y", &y);
        nextElement->QueryIntAttribute( "w", &w);
        nextElement->QueryIntAttribute( "h", &h);
        nextElement->QueryFloatAttribute("pX", &pX);
        nextElement->QueryFloatAttribute("pY", &pY);
        nextElement->QueryIntAttribute( "oX", &oX);
        nextElement->QueryIntAttribute( "oY", &oY);
        nextElement->QueryIntAttribute( "oW", &oW);
        nextElement->QueryIntAttribute( "oH", &oH);
        nextElement->QueryIntAttribute( "mX", &mX);
        mFramesData.push_back(frameData(n,x,y,w,h,pX,pY,oX,oY,oW,oH,mX));
        nextElement = nextElement->NextSiblingElement();
    }
}


void PlayerEntity::update(sf::Time deltaTime)
{

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


    // ducking is true if we are on the ground and pressing duck. false in either is not true.
    mIsDucking=!mIsFalling&&bDuckKeyPressed&&!mIsClimbing;


    // if we are no longer pressing the attack key and the attack animation is over, set attacking to false
    if(mFrame!=0&&mFrame!=1&&mFrame!=6&&mFrame!=7)
    {
        mIsAttacking=false;
    }

     static bool bAttackKeyWasPressed=false;

   // check for attack being pressed then attack!!
    if(bAttackKeyPressed&&!bAttackKeyWasPressed&&!mIsAttacking&&!mIsClimbing)
    {
        mIsAttacking=true;
        mFramesUntilNextFrame=10;
        mFrame=(mIsDucking)?6:0; // attack frame or duck attack frame

        // DO ATTACK


    }
    bAttackKeyWasPressed=bAttackKeyPressed;

    if (!bJumpKeyPressed&&!mIsFalling)
    {
        mIsJumping=false;
    }

     static bool bJumpKeyWasPressed=false;
         // delay until the wall can be gripped again
    const int climbAgainDelay = 5;
     static int framesSinceClimbing=climbAgainDelay;

     framesSinceClimbing++;



    if (bJumpKeyPressed&&(!mIsFalling||mIsClimbing)&&!bJumpKeyWasPressed&&!mIsDucking&&framesSinceClimbing>climbAgainDelay)
    {

        mIsJumping=true;
        if (mIsClimbing)
        {
            mIsClimbing=false;
            framesSinceClimbing=0;
             totalForces.y+=0.80*playerJumpForce;
              float lateralForce=(mFacing==RIGHT)?playerJumpForce:-playerJumpForce;
              totalForces.x+=0.33*lateralForce;
              mFacing=(mFacing==RIGHT)?LEFT:RIGHT;

        } else {
         totalForces.y+=playerJumpForce;
        }
    }
    bJumpKeyWasPressed=bJumpKeyPressed;



    // move left
    if (bA&&!bD&&!mIsDucking&&!mIsClimbing&&framesSinceClimbing>climbAgainDelay)
    {
        mFacing=LEFT;
        totalForces.x-=playerRunForce;
    }

    // move right
    if (bD&&!bA&&!mIsDucking&&!mIsClimbing&&framesSinceClimbing>climbAgainDelay)
    {
        mFacing=RIGHT;
        totalForces.x+=playerRunForce;
    }

    // is the origin on a vertically bordering block?
    int vMin=-TheGame::Instance()->getDisplayHeight(), vMax=TheGame::Instance()->getDisplayHeight()*2; // default vertical limit is offscreen
    int hMin=0, hMax=TheGame::Instance()->getDisplayWidth()-1;

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
        mFrame=9;
        mFramesUntilNextFrame=0;
        break;
    case FALLING_ATTACK:
        totalForces.y+=gravityForce;
        if ((mFrame==6||mFrame==0)&&mFramesUntilNextFrame<1)
        {
            mFrame=1;
            mFramesUntilNextFrame=FIRST_ATTACK_FRAMES;
        }

        else if ((mFrame==7||mFrame==1)&&mFramesUntilNextFrame<1)
        {
            mFrame=9; // falling
            mFramesUntilNextFrame=SECOND_ATTACK_FRAMES;
        }
        break;
    case IDLE:
        mFrame=8; // idle
        mFramesUntilNextFrame=0;
        break;
    case IDLE_ATTACK:
        if ((mFrame==6||mFrame==0)&&mFramesUntilNextFrame<1)
        {
            mFrame=1;
            mFramesUntilNextFrame=FIRST_ATTACK_FRAMES;
        }

        else if ((mFrame==7||mFrame==1)&&mFramesUntilNextFrame<1)
        {
            mFrame=8; //  idle
            mFramesUntilNextFrame=SECOND_ATTACK_FRAMES;
        }
        break;
    case MOVING:

        // switch running frames every n frames
        if (mFramesUntilNextFrame<1)
        {
            // running frames
            mFramesUntilNextFrame=8;
            mFrame++;
            if (mFrame>13||mFrame<11)
            {
                mFrame=11; // running
            }
        }
        break;
    case MOVING_ATTACK:
        if ((mFrame==6||mFrame==0)&&mFramesUntilNextFrame<1)
        {
            mFrame=1;
            mFramesUntilNextFrame=FIRST_ATTACK_FRAMES;
        }

        else if ((mFrame==7||mFrame==1)&&mFramesUntilNextFrame<1)
        {
            mFrame=11; // running
            mFramesUntilNextFrame=SECOND_ATTACK_FRAMES ;
        }
        break;
    case DUCK:
        mFrame=5; // duck
        mFramesUntilNextFrame=0;
        break;
    case DUCK_ATTACK:
        if ((mFrame==6||mFrame==0)&&mFramesUntilNextFrame<1)
        {
            mFrame=7;
            mFramesUntilNextFrame=FIRST_ATTACK_FRAMES;
        }

        else if ((mFrame==7||mFrame==1)&&mFramesUntilNextFrame<1)
        {
            mFrame=5; //  duck
            mFramesUntilNextFrame=SECOND_ATTACK_FRAMES;
        }
        break;
    case CLIMB:
    {
        // pick a randon frame 2 or 3
        if (mFrame!=2&&mFrame!=3)
        {
            mFrame=2+getRandomInt()%2;
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
    if (mPosition.y>TheGame::Instance()->getDisplayHeight()+64)
    {
        mPosition.x=30;
        mPosition.y=-32;
        mVelocity*=0.f;
        mAcceleration*=0.f;
    }



    mZ = int(mPosition.y); // fixed z=y since we fixed our origin offsets :)
    mFramesUntilNextFrame--;
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

    mSprite.setPosition(spriteOffsetX*2+mPosition.x+ oX*2, spriteOffsetY*2+mPosition.y+oY*2);

    TheGame::Instance()->getRenderTexture()->draw(mSprite);
    //TheGame::Instance()->drawMarker(mPosition.x, mPosition.y-64);

    float playerWidth = 24.f;
    float playerHeight = (mIsDucking)?32:48;
/*
    // draw bounding box
    sf::RectangleShape boundy = sf::RectangleShape(mPosition);
    boundy.setPosition(mPosition);
    boundy.setOutlineColor(sf::Color(0,0,0,255.f));
    boundy.setOutlineThickness(2);
    boundy.setFillColor(sf::Color(0,0,0,0));
    boundy.move(sf::Vector2f(-playerWidth*0.5f,-playerHeight));
    boundy.setSize(sf::Vector2f(playerWidth,playerHeight));
    TheGame::Instance()->getRenderTexture()->draw(boundy);

    //draw bounding box for weapon
    if (mIsAttacking)
    {
        float w=32.f;
        float h=16.f;


        if (mIsDucking)
        {
      boundy.setPosition(mPosition);
    boundy.setOutlineColor(sf::Color(255,0,0,255.f));
    boundy.setOutlineThickness(2);
    boundy.setFillColor(sf::Color(0,0,0,0));
    boundy.move(sf::Vector2f((mFacing==RIGHT)?(playerWidth*0.5):-w-(playerWidth*0.5),-playerHeight));
    boundy.setSize(sf::Vector2f(w,h));

        }
        else
        {
      boundy.setPosition(mPosition);
    boundy.setOutlineColor(sf::Color(255,0,0,255.f));
    boundy.setOutlineThickness(2);
    boundy.setFillColor(sf::Color(0,0,0,0));
    boundy.move(sf::Vector2f((mFacing==RIGHT)?(playerWidth*0.5):-w-(playerWidth*0.5),-playerHeight));
    boundy.setSize(sf::Vector2f(w,h));
        }
            TheGame::Instance()->getRenderTexture()->draw(boundy);
    } */

}
