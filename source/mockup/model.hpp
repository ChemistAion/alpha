#pragma once

//#include "scene.hpp"
//#include "node.hpp"

//#include "skin.hpp"
//#include "camera.hpp"
//#include "light.hpp"
//#include "mesh.hpp"

//#include "animation.hpp"

#include "gltf/model.hpp"

#include "engine/buffer.hpp"
#include "engine/texture.hpp"

namespace Alpha::Mockup
{
	class Model
	{
		//std::vector<std::shared_ptr<Engine::Texture>> textures_;

		//std::vector<Scene> scenes_;
		//std::vector<Node> nodes_;
		
		//std::vector<Skin> skins_;
		//std::vector<Camera> cameras_;
		//std::vector<Light> lights_;
		std::vector<Mesh> meshes_;

		std::vector<Data> buffers_data_;
		
		//std::vector<Animation> animations_;
		Mesh ImportMesh(const glTF::Mesh& element, const glTF::Model& model);
		Data ImportBufferData(const glTF::Buffer& buffer, const glTF::Model& model);
	public:
		void Import(const glTF::Model& model);

		//void Load(const std::string& path);
		//void Save(const std::string& path);
	};
}