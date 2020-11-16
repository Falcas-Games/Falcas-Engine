#include "ComponentCamera.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "External Libraries/ImGui/imgui.h"
#include "ModuleRenderer3D.h"

ComponentCamera::ComponentCamera(GameObject* owner, ComponentTransform* trans) :Component(Component_Type::Camera, owner, "Camera"), trans(trans)
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetPos(trans->GetPosition());
	frustum.SetFront({ 0,0,1 });
	frustum.SetUp({ 0,1,0 });

	UpdateFrustum();

}

ComponentCamera::~ComponentCamera()
{
}

void ComponentCamera::UpdateFrustum()
{
	frustum.SetViewPlaneDistances(near_plane_distance, far_plane_distance);
	field_of_view_horizontal = 2 * Atan(Tan(field_of_view_vertical / 2)*(width/height));
	frustum.SetPerspective(field_of_view_vertical, field_of_view_horizontal);
	update_projection_matrix = true;
}
void ComponentCamera::Update()
{
	frustum.SetPos(trans->GetPosition());
	frustum.SetFront(float3x3::FromQuat(trans->GetRotation())* float3::unitZ);
	frustum.SetUp(float3x3::FromQuat(trans->GetRotation())*float3::unitY);
	if (!App->scene_intro->GetDimensionsWindow(width, height) &&!needed_to_update)
		return;
	UpdateFrustum();
	needed_to_update = false;
}

bool ComponentCamera::GetIfIsFrustumCulling() const
{
	if (active)
		return frustum_culling;
	else
		return false;
}

float* ComponentCamera::GetProjectionMatrix() const
{
	static float4x4 ProjectionMatrix;
	ProjectionMatrix= frustum.ProjectionMatrix().Transposed();
	return (float*)ProjectionMatrix.v;
}

float* ComponentCamera::GetViewMatrix() const
{
	static float4x4 ViewMatrix;
	ViewMatrix = frustum.ViewMatrix();
	ViewMatrix.Transpose();
	return (float*)ViewMatrix.v;
	
}


void ComponentCamera::Inspector()
{
	ImGui::PushID(name.c_str());
	Component::Inspector();

	ImGui::Separator();

	if (ImGui::Checkbox("View Camera", &camera_active)) {
		if(camera_active==true)
			App->renderer3D->ChangeCameraActive(this);
		else
			App->renderer3D->ChangeCameraActive(nullptr);
	}

	if (ImGui::Checkbox("Camera Culling", &frustum_culling)) {
		if (frustum_culling == true)
			App->renderer3D->ChangeCullingCamera(this);
		else
			App->renderer3D->ChangeCullingCamera(nullptr);
	}

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Near Plane Distance");

	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	if (ImGui::DragFloat("##0", &near_plane_distance, 0.01f))
		needed_to_update = true;
	ImGui::PopItemWidth();

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Far Plane Distance");

	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	if (ImGui::DragFloat("##1", &far_plane_distance, 0.01f))
		needed_to_update = true;
	ImGui::PopItemWidth();

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Field of View Vertical");

	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	if (ImGui::DragFloat("##2", &field_of_view_vertical, 0.01f))
		needed_to_update = true;
	ImGui::PopItemWidth();

	ImGui::Separator();
	ImGui::PopID();
}
