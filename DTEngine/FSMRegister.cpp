#include "pch.h"
#include <string>
#include "FSMRegister.h"
#include "AnimationClip.h"
#include "Skeletal.h"
#include "Animatior.h"
#include "ResourceManager.h"
#include "AssetDatabase.h"


//여기서 Register를 임의로 만듦.

#include "SoundManager.h"

bool FSMRegister::Initalize()
{

    std::string RootPath = ResourceManager::Instance().GetResourceRootPath(); //Assets

    

    State idleState;
    idleState.onEnter = [=](GameObject& Owner) 
        
         {  /* SoundManager::Instance().PlayBGM
        
             (RootPath + "/Sound/The_World Is_Yours.mp3", 1.0f, false); */

         };

    idleState.onUpdate = [=](GameObject& Owner, float dt)
        {
           

        };

    RegisterState("P_Idle", idleState);

    State moveState;
    moveState.onEnter = [](GameObject& Owner) { };
    moveState.onUpdate = [](GameObject& Owner, float dt)
        {
      
        };

    RegisterState("P_Move", moveState);

    State attackState;
    attackState.onEnter = [](GameObject& Owner) { };
    attackState.onUpdate = [](GameObject& Owner, float dt) {
        };
    attackState.onExit = [](GameObject& Owner) {};
    RegisterState("P_Attack", attackState);

    State dieState;
    dieState.onEnter = [](GameObject& Owner) {
      
        };
    dieState.onUpdate = [](GameObject& Owner, float dt) {  };
    RegisterState("P_Die", dieState);


    State danceState;
    danceState.onEnter = [=](GameObject& Owner)
        
        {
            uint64_t id = AssetDatabase::Instance().GetIDFromPath(RootPath + "/Models/Sandre/Dancing_0.01.fbx");
            auto Comp = Owner.GetComponent<Animator>();
            if (Comp) Comp->SetClip(id);
          
           // SoundManager::Instance().PlayBGM(RootPath + "/Sound/Doomsday.mp3", 1, true);


        };


    danceState.onUpdate = [](GameObject& Owner, float dt)
        {
        
            auto Comp = Owner.GetComponent<Animator>();
        if (Comp)    Comp->SetPlay(true);
       
        };


    //해당 animation을 지운다?
    danceState.onExit = [](GameObject& Owner)
        {
            auto Comp = Owner.GetComponent<Animator>();
            if (Comp)    Comp->SetPlay(false);
        };

    RegisterState("P_Dance", danceState);



    return true;
}

State FSMRegister::GetState(std::string key)
{

    if (key == "") return State(); //비용좀 아낄래

    auto iter = m_FunctionMap.find(key);
    if (iter != m_FunctionMap.end())
    {
        return iter->second;
    }


    return State();
}

FSMRegister::~FSMRegister()
{
    m_FunctionMap.clear();
}


