#pragma once
#include "Clair/VertexLayout.h"

struct HWND__;
typedef HWND__* HWND;

namespace Clair {
	enum class RenderPass;
	class Scene;
	class Matrix;
	class VertexBuffer;
	class IndexBuffer;
	class InputLayout;
	class VertexShader;
	class PixelShader;

	class LowLevelRenderer {
	public:
		static bool initialize(HWND hwnd);
		static void terminate();

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
		static VertexShader* createVertexShader(char* vsData,
												size_t vsSize);
		static PixelShader* createPixelShader(char* psData,
											  size_t psSize);

		static void destroyVertexBuffer(VertexBuffer* vertexBuffer);
		static void destroyIndexBuffer(IndexBuffer* indexBuffer);
		static void destroyInputLayout(InputLayout* inputLayout);
		static void destroyVertexShader(VertexShader* vertexShader);
		static void destroyPixelShader(PixelShader* pixelShader);
	};
}