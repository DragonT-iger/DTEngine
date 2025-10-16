#include "pch.h"
#include <fstream>
#include "SceneSerializer.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"

bool SceneSerializer::SaveToJson(const std::string& path)
{
	Scene* curScene = SceneManager::Instance().GetActiveScene();
    if (curScene == nullptr) return false;

    const std::vector<std::unique_ptr<GameObject>>& objs = curScene->GetGameObjects();
    if (objs.empty())
    {
        // 비어있어도 빈 배열로 저장
    }
    // 2) Transform* -> id 맵 (parentId 기록용)
    std::unordered_map<const Transform*, int> toId;
    toId.reserve(objs.size());
    for (int i = 0; i < (int)objs.size(); ++i)
    {
        if (auto* tf = objs[i]->GetComponent<Transform>())
            toId[tf] = i;
    }

    JsonWriter w;

    w.BeginObject();            
    w.BeginArray("objects");

    for (int i = 0; i < (int)objs.size(); ++i)
    {
        GameObject* go = objs[i].get();
        w.BeginObject();         

        // 메타
        w.Write("id", i);
        w.Write("name", go->GetName().c_str());
        w.Write("active", go->IsActive());

        // parentId (루트는 -1)
        int parentId = -1;
        if (auto* tf = go->GetComponent<Transform>())
        {
            if (auto* p = tf->GetParent())
            {
                auto it = toId.find(p);
                if (it != toId.end()) parentId = it->second;
            }
        }
        w.Write("parentId", parentId);

        // 컴포넌트들 직렬화 (각 컴포넌트의 Serialize만 호출)
        w.BeginArray("components");
        {
			const auto& components = go->_GetComponents();
            for (auto& comp : components)
            {
                w.BeginObject();
                w.Write("type", typeid(*comp).name());
                comp->Serialize(w); 
                w.EndObject();      
			}
        }
        w.EndArray();

        w.EndObject();         
    }

    w.EndArray();              
    w.EndObject();             


    std::ofstream ofs(path, std::ios::binary);
    if (!ofs) return false;
    ofs << w.ToString();
    return true;
}