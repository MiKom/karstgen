#include "config.h"

#include "marchingcubes.h"
#include "exporters.h"

#include <vector>
#include <avr/avr++.h>

using namespace AVR;
using namespace std;

void export_avr(vector<MCMesh*> meshes, std::string fileName)
{
	AVRFile file;
	
	AVRMaterial material;
	material.setName("grey");
	material.setDiffuse(0.5f, 0.5f, 0.5f);
	material.setSpecular(0.0f, 0.0f, 0.0f);
	material.setAmbient(0.0f, 0.0f, 0.0f);
	material.setEmissive(0.5f, 0.5f, 0.5f);
	material.setOpacity(1.0f);
	material.setSpecularLevel(0.0f);
	material.setGlossiness(0.0f);
	file.addMaterial(material);
	
	//Using AVR library for multiple meshes is fishy, needs to be fixed.
	vector<AVRMesh*> avrMeshes;
	for(MCMesh* mcmesh : meshes) {
		AVRMesh *mesh = new AVRMesh();
		avrMeshes.push_back(mesh);
		mesh->setName("karst");
		mesh->addTextCoord(AVR::vec2{0.0f, 0.0f});
		
		for(int i{0}; i<mcmesh->verts.size(); i++) {
			float3 normal{mcmesh->normals[i]};
			mesh->addNormal(AVR::vec3{normal.x, normal.y, normal.z});
			
			float3 vert{mcmesh->verts[i]};
			mesh->addVertex(AVR::vec3{vert.x, vert.y, vert.z});
		}
		for(uint i{0}; i<mcmesh->verts.size(); i+=3) {
			mesh->addFace(AVRFace{{i, i+1, i+2}, {i, i+1, i+2}, {0, 0, 0}});
		}
		mesh->setMaterialId(0);
		file.addMesh(mesh);
	}
	
	file.save(fileName);
	for(AVRMesh* mesh : avrMeshes) {
		delete mesh;
	}
	
}
void export_wavefron_obj(vector<MCMesh*> meshes, std::string file)
{
	
}
