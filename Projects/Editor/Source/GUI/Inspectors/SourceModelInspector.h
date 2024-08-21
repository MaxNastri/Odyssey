#pragma once
#include "Inspector.h"
#include "GUID.h"
#include "AssetHandle.h"

namespace Odyssey
{
	class SourceModel;

	class SourceModelInspector : public Inspector
	{
	public:
		SourceModelInspector() = default;
		SourceModelInspector(GUID guid);

	public:
		virtual void Draw() override;

	private:
		AssetHandle<SourceModel> m_Model;
	};
}