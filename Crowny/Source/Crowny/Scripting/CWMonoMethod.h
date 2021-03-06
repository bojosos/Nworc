#pragma once

#include "Crowny/Scripting/CWMonoVisibility.h"
#include "Crowny/Scripting/CWMono.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/metadata.h>
END_MONO_INCLUDE

namespace Crowny
{	
	class CWMonoClass;
	
	class CWMonoMethod
	{
	public:
		CWMonoMethod(MonoMethod* method);
		const std::string& GetName() const { return m_Name; };
		const std::string& GetFullDeclName() const { return m_FullDeclName; }
		std::vector<CWMonoClass*> GetParameterTypes();
		CWMonoClass* GetReturnType();
		bool IsStatic();
		bool IsVirtual();
		CWMonoVisibility GetVisibility();
		bool HasAttribute(CWMonoClass* monoClass) const;
		MonoObject* GetAttribute(CWMonoClass* monoClass) const;
		void* GetThunk() const;
		MonoObject* Invoke(MonoObject* instance, void** params);

	private:
		MonoMethod* m_Method = nullptr;
		MonoMethodSignature* m_Signature = nullptr;
		std::string m_Name;
		std::string m_FullDeclName;
	};
}