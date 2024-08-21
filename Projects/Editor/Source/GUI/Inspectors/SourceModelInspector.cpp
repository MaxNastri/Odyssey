#include "SourceModelInspector.h"
#include "AssetManager.h"
#include "SourceModel.h"

namespace Odyssey
{
	SourceModelInspector::SourceModelInspector(GUID guid)
	{
		m_Model = AssetManager::LoadSourceModel(guid);
	}

	void SourceModelInspector::Draw()
	{
	}
}