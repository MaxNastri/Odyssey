#include "FBXAssetImporter.h"
#include "ufbx.h"
#include "Log.h"
#include "GeometryUtil.h"

namespace Odyssey
{
	inline static float2 ToFloat2(ufbx_vec2 vec2)
	{
		return float2(vec2.x, vec2.y);
	}

	inline static float3 ToFloat3(ufbx_vec3 vec3)
	{
		return float3(vec3.x, vec3.y, vec3.z);
	}

	inline static quat ToQuat(ufbx_quat q)
	{
		return quat((float)q.w, (float)q.x, (float)q.y, (float)q.z);
	}

	inline static mat4 ToMat4(ufbx_matrix mat)
	{
		float4 r0 = float4(ToFloat3(mat.cols[0]), 0.0f);
		float4 r1 = float4(ToFloat3(mat.cols[1]), 0.0f);
		float4 r2 = float4(ToFloat3(mat.cols[2]), 0.0f);
		float4 r3 = float4(ToFloat3(mat.cols[3]), 1.0f);

		return mat4(r0, r1, r2, r3);
	}

	inline static void LoadRig(ufbx_skin_deformer* skin, RigImportData& rigData)
	{
		rigData.RotationOffset = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));

		std::map<std::string, std::string> boneToParentName;

		for (size_t i = 0; i < skin->clusters.count; i++)
		{
			ufbx_skin_cluster* cluster = skin->clusters[i];
			std::string boneName = cluster->bone_node->name.data;

			auto& bone = rigData.Bones[boneName];
			bone.Name = boneName;
			bone.Index = (int32_t)i;
			bone.inverseBindpose = ToMat4(cluster->geometry_to_bone);
			bone.ParentIndex = -1;

			// Build a map of child -> parent nodes
			// Note: We insert an empty string for the root bone
			ufbx_node* parent = cluster->bone_node->parent;
			boneToParentName[bone.Name] = parent->is_root ? "" : parent->name.data;
		}

		// Assign the parent indices now that the bones are populated
		for (auto& [boneName, bone] : rigData.Bones)
		{
			const std::string& parentName = boneToParentName[boneName];

			// Check against an empty string to rule out the root bone being an error case
			if (!rigData.Bones.contains(parentName))
			{
				if (parentName != "")
					Log::Error(std::format("[FBXAssetImporter] Unable to find parent bone with name: {}", parentName));

				continue;
			}

			bone.ParentIndex = rigData.Bones[parentName].Index;
		}

		rigData.BoneCount = skin->clusters.count;
	}

	inline static void LoadMesh(ufbx_mesh* mesh, ufbx_skin_deformer* skin, MeshImportData& meshData)
	{
		size_t submeshIdx = 0;

		for (ufbx_mesh_part& submesh : mesh->material_parts)
		{
			std::vector<Vertex> vertices;
			std::vector<uint32_t> triIndices;

			triIndices.resize(mesh->max_face_triangles * 3);

			for (uint32_t faceIdx : submesh.face_indices)
			{
				ufbx_face face = mesh->faces[faceIdx];
				uint32_t triCount = ufbx_triangulate_face(triIndices.data(), triIndices.size(), mesh, face);

				for (size_t i = 0; i < triCount * 3; i++)
				{
					uint32_t index = triIndices[i];
					Vertex& vertex = vertices.emplace_back();
					vertex.Position = ToFloat3(mesh->vertex_position[index]);
					vertex.Normal = ToFloat3(mesh->vertex_normal[index]);
					vertex.TexCoord0 = ToFloat2(mesh->vertex_uv[index]);
					vertex.TexCoord0.y = 1.0f - vertex.TexCoord0.y;

					// Check if this is a skinned mesh
					if (skin)
					{
						// Get the skin
						uint32_t vertexIndex = mesh->vertex_indices[index];
						ufbx_skin_vertex skinVertex = skin->vertices[vertexIndex];

						// We only support up to 4 weights/indices
						float totalWeight = 0.0f;
						uint32_t weightCount = std::min(skinVertex.num_weights, uint32_t(4));

						for (size_t w = 0; w < weightCount; w++)
						{
							ufbx_skin_weight skinWeight = skin->weights[skinVertex.weight_begin + w];

							vertex.BoneIndices[(length_t)w] = (float)skinWeight.cluster_index;
							vertex.BoneWeights[(length_t)w] = (float)skinWeight.weight;
							totalWeight += (float)skinWeight.weight;
						}

						// Re-normalize the weights
						for (size_t w = 0; w < weightCount; w++)
						{
							vertex.BoneWeights[(length_t)w] /= totalWeight;
						}
					}
				}
			}

			ufbx_vertex_stream streams[1] =
			{
				{ vertices.data(), vertices.size(), sizeof(Vertex) },
			};

			std::vector<uint32_t> indices;
			indices.resize(submesh.num_triangles * 3);

			// This will de-duplicate vertices and modify the passed in vertices/indices
			size_t vertCount = ufbx_generate_indices(streams, 1, indices.data(), indices.size(), nullptr, nullptr);
			vertices.resize(vertCount);

			GeometryUtil::GenerateTangents(vertices, indices);

			meshData.VertexLists.push_back(vertices);
			meshData.IndexLists.push_back(indices);
			submeshIdx++;
		}
	}

	inline static void LoadAnimationClip(ufbx_scene* scene, ufbx_anim_stack* stack, AnimationImportData& animData)
	{
		ufbx_skin_deformer* skin = scene->skin_deformers[0];
		ufbx_anim* animation = stack->anim;

		animData.Start = animation->time_begin;
		animData.Duration = animation->time_end - animation->time_begin;
		animData.FramesPerSecond = 30;
		animData.Name = stack->name.data;

		ufbx_bake_opts opts
		{
			.resample_rate = 30.0,
			.skip_node_transforms = false,
		};

		ufbx_baked_anim* bake = ufbx_bake_anim(scene, animation, &opts, nullptr);

		if (!bake)
		{
			Log::Error("[FBXAssetImporter] Unable to load baked animation data.");
			return;
		}

		for (const ufbx_baked_node& bakeNode : bake->nodes)
		{
			ufbx_node* sceneNode = scene->nodes[bakeNode.typed_id];
			ufbx_bone* bone = sceneNode->bone;

			for (auto& key : bakeNode.translation_keys)
			{
				std::string nodeName = bone->name.data;
				animData.BoneKeyframes[nodeName].AddPositionKey(key.time, ToFloat3(key.value));
			}

			for (auto& key : bakeNode.rotation_keys)
			{
				std::string nodeName = bone->name.data;
				animData.BoneKeyframes[nodeName].AddRotationKey(key.time, ToQuat(key.value));
			}

			for (auto& key : bakeNode.scale_keys)
			{
				std::string nodeName = bone->name.data;
				animData.BoneKeyframes[nodeName].AddScaleKey(key.time, ToFloat3(key.value));
			}
		}

		ufbx_free_baked_anim(bake);

		// Add the local transform data as dummy keyframes for any missing bones
		for (size_t i = 0; i < skin->clusters.count; i++)
		{
			ufbx_skin_cluster* cluster = skin->clusters[i];
			std::string boneName = cluster->bone_node->name.data;

			if (!animData.BoneKeyframes.contains(boneName))
			{
				auto& keyframe = animData.BoneKeyframes[boneName];

				keyframe.AddPositionKey(animData.Start, ToFloat3(cluster->bone_node->local_transform.translation));
				keyframe.AddPositionKey(animData.Duration, ToFloat3(cluster->bone_node->local_transform.translation));

				keyframe.AddRotationKey(animData.Start, ToQuat(cluster->bone_node->local_transform.rotation));
				keyframe.AddRotationKey(animData.Duration, ToQuat(cluster->bone_node->local_transform.rotation));

				keyframe.AddScaleKey(animData.Start, ToFloat3(cluster->bone_node->local_transform.scale));
				keyframe.AddScaleKey(animData.Duration, ToFloat3(cluster->bone_node->local_transform.scale));
			}
			else
			{
				// Let's validate there are at least 2 keys per component
				auto& keyframe = animData.BoneKeyframes[boneName];
				auto& positionKeys = keyframe.GetPositionKeys();
				auto& rotationKeys = keyframe.GetRotationKeys();
				auto& scaleKeys = keyframe.GetScaleKeys();

				if (positionKeys.size() == 0)
				{
					keyframe.AddPositionKey(animData.Start, ToFloat3(cluster->bone_node->local_transform.translation));
					keyframe.AddPositionKey(animData.Duration, ToFloat3(cluster->bone_node->local_transform.translation));
				}
				else if (positionKeys.size() == 1)
				{
					if (positionKeys[0].Time == animData.Start)
					{
						keyframe.AddPositionKey(animData.Duration, positionKeys[0].Value);
					}
					else if (positionKeys[0].Time == animData.Duration)
					{
						keyframe.AddPositionKey(animData.Start, positionKeys[0].Value);
					}
					else
					{
						keyframe.AddPositionKey(animData.Start, positionKeys[0].Value);
						keyframe.AddPositionKey(animData.Duration, positionKeys[0].Value);
					}
				}

				if (rotationKeys.size() == 0)
				{
					keyframe.AddRotationKey(animData.Start, ToQuat(cluster->bone_node->local_transform.rotation));
					keyframe.AddRotationKey(animData.Duration, ToQuat(cluster->bone_node->local_transform.rotation));
				}
				else if (rotationKeys.size() == 1)
				{
					if (rotationKeys[0].Time == animData.Start)
					{
						keyframe.AddRotationKey(animData.Duration, rotationKeys[0].Value);
					}
					else if (rotationKeys[0].Time == animData.Duration)
					{
						keyframe.AddRotationKey(animData.Start, rotationKeys[0].Value);
					}
					else
					{
						keyframe.AddRotationKey(animData.Start, rotationKeys[0].Value);
						keyframe.AddRotationKey(animData.Duration, rotationKeys[0].Value);
					}
				}

				if (scaleKeys.size() == 0)
				{
					keyframe.AddScaleKey(animData.Start, ToFloat3(cluster->bone_node->local_transform.scale));
					keyframe.AddScaleKey(animData.Duration, ToFloat3(cluster->bone_node->local_transform.scale));
				}
				else if (scaleKeys.size() == 1)
				{
					if (scaleKeys[0].Time == animData.Start)
					{
						keyframe.AddScaleKey(animData.Duration, scaleKeys[0].Value);
					}
					else if (scaleKeys[0].Time == animData.Duration)
					{
						keyframe.AddScaleKey(animData.Start, scaleKeys[0].Value);
					}
					else
					{
						keyframe.AddScaleKey(animData.Start, scaleKeys[0].Value);
						keyframe.AddScaleKey(animData.Duration, scaleKeys[0].Value);
					}
				}
			}
		}

		size_t maxFrames = (size_t)std::ceil(animData.Duration * (double)animData.FramesPerSecond);
		double step = 1.0 / (double)animData.FramesPerSecond;

		// Fill in any missing keyframe times make sure all keyframes are the same length
		for (auto& [boneName, boneKeyframe] : animData.BoneKeyframes)
		{
			// Position Keys
			{
				auto positionKeys = boneKeyframe.GetPositionKeys();
				BoneKeyframe::PositionKey first = positionKeys[0];
				BoneKeyframe::PositionKey last = positionKeys[positionKeys.size() - 1];

				for (int i = 1; i < maxFrames; i++)
				{
					double frameTime = (double)i * step;
					if (!boneKeyframe.HasPositionKey(frameTime))
					{
						double blend = frameTime / animData.Duration;
						boneKeyframe.AddPositionKey(frameTime, glm::mix(first.Value, last.Value, blend));
					}
				}
			}

			// Rotation Keys
			{
				auto rotationKeys = boneKeyframe.GetRotationKeys();
				BoneKeyframe::RotationKey first = rotationKeys[0];
				BoneKeyframe::RotationKey last = rotationKeys[rotationKeys.size() - 1];

				for (int i = 1; i < maxFrames; i++)
				{
					double frameTime = (double)i * step;
					if (!boneKeyframe.HasRotationKey(frameTime))
					{
						float blend = (float)(frameTime / animData.Duration);
						boneKeyframe.AddRotationKey(frameTime, glm::slerp(first.Value, last.Value, blend));
					}
				}
			}

			// Scale Keys
			{
				auto scaleKeys = boneKeyframe.GetScaleKeys();
				BoneKeyframe::ScaleKey first = scaleKeys[0];
				BoneKeyframe::ScaleKey last = scaleKeys[scaleKeys.size() - 1];

				for (int i = 1; i < maxFrames; i++)
				{
					double frameTime = (double)i * step;
					if (!boneKeyframe.HasScaleKey(frameTime))
					{
						double blend = frameTime / animData.Duration;
						boneKeyframe.AddScaleKey(frameTime, glm::mix(first.Value, last.Value, blend));
					}
				}
			}
		}

		int d = 0;
	}

	bool FBXAssetImporter::Import(const Path& modelPath)
	{
		// Open the scene but ignore all content so we can determine what program exported this file
		ufbx_load_opts ignoreAllOpts = {
			.ignore_all_content = true
		};

		ufbx_error error;
		ufbx_scene* scene = ufbx_load_file(modelPath.string().c_str(), &ignoreAllOpts, &error);

		if (!scene)
		{
			Log::Error(std::format("[FBXAssetImporter] Failed to load fbx file: {}", modelPath.string()));
			Log::Error(error.description.data);
			return false;
		}

		// Check if this is a blender export
		bool blenderExport = scene->metadata.exporter == UFBX_EXPORTER_BLENDER_ASCII || scene->metadata.exporter == UFBX_EXPORTER_BLENDER_BINARY;

		ufbx_free_scene(scene);

		// Now load the scene completely while converting to left handed y-up
		ufbx_load_opts opts = {  };
		opts.target_axes = ufbx_axes_left_handed_y_up;
		opts.target_unit_meters = 1.0f;
		opts.handedness_conversion_axis = UFBX_MIRROR_AXIS_X;
		//opts.target_camera_axes = ufbx_axes_left_handed_y_up;
		//opts.target_light_axes = ufbx_axes_left_handed_y_up;

		if (blenderExport)
			opts.space_conversion = UFBX_SPACE_CONVERSION_ADJUST_TRANSFORMS;
		else
			opts.space_conversion = UFBX_SPACE_CONVERSION_MODIFY_GEOMETRY;


		scene = ufbx_load_file(modelPath.string().c_str(), &opts, &error);

		if (!scene)
		{
			Log::Error(std::format("[FBXAssetImporter] Failed to load fbx file: {}", modelPath.string()));
			Log::Error(error.description.data);
			return false;
		}

		ufbx_skin_deformer* skin = scene->skin_deformers.count > 0 ? scene->skin_deformers[0] : nullptr;

		if (skin)
			LoadRig(skin, m_RigData);

		// The scene is fully loaded, start parsing
		for (ufbx_mesh* mesh : scene->meshes)
		{
			LoadMesh(mesh, skin, m_MeshData);
		}

		if (scene->anim_stacks.count > 0 && scene->skin_deformers.count > 0)
			LoadAnimationClip(scene, scene->anim_stacks[0], m_AnimationData);

		return true;
	}
}