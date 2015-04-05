#pragma once
#include "Clair/VertexLayout.h"

namespace Clair {
	//class Material;
	class VertexShader;
	class PixelShader;
	class VertexBuffer;
	class IndexBuffer;

	class LowLevelRenderer {
	public:
		static bool initialize(HWND hwnd);
		static void terminate();

		static void clear();
		static void finalizeFrame();
		static void setViewport(int x, int y, int width, int height);

		static VertexShader* createVertexShader(char* vsData,
												size_t vsSize);
		static PixelShader* createPixelShader(char* psData,
											  size_t psSize);
		static VertexBuffer* createVertexBuffer(char* bufferData,
												unsigned bufferSize);
		static IndexBuffer* createIndexBuffer(unsigned* bufferData,
											  unsigned bufferSize);

		static void destroyVertexShader(VertexShader* vertexShader);
		static void destroyPixelShader(PixelShader* pixelShader);
		static void destroyVertexBuffer(VertexBuffer* vertexBuffer);
		static void destroyIndexBuffer(IndexBuffer* indexBuffer);
	};
}