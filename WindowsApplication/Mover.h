#pragma once
#include "GameScript.h"

namespace FlatEngine
{
    class Mover : public FlatEngine::GameScript
    {
    public:
        Mover();
        ~Mover();

        void Awake();
        void Start();
        void Update(float deltaTime);
    };
}
