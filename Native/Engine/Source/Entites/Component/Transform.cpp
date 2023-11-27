#include "Transform.h"
#include <Logger.h>
#include <glm.h>
#include <Yaml.h>

namespace Odyssey
{
	CLASS_DEFINITION(Odyssey.Entities, Transform);

    Transform::Transform()
    {
        position = glm::vec3(0, 0, 0);
        rotation = glm::quat(0, 0, 0, 1);
        scale = glm::vec3(1, 1, 1);
    }

    void Transform::Awake()
	{
	}

	void Transform::Update()
	{
		
	}

	void Transform::OnDestroy()
	{
		
	}

    void Transform::AddPosition(glm::vec3 pos)
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

    void Transform::SetPosition(glm::vec3 pos)
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

    void Transform::AddRotation(glm::vec3 eulerAngles)
    {
        eulerRotation += eulerAngles;
        eulerRotation.x = abs(eulerRotation.x) > 360.0f ? eulerRotation.x / 360.0f : eulerRotation.x;
        eulerRotation.y = abs(eulerRotation.y) > 360.0f ? eulerRotation.y / 360.0f : eulerRotation.y;
        eulerRotation.z = abs(eulerRotation.z) > 360.0f ? eulerRotation.z / 360.0f : eulerRotation.z;
        rotation = glm::quat(glm::vec3(glm::radians(eulerRotation.x), glm::radians(eulerRotation.y), glm::radians(eulerRotation.z)));
        dirty = true;
    }

    void Transform::AddRotation(glm::quat orientation)
    {
        rotation = orientation * rotation;
        dirty = true;
    }

    void Transform::SetRotation(glm::vec3 eulerAngles)
    {
        eulerRotation = eulerAngles;
        rotation = glm::quat(glm::vec3(glm::radians(eulerAngles.x), glm::radians(eulerAngles.y), glm::radians(eulerAngles.z)));
        dirty = true;
    }

    void Transform::SetRotation(glm::quat orientation)
    {
        rotation = orientation;
        eulerRotation = glm::degrees(glm::eulerAngles(rotation));
        dirty = true;
    }

    void Transform::AddScale(glm::vec3 scaleFactor)
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

    void Transform::SetScale(glm::vec3 scaleFactor)
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

    glm::vec3 Transform::Forward()
    {
        glm::vec3 row2 = glm::column(localMatrix, 2);
        return glm::normalize(row2);
    }

    glm::vec3 Transform::Right()
    {
        glm::vec3 row2 = glm::column(localMatrix, 0);
        return glm::normalize(row2);
    }

    glm::vec3 Transform::Up()
    {
        glm::vec3 row2 = glm::column(localMatrix, 1);
        return glm::normalize(row2);
    }

    glm::mat4x4 Transform::GetWorldMatrix()
    {
        //UpdateWorldMatrix();
        ComposeLocalMatrix();
        return localMatrix;
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

    void Transform::RotateAround(glm::vec3 center, glm::vec3 axis, float degrees, bool worldSpace)
    {
        glm::quat targetRotation = glm::angleAxis(glm::radians(degrees), axis);
        glm::vec3 directionToTarget = position - center;
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
            eulerRotation = glm::vec3(x, y, z);
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
            eulerRotation = glm::vec3(x, y, z);
        }
    }

    void Transform::ComposeLocalMatrix()
    {
        // Compose the position, rotation and scale into 3 matrices
        mat4x4 t = glm::translate(glm::identity<mat4x4>(), position);
        mat4x4 r = glm::toMat4(rotation);
        mat4x4 s = glm::scale(glm::identity<mat4x4>(), scale);
        localMatrix = t * r * s;
        worldMatrix = localMatrix;
    }

    void Transform::Reset()
    {
        position = glm::vec3(0, 0, 0);
        eulerRotation = glm::vec3(0, 0, 0);
        rotation = glm::quat(0, 0, 0, 1);
        scale = glm::vec3(1, 1, 1);

        //parent = nullptr;
        localMatrix = glm::identity<glm::mat4>();
        worldMatrix = glm::identity<glm::mat4>();
        ComposeLocalMatrix();

        dirty = true;
    }

    void Transform::Serialize(ryml::NodeRef& node)
    {
        ryml::NodeRef componentNode = node.append_child();
        componentNode |= ryml::MAP;

        componentNode["Name"] << Transform::Type;
        componentNode["Position"] << position;
        componentNode["Rotation"] << eulerRotation;
        componentNode["Scale"] << scale;
    }

    void Transform::Deserialize(ryml::ConstNodeRef node)
    {
        node["Position"] >> position;
        node["Rotation"] >> eulerRotation;
        node["Scale"] >> scale;
    }
}