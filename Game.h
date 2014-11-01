#ifndef __Game__
#define __Game__

#include <SFML/Graphics.hpp>

#include "Entity.h"

class Game{
public:

    static Game * Instance() {
    if (s_pInstance==0)
        {
            s_pInstance=new Game();
            return s_pInstance;
        }
        return s_pInstance;
    }
    void run();
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void updateStatistics(sf::Time elapsedTime);

    sf::RenderWindow * getWindow(){return &mWindow;}
    sf::RenderTexture * getRenderTexture(){return &mRenderTexture;}

       void applyShader(sf::RenderStates);

       void drawMarker(int x, int y);

private:
    Game();
    static const float		PlayerSpeed;
    static const sf::Time	TimePerFrame;

    sf::RenderWindow mWindow;
    sf::RenderTexture mRenderTexture;
    sf::Sprite mRenderSprite;
    sf::Sprite mMarkerSprite;

    sf::Texture mTexture;
     sf::Texture mTileset;
     sf::Texture mMarker;


    static Game* s_pInstance;

        sf::Font				mFont;
		sf::Text				mStatisticsText;
		sf::Time				mStatisticsUpdateTime;

		std::size_t				mStatisticsNumFrames;
};

typedef Game TheGame;

#endif // __Game__
