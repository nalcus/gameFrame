#include "EntityManager.h"
#include "Game.h"
#include <iostream>
#include "Entity.h"
#include <string>
#include "MapManager.h"

EntityManager* EntityManager::s_pInstance = 0;

void EntityManager::handleCollisionBetween(Entity* FirstEntity, Entity * SecondEntity)
{
    FirstEntity->receiveCollision(SecondEntity);
    SecondEntity->receiveCollision(FirstEntity);
}

void    EntityManager::update(sf::Time deltaTime)
{
    if (!mEntities.empty())
    {
        for (unsigned int i=mEntities.size()-1;i>0;i--)
        {
            if(!mEntities.at(i)->getActive())
            {
                mEntities.erase(mEntities.begin()+i);
            }
        }
    }


   // update movements
    if (!mEntities.empty())
    {
        for (unsigned int i=0; i<mEntities.size(); i++)
        {
            if(mEntities.at(i)->getActive())
            {


                // update movements and animation
                mEntities.at(i)->update(deltaTime);
            }
        }
    }

    // do collision check

    if (!mEntities.empty())
    {
        for (unsigned int i=0; i<mEntities.size(); i++)
        {
            if(mEntities.at(i)->getActive())
            {

                // test collisions
                // j=i+1 makes sure we never test an entity against itself,
                // and each entity against each other entity only once
                for (unsigned int j=i+1; j<mEntities.size(); j++)
                {
                    // make sure j is active too
                    if (mEntities.at(j)->getActive())
                    {
                        if (mEntities.at(i)->testCollision(mEntities.at(j)))
                        {
                            // make sure they are not the same type
                            if ((*mEntities.at(i)->getType())!=*mEntities.at(j)->getType())
                            {
                                // handle collision
                                handleCollisionBetween(mEntities.at(i),mEntities.at(j));

                            }

                        }
                    }
                }
            }
        }
    }

    // update camera

    if (!mEntities.empty())
    {
        for (unsigned int i=mEntities.size()-1;i>0;i--)
        {
            if(mEntities.at(i)->getActive())
            {
                std::string Pl="PLAYER";
                if(*mEntities.at(i)->getType()==Pl)
                {
                    int newcamOffsetX=0;
                    int newcamOffsetY=0;
                    int playerX = mEntities.at(i)->getPosition()->x;
                     int playerY = mEntities.at(i)->getPosition()->y;
                     int halfWidth = 0.5*TheGame::Instance()->getDisplayWidth();
                     int halfHeight = 0.5*TheGame::Instance()->getDisplayHeight();
                    newcamOffsetX=playerX-halfWidth;
                    newcamOffsetY=playerY-halfHeight;
                    int xMax=(TheMapManager::Instance()->getMapWidth()*32)-TheGame::Instance()->getDisplayWidth();
                    int yMax=(TheMapManager::Instance()->getMapHeight()*32)-TheGame::Instance()->getDisplayHeight();
                    if (newcamOffsetX<0) {newcamOffsetX=0;}
                    if (newcamOffsetY<0) {newcamOffsetY=0;}

                    if (newcamOffsetX>xMax) {newcamOffsetX=xMax;}
                    if (newcamOffsetY>yMax) {newcamOffsetY=yMax;}


                      TheGame::Instance()->mCameraOffset.x=newcamOffsetX;
                        TheGame::Instance()->mCameraOffset.y=newcamOffsetY;



                }

            }
        }
    }
}

void     EntityManager::render()
{

    if (!mEntities.empty())
    {
        for (int z=-64+TheGame::Instance()->mCameraOffset.y; z<TheGame::Instance()->getDisplayHeight()+TheGame::Instance()->mCameraOffset.y+128; z++)
        {


            for (unsigned int i=0; i<mEntities.size(); i++)
            {
                if (int(mEntities.at(i)->getPosition()->y)==z&&mEntities.at(i)->getActive())

                {
                    mEntities.at(i)->render();
                }

            }
        }

    }

}

void EntityManager::pushEntity(Entity * pEntity)
{
    mEntities.push_back(pEntity);
}
