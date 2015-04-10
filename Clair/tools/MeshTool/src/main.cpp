#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>
#include "../../../Clair/include/clair/vertexLayout.h"
#include "../../../Clair/src/serialization.h"
#include "ErrorCodes.h"

using namespace Assimp;

static std::string gInFile {"../../../samples/common/rawdata/materials/default.hlsl"};
static std::string gOutFile {"../../../samples/common/data/test/bla.cmat"};
static bool gSilentMode {false};

int convertMesh(const std::string& inFile, const std::string& outFile);

int main(int argc, char* argv[]) {
	// Get paths from command arguments (or hardcoded values for debugging)
	if (argc < 3) {
		//return -1;
		return MeshToolError::ARGS;
	} else {
		gInFile = argv[1];
		gOutFile = argv[2];
		if (argc == 4) {
			if (std::string{argv[3]} == "-s") {
				gSilentMode = true;
			}
		}
	}
	if (!gSilentMode) {
		//std::cout << "Converting " << argv[1] << "\nto " << argv[2] << '\n';
	}
	const int result {convertMesh(gInFile, gOutFile)};
	return result;
}

int convertMesh(const std::string& inFile, const std::string& outFile) {
	Importer importer;
	const aiScene* const scene = importer.ReadFile(inFile,
												   aiProcess_Triangulate);
	if (!scene) {
		if (!gSilentMode) {
			std::cout << "Couldn't load " << inFile << std::endl;
		}
		return MeshToolError::READ;
	}
	if (!gSilentMode) {
		std::cout << "Converting " << inFile << std::endl;
	}

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
	// Note: Switch Y<->Z and winding order
	//		 because of the way Blender exports .objs.
	fwrite(&mesh.mNumVertices, sizeof(unsigned), 1, file);
	for (unsigned j = 0; j < mesh.mNumVertices; j++) {
		const float v[3] = { mesh.mVertices[j].x,
							 mesh.mVertices[j].z,
							 mesh.mVertices[j].y };
		fwrite(v, sizeof(float), 3, file);
		const float n[3] = { mesh.mNormals[j].x,
							 mesh.mNormals[j].z,
							 mesh.mNormals[j].y };
		fwrite(n, sizeof(float), 3, file);
	}
	const unsigned numIndices = mesh.mNumFaces * 3;
	fwrite(&numIndices, sizeof(unsigned), 1, file);
	for (unsigned j = 0 ; j < mesh.mNumFaces ; j++) {
		const unsigned ind[3] = {
			mesh.mFaces[j].mIndices[0],
			mesh.mFaces[j].mIndices[2],
			mesh.mFaces[j].mIndices[1] };
		fwrite(ind, sizeof(unsigned), 3, file);
		//assert(face.mNumIndices == 3);
	}
	fclose(file);
	return MeshToolError::SUCCESS;
}