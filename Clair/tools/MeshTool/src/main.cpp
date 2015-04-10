#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>
#include "../../../Clair/include/clair/vertexLayout.h"
#include "../../../Clair/src/serialization.h"
#include "ErrorCodes.h"
#include "../../common/CommandLineUtils.h"

using namespace Assimp;

static std::string gInFile {"../../../samples/common/rawdata/materials/default.hlsl"};
static std::string gOutFile {"../../../samples/common/data/test/bla.cmat"};
static bool gSilentMode {false};
static bool gBlenderObjMode {false};

int convertMesh(const std::string& inFile, const std::string& outFile);

int main(int argc, char* argv[]) {
	if (argc < 3) {
		return MeshToolError::ARGS;
	}
	gInFile = argv[1];
	gOutFile = argv[2];
	const auto commands = CommandLineUtils::getCommands(argc - 2, argv + 2);
	for (const char c : commands) {
		if		(c == 's') gSilentMode = true;
		else if (c == 'b') gBlenderObjMode = true;
	}

	if (!gSilentMode) {
		std::cout << "Converting " << argv[1] << "\nto " << argv[2] << '\n';
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
	fwrite(&mesh.mNumVertices, sizeof(unsigned), 1, file);
	for (unsigned j = 0; j < mesh.mNumVertices; j++) {
		float v[3] = { mesh.mVertices[j].x,
					   mesh.mVertices[j].y,
					   mesh.mVertices[j].z };
		if (gBlenderObjMode) v[2] = -v[2];
		fwrite(v, sizeof(float), 3, file);
		float n[3] = { mesh.mNormals[j].x,
					   mesh.mNormals[j].y,
					   mesh.mNormals[j].z };
		if (gBlenderObjMode) n[2] = -n[2];
		fwrite(n, sizeof(float), 3, file);
	}
	const unsigned numIndices = mesh.mNumFaces * 3;
	fwrite(&numIndices, sizeof(unsigned), 1, file);
	for (unsigned j = 0 ; j < mesh.mNumFaces ; j++) {
		if (gBlenderObjMode) {
			const unsigned ind[3] = {
				mesh.mFaces[j].mIndices[0],
				mesh.mFaces[j].mIndices[2],
				mesh.mFaces[j].mIndices[1] };
			fwrite(ind, sizeof(unsigned), 3, file);
		} else {
			const unsigned ind[3] = {
				mesh.mFaces[j].mIndices[0],
				mesh.mFaces[j].mIndices[1],
				mesh.mFaces[j].mIndices[2] };
			fwrite(ind, sizeof(unsigned), 3, file);
		}
		//assert(face.mNumIndices == 3);
	}
	fclose(file);
	return MeshToolError::SUCCESS;
}