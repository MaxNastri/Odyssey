#pragma once
#include "Inspector.h"
#include "SourceModel.h"
#include "PropertyDrawers.h"

namespace Odyssey
{
	class SourceModelInspector : public Inspector
	{
	public:
		SourceModelInspector() = default;
		SourceModelInspector(GUID guid);

	public:
		virtual bool Draw() override;

	private:
		void OnDstPathChanged(std::string_view path) { m_DstPath = path; }

	private:
		Ref<SourceModel> m_Model;
		std::vector<StringDrawer> m_Drawers;
		Path m_DstPath;
	};
}