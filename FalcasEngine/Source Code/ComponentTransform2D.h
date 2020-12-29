#pragma once
#include "Component.h"
#include "External Libraries/MathGeoLib/include/Math/float2.h"
#include "External Libraries/MathGeoLib/include/Math/float4x4.h"
#include "External Libraries/MathGeoLib/include/Math/Quat.h"
#include "Json.h"

class GameObject;


class ComponentTransform2D : public Component {
public:
	ComponentTransform2D(GameObject* owner, float2 position, Quat rotation, float2 size);
	~ComponentTransform2D();
	void Update();

	float2 GetPosition()const;
	float3 GetRotation()const;
	Quat GetRotationQuaternion()const;
	float2 GetSize()const;
	float4x4 GetGlobalMatrix() const;
	float4x4 GetGlobalMatrixTransposed() const;

	bool SaveComponent(JsonObj& obj);
	float3 QuaternionToEuler(Quat q);
	Quat EulerToQuaternion(float3 eu);

	Quat LookAt(const float3& point);
	
	void SetTransformation(float2 pos, Quat rot, float2 size);
	void SetPosition(float2 pos);
	void SetRotation(Quat rot);
	void SetRotation(float3 rot);
	void SetSize(float2 size);
	void SetMatrices();
	void SetMatricesWithNewParent(float4x4 parent_global_matrix);
	void Inspector();



public:
	bool needed_to_update = false;
	bool needed_to_update_only_children = false;


private:
	float4x4 local_matrix;
	float4x4 global_matrix;
	float4x4 global_matrix2;
	float4x4 global_matrix_transposed;
	float2 position;
	float z_depth;
	float2 pivot_position;
	float2 size;
	float3 rotation;
	Quat quat_rotation;
};