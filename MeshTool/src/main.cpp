#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>

using namespace Assimp;

void convertMesh(const std::string& inFile, const std::string& outFile);

int main(int argc, char* argv[]) {
	for (int i = 1; i < argc; ++i) {
		convertMesh(argv[i], "test.txt");
	}
	//convertMesh("cube.obj", "test.txt");
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
	FILE* outputFile;
	fopen_s(&outputFile, outFile.c_str(), "wb");
	//for (unsigned i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh& mesh = *scene->mMeshes[0];
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
	}
}