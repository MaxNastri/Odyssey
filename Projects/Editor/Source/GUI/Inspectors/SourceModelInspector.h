#pragma once
#include "Inspector.h"
#include "GUID.h"
#include "AssetHandle.h"
#include "StringDrawer.h"

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
		void OnDstPathChanged(const std::string& path) { m_DstPath = path; }

	private:
		AssetHandle<SourceModel> m_Model;
		std::vector<StringDrawer> m_Drawers;
		Path m_DstPath;
	};
}