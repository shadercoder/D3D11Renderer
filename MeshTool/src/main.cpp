#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>
#include "../../Clair/include/clair/vertexLayout.h"
#include "../../Clair/src/serialization.h"

using namespace Assimp;

void convertMesh(const std::string& inFile, const std::string& outFile);

int main(int argc, char* argv[]) {
	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			convertMesh(argv[i], "test.txt");
		}
	} else {
		convertMesh("bunny.obj", "test.txt");
					//"D:/School/Specialization/ClairRenderer/Sample/"
					//"data/model.cmod");
	}
	return 0;
}

void convertMesh(const std::string& inFile, const std::string& outFile) {
	Importer importer;
	const aiScene* const scene = importer.ReadFile(inFile,
												   aiProcess_Triangulate);
	if (!scene) {
		std::cout << "Couldn't load " << inFile << std::endl;
		getchar();
		return;
	}
	std::cout << "Converting " << inFile << std::endl;

	FILE* file {nullptr};
	fopen_s(&file, outFile.c_str(), "wb");
	aiMesh& mesh = *scene->mMeshes[0];

	// fill vertex layout + stride
	unsigned stride {0};
	Clair::VertexLayout vertexLayout {};
	if (mesh.HasPositions()) {
		vertexLayout.push_back({"POSITION",
								Clair::VertexAttribute::Format::FLOAT3});
		stride += sizeof(float) * 3;
	}
	if (mesh.HasNormals()) {
		vertexLayout.push_back({"NORMAL",
								Clair::VertexAttribute::Format::FLOAT3});
		stride += sizeof(float) * 3;
	}
	Clair::Serialization::writeVertexLayoutToFile(file, vertexLayout);
	fwrite(&stride, sizeof(unsigned), 1, file);

	// write model
	fwrite(&mesh.mNumVertices, sizeof(unsigned), 1, file);
	for (unsigned j = 0; j < mesh.mNumVertices; j++) {
		const float v[3] = { mesh.mVertices[j].x,
							 mesh.mVertices[j].y,
							 mesh.mVertices[j].z };
		fwrite(v, sizeof(float), 3, file);
		const float n[3] = { mesh.mNormals[j].x,
							 mesh.mNormals[j].y,
							 mesh.mNormals[j].z };
		fwrite(n, sizeof(float), 3, file);
	}
	const unsigned numIndices = mesh.mNumFaces * 3;
	fwrite(&numIndices, sizeof(unsigned), 1, file);
	for (unsigned j = 0 ; j < mesh.mNumFaces ; j++) {
		const unsigned ind[3] = {
			mesh.mFaces[j].mIndices[0],
			mesh.mFaces[j].mIndices[1],
			mesh.mFaces[j].mIndices[2] };
		fwrite(ind, sizeof(unsigned), 3, file);
		//assert(face.mNumIndices == 3);
	}
	fclose(file);
}