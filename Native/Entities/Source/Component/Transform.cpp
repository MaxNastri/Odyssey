#include "Transform.h"
#include <Log.h>
#include <glm.h>

namespace Odyssey::Entities
{
	CLASS_DEFINITION(Odyssey.Entities, Transform);

	void Transform::Awake()
	{
	}

	void Transform::Update()
	{
		Framework::Log::Info("[Transform] Position: " + position.ToString());
	}

	void Transform::OnDestroy()
	{
		
	}

    void Transform::AddPosition(Vector3 pos)
    {
        position += pos;
        dirty = true;
    }

    void Transform::AddPosition(float x, float y, float z)
    {
        position.x += x;
        position.y += y;
        position.z += z;
        dirty = true;
    }

    void Transform::SetPosition(Vector3 pos)
    {
        position = pos;
        dirty = true;
    }

    void Transform::SetPosition(float x, float y, float z)
    {
        position.x = x;
        position.y = y;
        position.z = z;
        dirty = true;
    }

    void Transform::AddRotation(Vector3 eulerAngles)
    {
        eulerRotation += eulerAngles;
        eulerRotation.x = abs(eulerRotation.x) > 360.0f ? eulerRotation.x / 360.0f : eulerRotation.x;
        eulerRotation.y = abs(eulerRotation.y) > 360.0f ? eulerRotation.y / 360.0f : eulerRotation.y;
        eulerRotation.z = abs(eulerRotation.z) > 360.0f ? eulerRotation.z / 360.0f : eulerRotation.z;
        rotation = Quaternion::Euler(eulerRotation);
        dirty = true;
    }

    void Transform::AddRotation(Quaternion orientation)
    {
        rotation = orientation * rotation;
        dirty = true;
    }

    void Transform::SetRotation(Vector3 eulerAngles)
    {
        eulerRotation = eulerAngles;
        rotation = Quaternion::Euler(eulerRotation);
        dirty = true;
    }

    void Transform::SetRotation(Quaternion orientation)
    {
        rotation = orientation;
        eulerRotation = glm::degrees(glm::eulerAngles(rotation));
        dirty = true;
    }

    void Transform::AddScale(Vector3 scaleFactor)
    {
        scale += scaleFactor;
        dirty = true;
    }

    void Transform::AddScale(float x, float y, float z)
    {
        scale.x += x;
        scale.y += y;
        scale.z += z;
        dirty = true;
    }

    void Transform::SetScale(Vector3 scaleFactor)
    {
        scale = scaleFactor;
        dirty = true;
    }

    void Transform::SetScale(float x, float y, float z)
    {
        scale.x = x;
        scale.y = y;
        scale.z = z;
        dirty = true;
    }

    Vector3 Transform::Forward()
    {
        Vector3 row2 = Vector3(glm::column(localMatrix, 2));
        return glm::normalize(row2);
    }

    Vector3 Transform::Right()
    {
        Vector3 row2 = Vector3(glm::column(localMatrix, 0));
        return glm::normalize(row2);
    }

    Vector3 Transform::Up()
    {
        Vector3 row2 = Vector3(glm::column(localMatrix, 1));
        return glm::normalize(row2);
    }

    Matrix4x4 Transform::GetWorldMatrix()
    {
        UpdateWorldMatrix();
        return worldMatrix;
    }

    void Transform::UpdateWorldMatrix(bool forceUpdateWorld)
    {
        forceUpdateWorld |= dirty;

        // Check if we need to recalculate our world matrix
        if (dirty)
        {
            ComposeLocalMatrix();
            dirty = false;
        }

        // Check if we need to force an update to our world matrix
        //if (forceUpdateWorld && parent)
        //{
        //    worldMatrix = parent->worldMatrix * localMatrix;
        //}

        // Update our children
        //for (int i = 0; i < children.size(); ++i)
        //{
        //    children[i]->UpdateWorldMatrix(forceUpdateWorld);
        //}
    }

    void Transform::RotateAround(Vector3 center, Vector3 axis, float degrees, bool worldSpace)
    {
        Quaternion targetRotation = glm::angleAxis(glm::radians(degrees), axis);
        Vector3 directionToTarget = position - center;
        directionToTarget = targetRotation * directionToTarget;
        SetPosition(center + directionToTarget);

        if (worldSpace)
        {
            // Target * Current = world space rotation
            SetRotation(glm::normalize(targetRotation * rotation));
        }
        else
        {
            // Current * target = local space rotation
            SetRotation(glm::normalize(rotation * targetRotation));
        }
    }

    void Transform::CalculateEulerRotations()
    {
        // Check if the row 3 column 1 is +- 1
        if (localMatrix[2][0] != 1 && localMatrix[2][0] != -1)
        {
            // Calculate the euler angle rotation from the world matrix
            float x = -std::asin(localMatrix[2][0]);
            float cosX = std::cos(x);
            float y = std::atan2(localMatrix[2][1] / cosX, localMatrix[2][2] / cosX);
            float z = std::atan2(localMatrix[1][0] / cosX, localMatrix[0][0] / cosX);

            // Set the calculated rotation value
            eulerRotation = Vector3(x, y, z);
        }
        else
        {
            // Calculate the euler angle rotation from the world matrix
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            if (localMatrix[2][0] == -1)
            {
                x = glm::half_pi<float>();
                y = x + std::atan2(localMatrix[0][1], localMatrix[0][2]);
            }
            else
            {
                x = -glm::half_pi<float>();
                y = -x + std::atan2(-localMatrix[0][1], -localMatrix[0][2]);
            }

            // Set the calculated rotation value
            eulerRotation = Vector3(x, y, z);
        }
    }

    void Transform::ComposeLocalMatrix()
    {
        // Compose the position, rotation and scale into 3 matrices
        mat4x4 t = glm::translate(glm::identity<mat4x4>(), position);
        mat4x4 r = glm::toMat4(Quaternion(rotation.x, rotation.y, rotation.z, rotation.w));
        mat4x4 s = glm::scale(glm::identity<mat4x4>(), scale);
        localMatrix = t * r * s;
        worldMatrix = localMatrix;
    }

    void Transform::Reset()
    {
        position = Vector3(0, 0, 0);
        eulerRotation = Vector3(0, 0, 0);
        rotation = Quaternion(0, 0, 0, 1);
        scale = Vector3(1, 1, 1);

        //parent = nullptr;
        localMatrix = Matrix4x4::Identity();
        worldMatrix = Matrix4x4::Identity();
        ComposeLocalMatrix();

        dirty = true;
    }

	void Transform::Serialize(json& jsonObject)
	{
		json componentJson;
		to_json(componentJson, *this);
		jsonObject += { "Component." + Transform::ClassName, componentJson};
	}

	void Transform::Deserialize(const json& jsonObject)
	{
		from_json(jsonObject, *this);
	}
}