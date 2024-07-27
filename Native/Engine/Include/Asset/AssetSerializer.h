#pragma once
#include "Yaml.h"
#include <fstream>

namespace Odyssey
{
	class SerializationNode
	{
	public:
		SerializationNode() = default;
		SerializationNode(const std::string& nodeName, ryml::NodeRef node)
		{
			m_Name = nodeName;
			m_Node = node;
		}

	public:
		SerializationNode AppendChild()
		{
			return { "Child", m_Node.append_child() };
		}

		SerializationNode& CreateSequenceNode(const std::string& nodeName)
		{
			if (m_Sequences.contains(nodeName))
				return m_Sequences[nodeName];

			ryml::NodeRef nodeRef = m_Node[nodeName.c_str()];
			nodeRef |= ryml::SEQ;
			SerializationNode node{ nodeName, nodeRef };

			m_Sequences[nodeName] = node;
			return m_Sequences[nodeName];
		}

		SerializationNode GetNode(const std::string& nodeName)
		{
			return SerializationNode(nodeName, m_Node[nodeName.c_str()]);
		}

		SerializationNode GetChild(size_t index)
		{
			return SerializationNode("Child", m_Node.child(index));
		}
		template<typename T>
		void WriteData(ryml::csubstr name, const T& data)
		{
			m_Node[name] << data;
		}

		template<typename T>
		void ReadData(ryml::csubstr name, T& data)
		{
			if (m_Node[name].has_val())
				m_Node[name] >> data;
		}

		void SetMap() { m_Node |= ryml::MAP; }
		void SetSequence() { m_Node |= ryml::SEQ; }

		bool IsMap() { return m_Node.is_map(); }
		bool IsSequence() { return m_Node.is_seq(); }
		bool HasChildren() { return m_Node.has_children(); }
		bool HasChild(const std::string& childName) { return m_Node.has_child(childName.c_str()); }
		size_t ChildCount() { return m_Node.num_children(); }

	private:
		std::string m_Name;
		ryml::NodeRef m_Node;
		std::map<std::string, SerializationNode> m_Sequences;
	};

	class AssetDeserializer
	{
	public:
		AssetDeserializer() = default;
		operator bool() { return IsValid(); }

		AssetDeserializer(const std::filesystem::path& assetPath)
		{
			if (std::ifstream stream{ assetPath })
			{
				// Create the yaml root node
				std::string data((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

				m_Tree = ryml::parse_in_arena(ryml::to_csubstr(data));
				m_RootRef = m_Tree.rootref();
				m_RootNode = SerializationNode("Root", m_RootRef);
				m_DataParsed = true;

				stream.close();
			}
		}

	public:
		SerializationNode& GetRoot() { return m_RootNode; }
		bool IsValid() { return m_DataParsed; }

	private:
		ryml::Tree m_Tree;
		ryml::NodeRef m_RootRef;
		SerializationNode m_RootNode;
		std::map<std::string, SerializationNode> m_CreatedNodes;
		bool m_DataParsed = false;
	};

	class AssetSerializer
	{
	public:
		AssetSerializer()
		{
			m_RootRef = m_Tree.rootref();
			m_RootRef |= ryml::MAP;
			m_RootNode = SerializationNode("Root", m_RootRef);
		}

	public:
		SerializationNode& GetRoot() { return m_RootNode; }

		void WriteToDisk(const std::filesystem::path& path)
		{
			std::ofstream file(path, std::ios::trunc);
			if (file.is_open())
			{
				std::string fileContents = ryml::emitrs_yaml<std::string>(m_Tree);
				file << fileContents;
				file.close();
			}
		}

	private:
		ryml::Tree m_Tree;
		ryml::NodeRef m_RootRef;
		SerializationNode m_RootNode;
		std::map<std::string, SerializationNode> m_CreatedNodes;
	};



}