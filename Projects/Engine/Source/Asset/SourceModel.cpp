#include "SourceModel.h"

namespace Odyssey
{
	SourceModel::SourceModel(const Path& sourcePath)
	{
		// TODO: Add validation
		m_Importer.Import(sourcePath);
	}
}