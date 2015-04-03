#pragma once
#include <string>
#include <vector>

namespace Clair {
	class Scene;
	class Matrix;
	class InputLayout;
	class VertexLayoutOld;
	class VertexBuffer;
	struct VertexBufferDesc;
	class VertexShader;
	class PixelShader;
	class Mesh;
	struct MeshDesc;
	class SubMaterial;

	class Renderer {
	public:
		static bool initialize(HWND hwnd);
		static void terminate();

		static void setViewport(float x, float y, float width, float height);
		static void clear();
		static void finalizeFrame();
		static void render(Scene* scene);

		static Scene* createScene();
		static VertexShader* createVertexShader(char* byteCode, size_t size);
		static PixelShader* createPixelShader(char* byteCode, size_t size);

		static InputLayout* createInputLayout(VertexLayoutOld& desc,
											  VertexShader* vs);

		static Mesh* createMesh(MeshDesc& desc);
		static void createMaterial(char* data, VertexShader*& outVs,
								   PixelShader*& outPs);

		static SubMaterial* createSubMaterial(char* data);

		static void setCameraMatrix(const Clair::Matrix& m);
	};
};
