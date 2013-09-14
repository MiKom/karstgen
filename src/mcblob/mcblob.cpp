#include "config.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <tuple>
#include <avr/avr++.h>
#include <boost/program_options.hpp>
#include <signal.h>

#include "util.h"
#include "grid.h"
#include "context.h"
#include "marchingcubes.h"
#include "blob.h"
#include "exporters.h"

using namespace AVR;
using namespace std;

namespace po = boost::program_options;

enum class OutputFormat {
	OUTPUT_FORMAT_OBJ,
	OUTPUT_FORMAT_AVR
} outputFormat = OutputFormat::OUTPUT_FORMAT_OBJ;

string outputFormatString;
string outputFile;
string inputFile;
bool debug = false;

/**
 * @brief if true, stop calculations and dump results so far
 */
bool bailout = false;

//Input data
static float3 startPoint{0.0f, 0.0f, 0.0f};
static float3 blockSize{1.0f, 1.0f, 1.0f};
static unsigned int logBlockDim{5}; //32 x 32 x 32
static uint3 gridConf{1, 1, 1};

//static float maxX = 0.0f;
//static float minX = 0.0f;
//static float maxY = 0.0f;
//static float minY = 0.0f;
//static float maxZ = 0.0f;
//static float minZ = 0.0f;
//
//static float minDiam = 0.0f;

void parse_options(int argc, char** argv)
{
	po::options_description desc("Available options");
	desc.add_options()
	    ("help,h", "Print this message")
	    ("debug,d", po::value(&debug)->zero_tokens(),
	  "Print debug messages to stderr")
	    ("format,f", po::value<string>(&outputFormatString)->default_value(string("obj")),
	  "Format of the file to be create (avr or obj)")
	    ("output,o", po::value<string>(&outputFile),
	  "Name of the file to which the mesh will be saved")
	    ("input,i", po::value<string>(&inputFile)->default_value(string("-")),
	  "Input file with blob data and Marching cubes parameters\n"
	  "If not specified, or speciefied as \"-\" will be read from standard input\n"
	  "  First line should contain the starting point of the grid of blocks."
	  "It should be expressed as three floating point numbers. This starting "
	  "point will be a corner of the structure (here marked as 1):\n"
	  "         +-------------+\n"
	  "        /|            /|\n"
	  "       / |           / |\n"
	  "      +-------------+  |\n"
	  "      |  |          |  |\n"
	  "      |  |          |  |\n"
	  "y  z  |  +----------|--+\n"
	  "| /   | /           | / \n"
	  "|/    |/            |/  \n"
	  "+--x  +-------------+   \n"
	  "     1                  \n\n"
	  "  Second line should contain three integer numbers. This will be the size "
	  "of the grid of blocks i.e. how many blocks are on each axis.\n"
	  "  Third line should contain size of each block. Also expressed as three "
	  "floating point numbers.\n"
	  "  Fourth line should contain one integer n where which is log2 of size "
	  "(in voxels) of each axis of each block. For example, if you want each "
	  "block to be 256x256x256, n should be 8\n"
	  "  Fifth line should be a single integer denoting number of blob "
	  "descriptions that will follow. Each blob description is located on "
	  "separate line. Single blob data consists of four float values, its "
	  "location as x,y,z coordinates and diameter.\n\n"
	  "Example:\n"
	  "-1.0 0.0 2.0\n"
	  "4 4 4\n"
	  "10.0 10.0 10.0\n"
	  "5\n"
	  "3\n"
	  "0.0 0.0 0.0 0.5\n"
	  "1.0 1.0 1.0 0.7\n"
	  "-1.0 2.0 0.9 0.3\n\n"
	  "In the above example, structure starts at coordinates (-1,0,2), it's "
	  "made of 64 (4x4x4) blocks, each of which is 10x10x10 units in size. "
	  "Each block will be divided into 32x32x32 uniform voxels (because 2^5=32).\n"
	  "There will be 3 blobs rendered with following parameters:\n"
	  "  No.  |  x  |  y  |  z  | diameter \n"
	  "-------+-----+-----+-----+----------\n"
	  "   1   | 0.0 | 0.0 | 0.0 |    0.5   \n"
	  "   2   | 1.0 | 1.0 | 1.0 |    0.7   \n"
	  "   3   |-1.0 | 2.0 | 0.9 |    0.3   "
	);
	
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	
	if(vm.count("help")) {
		cerr << desc << "\n";
		exit(1);
	}
	if(!vm.count("output")) {
		throw runtime_error("No output file specified");
		exit(1);
	}
	
	if(outputFormatString == "avr") {
		outputFormat = OutputFormat::OUTPUT_FORMAT_AVR;
	} else if(outputFormatString == "obj") {
		//already set as default
	} else {
		throw runtime_error("Unsupported file format");
		exit(1);
	}
}

