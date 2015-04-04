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
	(void)argc;
	(void)argv;
	//for (int i = 1; i < argc; ++i) {
	//	convertMesh(argv[i], "test.txt");
	//}
	convertMesh("bunny.obj",
				"D:/School/Specialization/ClairRenderer/Sample/"
				"data/model.cmod");
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

	FILE* outputFile {nullptr};
	fopen_s(&outputFile, outFile.c_str(), "wb");
	aiMesh& mesh = *scene->mMeshes[0];

	// fill vertex layout
	Clair::VertexLayout vertexLayout {};
	if (mesh.HasPositions()) {
		vertexLayout.push_back({"POSITION",
								Clair::VertexAttribute::Format::FLOAT3});
	}
	if (mesh.HasNormals()) {
		vertexLayout.push_back({"NORMAL",
								Clair::VertexAttribute::Format::FLOAT3});
	}
	Serialization::writeVertexLayoutToFile(
		outputFile, vertexLayout);

	// write model
	fwrite(&mesh.mNumVertices, sizeof(unsigned), 1, outputFile);
	for (unsigned j = 0; j < mesh.mNumVertices; j++) {
		const float v[3] = { mesh.mVertices[j].x,
							 mesh.mVertices[j].y,
							 mesh.mVertices[j].z };
		fwrite(v, sizeof(float), 3, outputFile);
		const float n[3] = { mesh.mNormals[j].x,
							 mesh.mNormals[j].y,
							 mesh.mNormals[j].z };
		fwrite(n, sizeof(float), 3, outputFile);
	}
	const unsigned numIndices = mesh.mNumFaces * 3;
	fwrite(&numIndices, sizeof(unsigned), 1, outputFile);
	for (unsigned j = 0 ; j < mesh.mNumFaces ; j++) {
		const unsigned ind[3] = {
			mesh.mFaces[j].mIndices[0],
			mesh.mFaces[j].mIndices[1],
			mesh.mFaces[j].mIndices[2] };
		fwrite(ind, sizeof(unsigned), 3, outputFile);
		//assert(face.mNumIndices == 3);
	}
	fclose(outputFile);
}