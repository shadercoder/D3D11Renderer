#pragma once
#include "../include/Clair/VertexLayout.h"

namespace Clair {
	class Serialization {
	public:
		static Clair::VertexLayout readVertexLayoutFromBytes(const char*& data) {
			Clair::VertexLayout vertexLayout{};
			size_t layoutSize {0};
			memcpy(&layoutSize, data, sizeof(size_t));
			data += sizeof(size_t);
			for (size_t i {0}; i < layoutSize; ++i) {
				Clair::VertexAttribute element{};
				size_t strSize {0};
				memcpy(&strSize, data, sizeof(size_t));
				data += sizeof(size_t);
				char* const str {new char[strSize + 1]};
				memcpy(str, data, sizeof(char) * strSize);
				str[strSize] = '\0';
				element.name = std::string(str);
				delete[] str;
				data += sizeof(char) * strSize;
				int format {0};
				memcpy(&format, data, sizeof(int));
				element.format =
					static_cast<Clair::VertexAttribute::Format>(format);
				data += sizeof(int);
				vertexLayout.push_back(element);
			}
			return vertexLayout;
		}

		static void writeVertexLayoutToFile(FILE* file,
									  Clair::VertexLayout& out_vertexLayout) {
			const size_t layoutSize {out_vertexLayout.size()};
			fwrite(&layoutSize, sizeof(size_t), 1, file);
			for (size_t i {0}; i < layoutSize; ++i) {
				const auto element = out_vertexLayout[i];
				const size_t strSize {element.name.length()};
				fwrite(&strSize, sizeof(size_t), 1, file);
				const char* str {element.name.c_str()};
				fwrite(str, sizeof(char), strSize, file);
				const int format {static_cast<int>(element.format)};
				fwrite(&format, sizeof(int), 1, file);
			}
		}
	};
}