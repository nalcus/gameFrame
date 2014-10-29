#ifndef __MapManager__
#define __MapManager__

#include <SFML/Graphics.hpp>
#include <vector>

class MapManager
{
 public:
    static MapManager* Instance()
    {
        if(s_pInstance==0)
        {
            s_pInstance=new MapManager;
            return s_pInstance;
        }
        return s_pInstance;
    }

    void setTileset(sf::Texture * pTexture);
    void update(sf::Time deltaTime);
    void render();



 private:
     MapManager();
        static MapManager* s_pInstance;
            sf::Sprite mSprite;

        std::vector<int> mMapData;

        int mMapWidth;
        int mMapHeight;
};

typedef MapManager TheMapManager;


#endif // __MapManager__
