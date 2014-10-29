#include "EntityManager.h"

EntityManager* EntityManager::s_pInstance = 0;

void    EntityManager::update(sf::Time deltaTime)
    {
        if (!mEntities.empty())
        {
         for (unsigned int i=0; i<mEntities.size(); i++)
         {
         mEntities.at(i)->update(deltaTime);
         }

        }
    }

void     EntityManager::render()
    {

    if (!mEntities.empty())
        {
         for (int z=-32;z<513;z++)
         {


             for (unsigned int i=0; i<mEntities.size(); i++)
             {
                if (mEntities.at(i)->getZ()==z)
                mEntities.at(i)->render();
             }
        }

        }

    }

void EntityManager::pushEntity(Entity * pEntity)
{
    mEntities.push_back(pEntity);
}
