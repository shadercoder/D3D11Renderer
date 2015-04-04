#pragma once
#include "clair/vertexLayout.h"

namespace Clair {
	//class Material;
	class VertexShader;
	class PixelShader;

	class LowLevelRenderer {
	public:
		static bool initialize(HWND hwnd);
		static void terminate();

		static void clear();
		static void finalizeFrame();
		static void setViewport(int x, int y, int width, int height);

		static VertexShader* createVertexShader(char* const vsData,
												const size_t vsSize);
		static PixelShader* createPixelShader(char* const psData,
											  const size_t psSize);
	};
}