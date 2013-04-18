#include "config.h"
#include "context.h"
#include "grid.h"
#include "blob.h"
#include "marchingcubes.h"
#include "exporters.h"
#include "util.h"

#include <iostream>
#include <CL/cl.hpp>
#include <avr/avr++.h>

using namespace std;
using namespace AVR;

int main()
{
	try {
		Context ctx{true};
		cl::CommandQueue q = ctx.getQueues()[0];
		Grid grid(
			uint3(64,64,64),
			float3(15.0f/64.0f, 15.0f / 64.0f, 15.0f / 64.0f),
			float3(-2.5f, -2.5f, -2.5f),
			ctx.getClContext(),
			ctx.getQueues()[0]
		);
		
		float3 blobs[] = { {0.0f, 0.0f, 0.0f, 1.94f}, {0.0f, 0.5f, 2.0f, 2.0f} };
		
		Blob* blobProgram = ctx.getBlobProgram();
	
		blobProgram->runBlob(blobs, sizeof(blobs) / sizeof(blobs[0]), grid);
		
		
		MarchingCubes* mc = ctx.getMcProgram();
		MCMesh mcmesh = mc->compute(grid, 3.0f);
		
		vector<MCMesh*> meshes = {&mcmesh};
		export_avr(meshes, "out.avr");
		
		/*
		AVRMesh mesh;
		mesh.setName("bloba");
		mesh.addTextCoord(AVR::vec2{0.0f, 0.0f});
		for(uint i{0}; i<mcmesh.verts.size(); i++) {
			float3 normal{mcmesh.normals[i]};
			mesh.addNormal(AVR::vec3{normal.x, normal.y, normal.z});
			
			float3 vert{mcmesh.verts[i]};
			mesh.addVertex(AVR::vec3{vert.x, vert.y, vert.z});
		}
		for(uint i{0}; i<mcmesh.verts.size(); i+=3) {
			mesh.addFace(AVRFace{{i, i+1, i+2}, {i, i+1, i+2}, {0, 0, 0}});
		}
		
		AVRMaterial material;
		material.setName("red");
		material.setDiffuse(0.5f, 0.5f, 0.5f);
		material.setSpecular(0.0f, 0.0f, 0.0f);
		material.setAmbient(0.0f, 0.0f, 0.0f);
		material.setEmissive(0.5f, 0.5f, 0.5f);
		material.setOpacity(1.0f);
		material.setSpecularLevel(0.4f);
		material.setGlossiness(0.0f);
		
		mesh.setMaterialId(0);
		
		AVRFile file;
		file.addMaterial(material);
		file.addMesh(&mesh);
		
		file.save("out.avr");
		*/
		
	} catch ( cl::Error &e ) {
		cerr
		  << "OpenCL runtime error at function " << endl
		  << e.what() << endl
		  << "Error code: "<< endl
		  << errorString(e.err()) << endl;
		return 1;
	}
	
	return 0;
}
