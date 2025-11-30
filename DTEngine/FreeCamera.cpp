#include "pch.h"
#include "FreeCamera.h"
#include "Transform.h"
#include "GameObject.h"
#include "InputManager.h" 
#include "HistoryManager.h"
//#include "ResourceManager.h"

BEGINPROPERTY(FreeCamera)
DTPROPERTY(FreeCamera, m_moveSpeed)
DTPROPERTY(FreeCamera, m_turboScale)
DTPROPERTY(FreeCamera, m_rotationSpeed)
DTPROPERTY(FreeCamera, m_yaw)
DTPROPERTY(FreeCamera, m_pitch);
ENDPROPERTY()

FreeCamera::FreeCamera() = default;
FreeCamera::~FreeCamera() = default;

void FreeCamera::Awake()
{
    Transform* transform = GetComponent<Transform>();
    if (transform)
    {
        Vector3 euler = transform->GetEditorEuler();
        m_pitch = euler.x;
        m_yaw = euler.y;
    }

	//ResourceManager::Instance().LoadModel("Assets/Models/Dwarf.x");
}

void FreeCamera::Update(float deltaTime)
{
    HandleInput(deltaTime);
}

void FreeCamera::HandleInput(float deltaTime)
{
    Transform* transform = GetComponent<Transform>();
    if (!transform) return;

    InputManager& input = InputManager::Instance();

    if (input.GetKeyDown(KeyCode::MouseRight))
    {
        if (s_isSceneHovered)
        {
            m_isControlling = true;
        }
    }

    if (input.GetKeyUp(KeyCode::MouseRight))
    {
        m_isControlling = false;
    }

    if (m_isControlling && input.GetKey(KeyCode::MouseRight))
    {
        auto delta = input.GetMouseDelta();

        m_yaw -= delta.x * m_rotationSpeed;
        m_pitch -= delta.y * m_rotationSpeed;

        transform->SetRotationEuler(Vector3(m_pitch, m_yaw, 0.0f));


        Vector3 moveDir = Vector3(0, 0, 0); 

        if (input.GetKey(KeyCode::W)) moveDir.z -= 1.0f;
        if (input.GetKey(KeyCode::S)) moveDir.z += 1.0f;
        if (input.GetKey(KeyCode::D)) moveDir.x += 1.0f;
        if (input.GetKey(KeyCode::A)) moveDir.x -= 1.0f;
        if (input.GetKey(KeyCode::E)) moveDir.y += 1.0f;
        if (input.GetKey(KeyCode::Q)) moveDir.y -= 1.0f;

        if (moveDir != Vector3(0, 0, 0))
        {
            moveDir.Normalize();
            float currentSpeed = m_moveSpeed;
            if (input.GetKey(KeyCode::Shift)) currentSpeed *= m_turboScale;

            Vector3 forward = transform->Forward();
            Vector3 right = transform->Right();

            Vector3 moveVector = (forward * moveDir.z) + (right * moveDir.x) + (Vector3(0, 1, 0) * moveDir.y);

            Vector3 newPos = transform->GetPosition() + (moveVector * currentSpeed * deltaTime);
            transform->SetPosition(newPos);
        }


        HistoryManager::Instance().SetDirty();
    }

    
}