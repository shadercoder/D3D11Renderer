#pragma once

struct ID3D11Buffer;

namespace Clair {
	class IndexBuffer {
	public:
		ID3D11Buffer* d3dBuffer {nullptr};
	};
}