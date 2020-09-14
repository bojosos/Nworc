#include "cwepch.h"

#include "ImGuiHierarchyPanel.h"

#include "Crowny/SceneManagement/SceneManager.h"
#include "Crowny/Ecs/Entity.h"
#include "Crowny/Ecs/Components.h"
#include "Crowny/Input/Input.h"

#include <imgui.h>
#include "Crowny/Input/KeyCodes.h"

namespace Crowny
{
	Entity ImGuiHierarchyPanel::s_SelectedEntity;

	ImGuiHierarchyPanel::ImGuiHierarchyPanel(const std::string& name) : ImGuiPanel(name)
	{

	}

	void ImGuiHierarchyPanel::DisplayTree(Entity& e, uint32_t i)
	{
		if (!e.IsValid())
			return;

		auto& rc = e.GetComponent<RelationshipComponent>();
		auto& tc = e.GetComponent<TagComponent>();

		std::string name = tc.Tag.empty() ? "Entity" : tc.Tag.c_str();
		ImGuiTreeNodeFlags selected = (m_SelectedItems.find(e) != m_SelectedItems.end()) ? ImGuiTreeNodeFlags_Selected : 0;
		ImGui::AlignTextToFramePadding();

		ImGui::PushID(i);

		if (!rc.Children.empty())
		{
			bool open = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | selected);

			//if (ImGui::BeginDragDropSource()) {
			//	ImGui::SetDragDropPayload("_TREENODE", nullptr, 0);
			//		ImGui::Text("wat");
			//		ImGui::EndDragDropSource();
			//}
			
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::Selectable("New Entity"))
				{
					m_NewEntityParent = &e;
				}
				
				if (ImGui::Selectable("Delete"))
				{
					e.Destroy();
					ImGuiHierarchyPanel::s_SelectedEntity = SceneManager::GetActiveScene()->GetRootEntity();
				}

				ImGui::EndPopup();
			}

			if (Input::IsKeyPressed(Key::Delete))
			{
				for (int i = 0; i < rc.Children.size(); i++)
				{
					if (rc.Children[i] == ImGuiHierarchyPanel::s_SelectedEntity)
					{
						rc.Children[i].Destroy();
						rc.Children.erase(rc.Children.begin() + i);
						break;
					}
				}

				ImGuiHierarchyPanel::s_SelectedEntity = SceneManager::GetActiveScene()->GetRootEntity();
			}

			if (open)
			{
				if (ImGui::IsItemClicked())
				{
					if (!m_SelectedItems.empty() && Input::IsKeyPressed(Key::LeftControl))
					{
						if (m_SelectedItems.find(e) == m_SelectedItems.end())
							m_SelectedItems.insert(e);
						else
							m_SelectedItems.erase(e);
					}
					else
					{
						m_SelectedItems.clear();
						m_SelectedItems.insert(e);
						ImGuiHierarchyPanel::s_SelectedEntity = e;
					}
				}

				for (auto& c : rc.Children) 
				{
					DisplayTree(c, ++i);
				}

				ImGui::TreePop();
			}
		}
		else
		{
			ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_NoTreePushOnOpen | selected | ImGuiTreeNodeFlags_Leaf);

			if(ImGui::IsItemClicked())
			{
				if (!m_SelectedItems.empty() && Input::IsKeyPressed(Key::LeftControl))
				{
					if (m_SelectedItems.find(e) == m_SelectedItems.end())
						m_SelectedItems.insert(e);
					else
						m_SelectedItems.erase(e);
				}
				else
				{
					m_SelectedItems.clear();
					m_SelectedItems.insert(e);
					ImGuiHierarchyPanel::s_SelectedEntity = e;
				}
			}
			
			if (Input::IsKeyPressed(Key::Delete))
			{
				for (int i = 0; i < rc.Children.size(); i++)
				{
					if (rc.Children[i] == ImGuiHierarchyPanel::s_SelectedEntity)
					{
						rc.Children[i].Destroy();
						rc.Children.erase(rc.Children.begin() + i);
						break;
					}
				}

				ImGuiHierarchyPanel::s_SelectedEntity = SceneManager::GetActiveScene()->GetRootEntity();
			}

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::Selectable("New Entity"))
				{
					m_NewEntityParent = &e;
				}

				if (ImGui::Selectable("Delete"))
				{
					e.Destroy();
					ImGuiHierarchyPanel::s_SelectedEntity = SceneManager::GetActiveScene()->GetRootEntity();
				}

				ImGui::EndPopup();
			}
		}

		ImGui::PopID();
	}

	void ImGuiHierarchyPanel::Update()
	{
		if (m_NewEntityParent)
		{
			ImGuiHierarchyPanel::s_SelectedEntity = m_NewEntityParent->AddChild("New Entity");
			m_NewEntityParent = nullptr;
		}
	}

	void ImGuiHierarchyPanel::Render()
	{
		ImGui::Begin("Hierarchy", &m_Shown);
		Scene* activeScene = SceneManager::GetActiveScene();
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("New Entity"))
			{
				m_NewEntityParent = &activeScene->GetRootEntity();
			}
			ImGui::EndPopup();
		}

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);

		DisplayTree(activeScene->GetRootEntity());

		ImGui::End();
	}

	void ImGuiHierarchyPanel::Show()
	{
		m_Shown = true;
	}

	void ImGuiHierarchyPanel::Hide()
	{
		m_Shown = false;
	}

}
