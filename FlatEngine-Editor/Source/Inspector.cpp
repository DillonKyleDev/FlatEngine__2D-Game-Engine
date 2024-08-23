#include "FlatEngine.h"
#include "FlatGui.h"
#include "Project.h"
#include "TagList.h"
#include "Scene.h"
#include "Sprite.h"
#include "Button.h"
#include "Camera.h"
#include "Canvas.h"
#include "Script.h"
#include "Text.h"
#include "Audio.h"
#include "CharacterController.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
#include "Component.h"

namespace FL = FlatEngine;

using Component = FL::Component;
using ComponentTypes = FL::Component::ComponentTypes;
using Sprite = FL::Sprite;
using CharacterController = FL::CharacterController;
using BoxCollider = FL::BoxCollider;
using CircleCollider = FL::CircleCollider;
using RigidBody = FL::RigidBody;
using Button = FL::Button;


namespace FlatGui 
{
	void RenderInspector()
	{
		FL::BeginWindow("Inspector");

		long focusedObjectID = GetFocusedGameObjectID();

		if (focusedObjectID != -1 && FL::GetObjectById(focusedObjectID) != nullptr)
		{
			// Get focused GameObject
			GameObject* focusedObject = FL::GetObjectById(focusedObjectID);

			// Lambda
			auto L_ShowAddComponentsWindow = [&]()
			{
				FL::PushMenuStyles();

				// Add all the component types you can add to this GameObject
				//
				if (!focusedObject->HasComponent("Transform"))
				{
					if (ImGui::MenuItem("Transform"))
					{
						focusedObject->AddTransformComponent();
						ImGui::CloseCurrentPopup();
					}
				}

				if (!focusedObject->HasComponent("Sprite"))
				{
					if (ImGui::MenuItem("Sprite"))
					{
						focusedObject->AddSpriteComponent();
						ImGui::CloseCurrentPopup();
					}
				}

				if (ImGui::MenuItem("Button"))
				{
					focusedObject->AddButtonComponent();
					ImGui::CloseCurrentPopup();
				}

				if (!focusedObject->HasComponent("Camera"))
				{
					if (ImGui::MenuItem("Camera"))
					{
						focusedObject->AddCameraComponent();
						ImGui::CloseCurrentPopup();
					}
				}

				if (!focusedObject->HasComponent("Canvas"))
				{
					if (ImGui::MenuItem("Canvas"))
					{
						focusedObject->AddCanvasComponent();
						ImGui::CloseCurrentPopup();
					}
				}

				if (!focusedObject->HasComponent("Animation"))
				{
					if (ImGui::MenuItem("Animation"))
					{
						focusedObject->AddAnimationComponent();
						ImGui::CloseCurrentPopup();
					}
				}

				if (!focusedObject->HasComponent("Audio"))
				{
					if (ImGui::MenuItem("Audio"))
					{
						focusedObject->AddAudioComponent();
						ImGui::CloseCurrentPopup();
					}
				}

				if (!focusedObject->HasComponent("Text"))
				{
					if (ImGui::MenuItem("Text"))
					{
						focusedObject->AddTextComponent();
						ImGui::CloseCurrentPopup();
					}
				}

				if (ImGui::MenuItem("Script"))
				{
					focusedObject->AddScriptComponent();
					ImGui::CloseCurrentPopup();
				}

				if (!focusedObject->HasComponent("CharacterController"))
				{
					if (ImGui::MenuItem("CharacterController"))
					{
						focusedObject->AddCharacterControllerComponent();
						ImGui::CloseCurrentPopup();
					}
				}

				if (!focusedObject->HasComponent("RigidBody"))
				{
					if (ImGui::MenuItem("RigidBody"))
					{
						focusedObject->AddRigidBodyComponent();
						ImGui::CloseCurrentPopup();
					}
				}

				if (ImGui::MenuItem("BoxCollider"))
				{
					focusedObject->AddBoxColliderComponent();
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("CircleCollider"))
				{
					focusedObject->AddCircleColliderComponent();
					ImGui::CloseCurrentPopup();
				}

				FL::PopMenuStyles();
			};

			// Name editing
			std::string nameLabel = "Name: ";
			char newName[1024];
			strcpy_s(newName, focusedObject->GetName().c_str());
			

			// Edit field
			ImGui::Text(nameLabel.c_str());
			ImGui::SameLine(0, 5);

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("input"));
			if (ImGui::InputText("##GameObject Name", newName, IM_ARRAYSIZE(newName), FL::F_inputFlags))
				focusedObject->SetName(newName);
			ImGui::PopStyleColor();

			bool _objectActive = focusedObject->IsActive();

			ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 2));

			// GameObject Active Checkbox
			if (FL::RenderCheckbox("Active", _objectActive))
				focusedObject->SetActive(_objectActive);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 62, 5);

			// GameObject TagList Dropdown
			TagList tagList = focusedObject->GetTagList();			
			FL::RenderButton("Tags");
			FL::PushMenuStyles();
			if (ImGui::BeginPopupContextItem("TagsPopup", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
			{
				std::string labels[2] = { "Tag", "Ignore" };
				if (FL::PushTable("TagsTable", 3, FL::F_resizeableTableFlags))
				{
					FL::RenderTextTableRow("TagsTableHeaders", "Tag", "Has", "Ignore");

					for (std::pair<std::string, bool> tag : tagList.GetTagsMap())
					{
						std::string tableRowId = tag.first + "TagCheckboxTableRow";
						RenderTagListTableRow(tableRowId.c_str(), tag.first, tagList);
					}
				
					FL::PopTable();
				}
				ImGui::EndPopup();
			}
			FL::PopMenuStyles();


			// Three Dots More Options Button
			ImGui::SameLine(0, 5);
			FL::RenderImageButton("##InspectorMoreButton", FL::GetTexture("threeDots"), Vector2(16, 16), 1, FL::GetColor("transparent"));
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
			// Render the actual menu on click
			FL::PushMenuStyles();
			if (ImGui::BeginPopupContextItem("##InspectorMoreContext", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
			{
				if (ImGui::BeginMenu("Add Component"))
				{
					L_ShowAddComponentsWindow();
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Delete GameObject"))
				{
					SetFocusedGameObjectID(-1);
					FL::DeleteGameObject(focusedObject->GetID());
					ImGui::CloseCurrentPopup();
				}
	
				ImGui::EndPopup();
			}
			FL::PopMenuStyles();
		

			// Components Section
			FL::RenderSectionHeader("Components");


			// For scrolling components section with background
			ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("logBg"));
			ImGui::BeginChild("ComponentsSectionBg", Vector2(0,ImGui::GetContentRegionAvail().y - 30), FL::F_childFlags);
			ImGui::PopStyleColor();

			// Border around each component
			auto wPos = ImGui::GetWindowPos();
			auto wSize = ImGui::GetWindowSize();
		

			if (focusedObject != nullptr)
			{
				long queuedForDelete = -1;

				// Transform
				FL::Transform* transform = focusedObject->GetTransform();
				if (transform != nullptr)
				{
					BeginComponent(transform, queuedForDelete);
					if (!transform->IsCollapsed())
						RenderTransformComponent(transform);
					EndComponent(transform);
				}

				// Sprite
				Sprite* sprite = FL::GetLoadedScene()->GetSpriteByOwner(focusedObjectID);
				if (sprite != nullptr)
				{
					BeginComponent(sprite, queuedForDelete);
					if (!sprite->IsCollapsed())
						RenderSpriteComponent(sprite);
					EndComponent(sprite);
				}

				// Camera
				Camera* camera = focusedObject->GetCamera();
				if (camera != nullptr)
				{
					BeginComponent(camera, queuedForDelete);
					if (!camera->IsCollapsed())
						RenderCameraComponent(camera);
					EndComponent(camera);
				}

				// Script
				std::vector<Script*> scripts = focusedObject->GetScripts();
				for (Script* script : scripts)
				{
					BeginComponent(script, queuedForDelete);
					if (!script->IsCollapsed())
						RenderScriptComponent(script);
					EndComponent(script);
				}

				// Button
				Button* button = focusedObject->GetButton();
				if (button != nullptr)
				{
					BeginComponent(button, queuedForDelete);
					if (!button->IsCollapsed())
						RenderButtonComponent(button);
					EndComponent(button);
				}

				// Canvas
				Canvas* canvas = focusedObject->GetCanvas();
				if (canvas != nullptr)
				{
					BeginComponent(canvas, queuedForDelete);
					if (!canvas->IsCollapsed())
						RenderCanvasComponent(canvas);
					EndComponent(canvas);
				}

				// Animation
				Animation* animation = focusedObject->GetAnimation();
				if (animation != nullptr)
				{
					BeginComponent(animation, queuedForDelete);
					if (!animation->IsCollapsed())
						RenderAnimationComponent(animation);
					EndComponent(animation);
				}

				// Audio
				Audio* audio = focusedObject->GetAudio();
				if (audio != nullptr)
				{
					BeginComponent(audio, queuedForDelete);
					if (!audio->IsCollapsed())
						RenderAudioComponent(audio);
					EndComponent(audio);
				}

				// Text
				Text* text = focusedObject->GetText();
				if (text != nullptr)
				{
					BeginComponent(text, queuedForDelete);
					if (!text->IsCollapsed())
						RenderTextComponent(text);
					EndComponent(text);
				}

				// CharacterController
				CharacterController* characterController = focusedObject->GetCharacterController();
				if (characterController != nullptr)
				{
					BeginComponent(characterController, queuedForDelete);
					if (!characterController->IsCollapsed())
						RenderCharacterControllerComponent(characterController);
					EndComponent(characterController);
				}

				// BoxColliders
				std::vector<BoxCollider*> boxColliders = focusedObject->GetBoxColliders();
				for (BoxCollider* boxCollider : boxColliders)
				{
					BeginComponent(boxCollider, queuedForDelete);
					if (!boxCollider->IsCollapsed())
						RenderBoxColliderComponent(boxCollider);
					EndComponent(boxCollider);
				}

				// CircleColliders
				std::vector<CircleCollider*> circleColliders = focusedObject->GetCircleColliders();
				for (CircleCollider* circleCollider : circleColliders)
				{
					BeginComponent(circleCollider, queuedForDelete);
					if (!circleCollider->IsCollapsed())
						RenderCircleColliderComponent(circleCollider);
					EndComponent(circleCollider);
				}

				// RigidBody
				RigidBody* rigidBody = focusedObject->GetRigidBody();
				if (rigidBody != nullptr)
				{
					BeginComponent(rigidBody, queuedForDelete);
					if (!rigidBody->IsCollapsed())
						RenderRigidBodyComponent(rigidBody);
					EndComponent(rigidBody);
				}

				if (queuedForDelete != -1)
				{
					focusedObject->RemoveComponent(queuedForDelete);
					queuedForDelete = -1;
				}
			}

			ImGui::EndChild(); // ComponentsSectionBg

			// Border around Components Section
			ImGui::GetWindowDrawList()->AddRect({ wPos.x, wPos.y - 1 }, { wPos.x + wSize.x, wPos.y + wSize.y + 1}, FL::GetColor32("componentSectionBorder"), 1);

			// Render the Adding Components button
			FL::RenderButton("Add Component", Vector2(ImGui::GetContentRegionAvail().x, 0));
			if (ImGui::BeginPopupContextItem("##AddComponent", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
			{
				L_ShowAddComponentsWindow();
				ImGui::EndPopup();
			}
		}

		FL::EndWindow();
	}
}