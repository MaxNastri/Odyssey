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
		virtual void Draw() override;

	private:
		void OnDstPathChanged(std::string_view path) { m_DstPath = path; }

	private:
		std::shared_ptr<SourceModel> m_Model;
		std::vector<StringDrawer> m_Drawers;
		Path m_DstPath;
	};
}