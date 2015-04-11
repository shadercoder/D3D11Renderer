#pragma once
#include "Clair/VertexLayout.h"

struct ID3D11Device;
struct ID3D11InputLayout;

namespace Clair {
	class VertexShader;

	class InputLayout {
	public:
		InputLayout(ID3D11Device* d3dDevice, const VertexLayout& vertexLayout,
					const VertexShader* vertexShader);
		~InputLayout();

		bool isValid() const;
		unsigned getStride() const;
		ID3D11InputLayout* getD3dInputLayout() const;

	private:
		bool mIsValid {false};
		unsigned mStride {0};
		ID3D11InputLayout* mD3dInputLayout {nullptr};
	};

	inline bool InputLayout::isValid() const {
		return mIsValid;
	}

	inline unsigned InputLayout::getStride() const {
		return mStride;
	}

	inline ID3D11InputLayout* InputLayout::getD3dInputLayout() const {
		return mD3dInputLayout;
	}
}