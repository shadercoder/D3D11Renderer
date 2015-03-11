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
	convertMesh("sphere.obj", "test.txt");
    return 0;
}

void convertMesh(const std::string& inFile, const std::string& outFile) {
	Importer importer;
	const aiScene* const scene = importer.ReadFile(inFile, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
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
			const float v[3] = { mesh.mVertices[j].x, mesh.mVertices[j].y, mesh.mVertices[j].z };
			fwrite(v, sizeof(float), 3, outputFile);
			const float n[3] = { mesh.mNormals[j].x, mesh.mNormals[j].y, mesh.mNormals[j].z };
			fwrite(n, sizeof(float), 3, outputFile);
			//m.positions.	push_back(vec3());
			//m.normals.		push_back(vec3(mesh.mNormals[j].x,			mesh.mNormals[j].y,			mesh.mNormals[j].z));
			//if (mesh.HasTextureCoords(0)) m.uvs.push_back(vec2(mesh.mTextureCoords[0][j].x,	mesh.mTextureCoords[0][j].y));
			//if (mesh.HasTextureCoords(0)) textured = true;
		}
		//const unsigned numIndicies = m.indicies.size();
		const unsigned numIndices = mesh.mNumFaces * 3;
		fwrite(&numIndices, sizeof(unsigned), 1, outputFile);
		for (unsigned j = 0 ; j < mesh.mNumFaces ; j++) {
			const unsigned ind[3] = { mesh.mFaces[j].mIndices[0], mesh.mFaces[j].mIndices[1], mesh.mFaces[j].mIndices[2] };
			fwrite(ind, sizeof(unsigned), 3, outputFile);
			//const aiFace& face = mesh.mFaces[j];
			//assert(face.mNumIndices == 3);
			//m.indicies.push_back(face.mIndices[0] + numIndicies);
			//m.indicies.push_back(face.mIndices[1] + numIndicies);
			//m.indicies.push_back(face.mIndices[2] + numIndicies);
		}
	}
}