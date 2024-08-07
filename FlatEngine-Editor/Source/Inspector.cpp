#include "FlatEngine.h"
#include "FlatGui.h"
#include "Project.h"
#include "TagList.h"
#include "Scene.h"
#include "Sprite.h"
#include "Button.h"
#include "Camera.h"
#include "Canvas.h"
#include "ScriptComponent.h"
#include "Text.h"
#include "Audio.h"
#include "CharacterController.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
#include "Component.h"


using Component = FlatEngine::Component;
using ComponentTypes = FlatEngine::Component::ComponentTypes;
using Sprite = FlatEngine::Sprite;
using CharacterController = FlatEngine::CharacterController;
using BoxCollider = FlatEngine::BoxCollider;
using CircleCollider = FlatEngine::CircleCollider;
using RigidBody = FlatEngine::RigidBody;
using Button = FlatEngine::Button;


namespace FlatGui 
{
	void RenderInspector()
	{
		FlatEngine::BeginWindow("Inspector");

		long focusedObjectID = GetFocusedGameObjectID();

		if (focusedObjectID != -1 && FlatEngine::GetObjectById(focusedObjectID) != nullptr)
		{
			// Get focused GameObject
			GameObject* focusedObject = FlatEngine::GetObjectById(focusedObjectID);

			// Lambda
			auto L_ShowAddComponentsWindow = [&]()
			{
				FlatEngine::PushMenuStyles();

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

				FlatEngine::PopMenuStyles();
			};

			// Name editing
			std::string nameLabel = "Name: ";
			char newName[1024];
			strcpy_s(newName, focusedObject->GetName().c_str());
			

			// Edit field
			ImGui::Text(nameLabel.c_str());
			ImGui::SameLine(0, 5);

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, FlatEngine::F_inputColor);
			if (ImGui::InputText("##GameObject Name", newName, IM_ARRAYSIZE(newName), FlatEngine::F_inputFlags))
				focusedObject->SetName(newName);
			ImGui::PopStyleColor();

			bool _objectActive = focusedObject->IsActive();

			ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 2));

			// GameObject Active Checkbox
			if (FlatEngine::RenderCheckbox("Active", _objectActive))
				focusedObject->SetActive(_objectActive);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 62, 5);

			// GameObject TagList Dropdown
			TagList tagList = focusedObject->GetTagList();			
			FlatEngine::RenderButton("Tags");
			FlatEngine::PushMenuStyles();
			if (ImGui::BeginPopupContextItem("TagsPopup", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
			{
				std::string labels[2] = { "Tag", "Ignore" };
				if (FlatEngine::PushTable("TagsTable", 3, FlatEngine::F_resizeableTableFlags))
				{
					FlatEngine::RenderTextTableRow("TagsTableHeaders", "Tag", "Has", "Ignore");

					for (std::pair<std::string, bool> tag : tagList.GetTagsMap())
					{
						std::string tableRowId = tag.first + "TagCheckboxTableRow";
						RenderTagListTableRow(tableRowId.c_str(), tag.first, tagList);
					}
				
					FlatEngine::PopTable();
				}
				ImGui::EndPopup();
			}
			FlatEngine::PopMenuStyles();


			// Three Dots More Options Button
			ImGui::SameLine(0, 5);
			FlatEngine::RenderImageButton("##InspectorMoreButton", FlatEngine::F_threeDotsIcon.GetTexture(), Vector2(16, 16), 1, FlatEngine::F_transparentColor);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
			// Render the actual menu on click
			FlatEngine::PushMenuStyles();
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
					FlatEngine::DeleteGameObject(focusedObject->GetID());
					ImGui::CloseCurrentPopup();
				}
	
				ImGui::EndPopup();
			}
			FlatEngine::PopMenuStyles();
		

			// Components Section
			FlatEngine::RenderSectionHeader("Components");


			// For scrolling components section with background
			ImGui::PushStyleColor(ImGuiCol_ChildBg, FlatEngine::F_logBgColor);
			ImGui::BeginChild("ComponentsSectionBg", Vector2(0,ImGui::GetContentRegionAvail().y - 30), FlatEngine::F_childFlags);
			ImGui::PopStyleColor();

			if (focusedObject != nullptr)
			{
				long queuedForDelete = -1;

				// Transform
				FlatEngine::Transform* transform = focusedObject->GetTransform();
				if (transform != nullptr)
				{
					BeginComponent(transform, queuedForDelete);
					if (!transform->IsCollapsed())
						RenderTransformComponent(transform);
					EndComponent(transform);
				}

				// Sprite
				Sprite* sprite = FlatEngine::GetLoadedScene()->GetSpriteByOwner(focusedObjectID);
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

				// ScriptComponent
				std::vector<ScriptComponent*> scripts = focusedObject->GetScripts();
				for (ScriptComponent* script : scripts)
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

				// BoxCollider
				std::vector<BoxCollider*> boxColliders = focusedObject->GetBoxColliders();
				for (BoxCollider* boxCollider : boxColliders)
				{
					BeginComponent(boxCollider, queuedForDelete);
					if (!boxCollider->IsCollapsed())
						RenderBoxColliderComponent(boxCollider);
					EndComponent(boxCollider);
				}

				// CircleCollider
				CircleCollider* circleCollider = focusedObject->GetCircleCollider();
				if (circleCollider != nullptr)
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

			// Render the Adding Components button
			FlatEngine::RenderButton("Add Component", Vector2(ImGui::GetContentRegionAvail().x, 0));
			if (ImGui::BeginPopupContextItem("##AddComponent", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
			{
				L_ShowAddComponentsWindow();
				ImGui::EndPopup();
			}
		}

		FlatEngine::EndWindow();
	}
}