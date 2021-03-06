#pragma once

#include <entt/entt.hpp>

#include "Crowny/Ecs/Components.h"
#include "Crowny/Ecs/Entity.h"
#include <mono/metadata/object.h>

namespace Crowny
{

	class ScriptComponent
	{
	public:
    struct ComponentInfo
    {
      std::function<MonoObject*(Entity)> AddCallback;
      std::function<bool(Entity)> HasCallback;
      std::function<MonoObject*(Entity)> GetCallback;
    };
    
		static void InitRuntimeFunctions();
		static std::unordered_map<uint32_t, MonoObject*> s_EntityComponents;
	private:
    template <typename Component>
    static void RegisterComponent();
    static std::unordered_map<MonoClass*, uint32_t> s_TypeMap;
    static std::unordered_map<uint32_t, ComponentInfo> s_ComponentInfos;
		static MonoObject* Internal_GetEntity(MonoScriptComponent* component);
    static MonoObject* Internal_GetComponent(MonoScriptComponent* thisptr, MonoReflectionType* type);
    static bool Internal_HasComponent(MonoScriptComponent* thisptr, MonoReflectionType* type);
    static MonoObject* Internal_AddComponent(MonoScriptComponent* thisptr, MonoReflectionType* type);
	};

  template <typename Component>
  static MonoObject* AddComponent(Entity e)
  {
    return e.AddComponent<Component>().ManagedInstance;
  }

  template <typename Component>
  static bool HasComponent(Entity e)
  {
    return e.HasComponent<Component>();
  }

  template <typename Component>
  static MonoObject* GetComponent(Entity e)
  {
    return e.GetComponent<Component>().ManagedInstance;
  }

  template <typename Component>
  void ScriptComponent::RegisterComponent()
  {
    s_ComponentInfos[entt::type_info<Component>::id()] = { AddComponent<Component>, HasComponent<Component>, GetComponent<Component> };
  }

}