tuple<unique_ptr<float4[]>, int>
read_input(istream& is)
{
	is >> startPoint.x >> startPoint.y >> startPoint.z;
	is >> gridConf.x >> gridConf.y >> gridConf.z;
	is >> blockSize.x >> blockSize.y >> blockSize.z;
	is >> logBlockDim;
	
	int nBlobs;
	is >> nBlobs;
	
	float4* blobs = new float4[nBlobs];
	for(int i{0}; i<nBlobs; i++) {
		is >> blobs[i].x >> blobs[i].y >> blobs[i].z >> blobs[i].w;
	}
	if(is.eof()) {
		throw runtime_error("EOF in input reached prematurely");
	}
	if(is.bad()) {
		throw runtime_error("Input malformed");
	}
	if(is.fail()) {
		throw runtime_error("Input IO error");
	}
	return make_tuple(unique_ptr<float4[]>(blobs), nBlobs);
}

///**
//  @brief find boundaries in which the domain should be enclosed
//  
//  Function iterates over all blobs and find maximal and minimal value on each
//  axis of (center + diameter) the blob. Results are put in global variables.
// */
//void find_boundaries_and_smallest_blob(float4* blobs, int nBlobs)
//{
//	maxX = blobs[0].x + blobs[0].w;
//	minX = blobs[0].x - blobs[0].w;
//	
//	maxY = blobs[0].y + blobs[0].w;
//	minY = blobs[0].y - blobs[0].w;
//	
//	maxZ = blobs[0].z + blobs[0].w;
//	minZ = blobs[0].z - blobs[0].w;
//	
//	minDiam = blobs[0].w;
//	
//	for(int i{1}; i<nBlobs; i++) {
//		maxX = std::max(maxX, blobs[i].x + blobs[i].w);
//		minX = std::min(minX, blobs[i].x - blobs[i].w);
//		
//		maxY = std::max(maxY, blobs[i].y + blobs[i].w);
//		minY = std::min(minY, blobs[i].y - blobs[i].w);
//		
//		maxZ = std::max(maxZ, blobs[i].z + blobs[i].w);
//		minZ = std::min(minZ, blobs[i].z - blobs[i].w);
//		
//		minDiam = std::min(minDiam, blobs[i].w);
//	}
//}

void usr1_handler(int signal)
{
	bailout = true;
}

struct sigaction usr1_action = {
	usr1_handler, 0, 0, 0, 0
};

int main(int argc, char** argv)
{
	try {
		parse_options(argc, argv);
		
		unique_ptr<float4[]> blobs;
		int nBlobs;
		if(inputFile == "-") {
			tie(blobs, nBlobs) = read_input(cin);
		} else {
			ifstream inputStream(inputFile);
			tie(blobs, nBlobs) = read_input(inputStream);
		}
		
		//find_boundaries_and_smallest_blob(blobs.get(), nBlobs);
		
		Context ctx;
		
		sigaction(SIGUSR1, &usr1_action, NULL);
		
		//Main algorithm
		
		vector<MCMesh> meshes;
		
		uint3 gridDim = uint3(static_cast<uint>(1) << logBlockDim);
		
		float3 voxelSize{
			blockSize.x / gridDim.x,
			blockSize.y / gridDim.y,
			blockSize.z / gridDim.z
		};
		if(debug) {
			cerr << "Processed blocks 0/"<< gridConf.x * gridConf.y * gridConf.z;
		}
		int generatedVertices = 0;
		for(int i=1; i<gridConf.x; i++) {
			for(int j=0; j<gridConf.y; j++){
				for(int k=0; k<gridConf.z; k++){
					float3 blockStart {
						startPoint.x + blockSize.x * i,
						startPoint.y + blockSize.y * j,
						startPoint.z + blockSize.z * k,
						1.0f
					};
					Grid grid{
						gridDim,
						voxelSize,
						blockStart,
						ctx.getClContext(),
						ctx.getQueues()[0],
						ctx.getMemsetKernel()
					};
					grid.clear();
					ctx.getBlobProgram()->runBlob(blobs.get(), nBlobs, grid);
					MarchingCubes* mc = ctx.getMcProgram();
					meshes.push_back(mc->compute(grid, 1.0f));
					if(debug) {
						generatedVertices += meshes.at(meshes.size() - 1).verts.size();
						cerr << '\r' << "Processed blocks "
						     << i*(gridConf.z*gridConf.y) + j*gridConf.z + k
						     << "/"
						     << gridConf.x * gridConf.y * gridConf.z << " "
						     << "Vertices generated " << generatedVertices;
					}
					if(bailout) goto after_computation;
				}
			}
		}
after_computation:
		if(debug) {
			cout<< "\n";
		}
		
		switch(outputFormat) {
		case OutputFormat::OUTPUT_FORMAT_AVR:
			export_avr(meshes, outputFile);
			break;
		case OutputFormat::OUTPUT_FORMAT_OBJ:
		default:
			export_wavefront_obj(meshes, outputFile);
		}
		
	} catch ( cl::Error &e ) {
		cerr
		  << "OpenCL runtime error at function " << endl
		  << e.what() << endl
		  << "Error code: "<< endl
		  << errorString(e.err()) << endl;
		return 1;
	} catch (runtime_error &e) {
		cerr <<"Runtime error: " <<  e.what() << "\n";
		return 1;
	} catch (po::error& e) {
		cerr << "Error parsing options: " << e.what() << "\n";
		cerr << "See " << argv[0] << " --help\n";
	} catch (...) {
		cerr << "Unknown error\n";
		return 255;
	}
	return 0;
}
