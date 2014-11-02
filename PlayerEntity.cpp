
#include "PlayerEntity.h"
#include "Randomizer.h"
#include "Game.h"
#include "MapManager.h"
#include <iostream>
#include "frameData.h"


using namespace std;

PlayerEntity::PlayerEntity(sf::Texture* pTexture)
    : mFacing(getRandomInt()%2)
    , mFrame(0)
    , mFramesUntilNextFrame(0)
    , mFramesUntilAction(getRandomInt()%600)
    ,mZ(0)
    ,mPosition(30,
               -32)
    ,mVelocity(0.f,0.f)
    ,mAcceleration(0.f,0.f)
    ,mFalling(false)
    ,mIsJumping(false)
    ,mWasJumping(false)
    ,mIsMoving(false)
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


    if (mPosition.y>600)
    {



        mPosition.x=30;
        mPosition.y=-32;
        mVelocity*=0.f;
        mAcceleration*=0.f;
    }

    //mFrameTime+=deltaTime;

    //mSprite.move(100.f*deltaTime.asSeconds(),0.f);

    //if framesuntil action is up do

    int tileUnderfoot=TheMapManager::Instance()->getClipAtScreenPosition(mPosition.x, mPosition.y);

    // get the tile under foot.

    int r=mPosition.y/32;
    int c=mPosition.x/32;
    int rOffset=int(mPosition.y)%32;
    int cOffset=int(mPosition.x)%32;

    bool bA=sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool bD=sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    bool bJumpKeyPressed=sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

    const float playerRunForce = 0.05f;
    const float playerJumpForce = -3.5f;

    if (!bJumpKeyPressed&&!mFalling&&mWasJumping)
    {
        mIsJumping=false;
    }

    if (bJumpKeyPressed&&!mFalling&&!mWasJumping)
    {
        totalForces.y+=playerJumpForce;
        mIsJumping=true;
    }


    if (bA&&!bD)
    {
        mFacing=LEFT;
        totalForces.x-=playerRunForce;
    }

    if (bD&&!bA)
    {
        mFacing=RIGHT;
        totalForces.x+=playerRunForce;
    }

    // is the origin on a vertically bordering block?
    int vl=480*2; // default vertical limit is offscreen
    int hMin=0, hMax=639;




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
                    , mPosition.y)==1)
        {
            hMin=mPosition.x-cOffset;
        }




    // these block types are vertically supporting
    switch (tileUnderfoot)
    {
    case 2:
    case 3:
    case 4:
    case 5:
    case 8:

        vl=r*32+16;
    }



// if your y position if lesser than your current vertical limit then you are off the ground are are falling.
    if (mPosition.y<vl)
    {
        mFalling=true;
    }
    else
    {
        mPosition.y=vl;
        mFalling=false;
    }

    const float gravityForce = 0.125;

    if (mFalling)
    {
        totalForces.y+=gravityForce;
        mFrame=3;
        mFramesUntilNextFrame=0;
    }
    else
    {

        // if we were falling, switch to standing
        if (mFrame==3)
        {
            mFrame=12;
            mFramesUntilNextFrame=0;
        }

        // if our velocity is basically still then stand else run
        if(mVelocity.x<0.1&&mVelocity.x>-0.1)
        {
            mFrame=12;
            mFramesUntilNextFrame=0;
        }
        else
        {
            // switch running frames every n frames
            if (--mFramesUntilNextFrame<1)
            {
                // running frames
                mFramesUntilNextFrame=5+getRandomInt()%5;
                mFrame++;
                if (mFrame>2)
                {
                    mFrame=0;
                }
            }
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
    if (mVelocity.x>0.f&&mPosition.x>=hMax)
    {
        mPosition.x=hMax;
        mVelocity.x=0;
        mAcceleration.x=0;
    } else if (mVelocity.x<0.f&&mPosition.x<=hMin)
    {
                mPosition.x=hMin;
        mVelocity.x=0;
        mAcceleration.x=0;
    }

    // correct if you are just below the ground for some reason
    if (mPosition.y>vl)
    {
        mPosition.y=vl;
        mVelocity.y=0;
        mAcceleration.y=0;
    }


    mZ = int(mPosition.y); // fixed z=y since we fixed our origin offsets :)


    mWasJumping=mIsJumping;

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
        spriteOffsetX=10;

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
    TheGame::Instance()->drawMarker(mPosition.x, mPosition.y);

}
