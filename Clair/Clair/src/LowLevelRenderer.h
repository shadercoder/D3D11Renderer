#pragma once
#include "Clair/VertexLayout.h"
#include "Clair/RenderPass.h"

struct ID3D11Device;
struct HWND__;
typedef HWND__* HWND;

namespace Clair {
	class Scene;
	class Matrix;
	class VertexBuffer;
	class IndexBuffer;
	class InputLayout;
	class VertexShader;

	class LowLevelRenderer {
	public:
		static bool initialize(HWND hwnd);
		static void terminate();

		static ID3D11Device* getD3dDevice();
		static void clear();
		static void finalizeFrame();
		static void setViewport(int x, int y, int width, int height);
		static void render(Scene* scene);
		static void setCameraMatrix(const Matrix& m);
		static void setRenderPass(RenderPass pass);

		static VertexBuffer* createVertexBuffer(char* bufferData,
												unsigned bufferSize);
		static IndexBuffer* createIndexBuffer(unsigned* bufferData,
											  unsigned bufferSize);
		static InputLayout* createInputLayout(const VertexLayout& vertexLayout,
											  VertexShader* vertexShader);

		static void destroyVertexBuffer(VertexBuffer* vertexBuffer);
		static void destroyIndexBuffer(IndexBuffer* indexBuffer);
		static void destroyInputLayout(InputLayout* inputLayout);
	};
}