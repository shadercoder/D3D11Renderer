#pragma once
#include <string>
#include <vector>

namespace Clair {
	class Scene;
	class Matrix;
	class InputLayout;
	class VertexLayout;
	class VertexBuffer;
	struct VertexBufferDesc;
	class VertexShader;
	class PixelShader;
	class Mesh;
	struct MeshDesc;

	class Renderer {
	public:
		static bool initialize(HWND hwnd, const std::string& clairDataPath);
		static void terminate();

		static void setViewport(float x, float y, float width, float height);
		static void clear();
		static void finalizeFrame();
		static void render(Scene* scene);

		static Scene* createScene();
		static VertexShader* createVertexShader(char* byteCode, size_t size);
		static PixelShader* createPixelShader(char* byteCode, size_t size);

		static InputLayout* createInputLayout(VertexLayout& desc,
											  VertexShader* vs);

		static Mesh* createMesh(MeshDesc& desc);
		static void createMaterial(char* data, VertexShader*& outVs,
								   PixelShader*& outPs);

		static void setCameraMatrix(const Clair::Matrix& m);
	};
};
