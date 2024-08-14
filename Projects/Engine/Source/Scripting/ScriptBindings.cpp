#include "ScriptBindings.h"
#include "InternalCalls.hpp"

namespace Odyssey
{
#define ADD_INTERNAL_CALL(func) frameworkAssembly.AddInternalCall("Odyssey.InternalCalls", #func, (void*)InternalCalls::func)

	void ScriptBindings::Initialize(Coral::ManagedAssembly frameworkAssembly)
	{
		ADD_INTERNAL_CALL(GameObject_Create);

		frameworkAssembly.UploadInternalCalls();
	}
}