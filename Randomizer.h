#ifndef __Randomizer__
#define __Randomizer__

class Randomizer
{
public:
    static Randomizer *Instance()
    {
        if (s_pInstance==0)
        {
            s_pInstance=new Randomizer();
            return s_pInstance;
        }
        return s_pInstance;
    }

    Randomizer();
    void reseed();
    int getRandomInt();
private:

    static Randomizer* s_pInstance;
};

typedef Randomizer TheRandomizer;

#endif // __Randomizer__
