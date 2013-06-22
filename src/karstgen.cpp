#include "config.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <tuple>
#include <avr/avr++.h>
#include <boost/program_options.hpp>

#include "util.h"
#include "context.h"

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

static float maxX = 0.0f;
static float minX = 0.0f;
static float maxY = 0.0f;
static float minY = 0.0f;
static float maxZ = 0.0f;
static float minZ = 0.0f;

void parse_options(int argc, char** argv)
{
	po::options_description desc("Available options");
	desc.add_options()
	    ("help", "Print this message")
	    ("format,f", po::value<string>(&outputFormatString)->default_value(string("obj")),
	  "Format of the file to be create (avr or obj)")
	    ("output,o", po::value<string>(&outputFile),
	  "Name of the file to which the mesh will be saved")
	    ("input,i", po::value<string>(&inputFile)->default_value(string("-")),
	  "Input file with blob data. First line should contain number of"
	  "blobs that will follow. Each blob data is located on separate line."
	  "Single blob data consists of four float values, its location as x,y,z"
	  "coordinates and diameter.\n\n"
	  "Example:\n"
	  "3\n"
	  "0.0 0.0 0.0 0.5\n"
	  "1.0 1.0 1.0 0.7\n"
	  "-1.0 2.0 0.9 0.3\n\n"
	  "If not specified, will be read from standard input")
	;
	
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

tuple<unique_ptr<float4[]>, int> read_input(istream& is)
{
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

void find_boundaries(float4* blobs, int nBlobs)
{
	maxX = blobs[0].x + blobs[0].w;
	minX = blobs[0].x - blobs[0].w;
	
	maxY = blobs[0].y + blobs[0].w;
	minY = blobs[0].y - blobs[0].w;
	
	maxZ = blobs[0].z + blobs[0].w;
	minZ = blobs[0].z - blobs[0].w;
	
	for(int i{1}; i<nBlobs; i++) {
		maxX = std::max(maxX, blobs[i].x + blobs[i].w);
		minX = std::min(minX, blobs[i].x - blobs[i].w);
		
		maxY = std::max(maxY, blobs[i].y + blobs[i].w);
		minY = std::min(minY, blobs[i].y - blobs[i].w);
		
		maxZ = std::max(maxZ, blobs[i].z + blobs[i].w);
		minZ = std::min(minZ, blobs[i].z - blobs[i].w);
	}
}

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
		
		find_boundaries(blobs.get(), nBlobs);
		
		Context ctx;
		
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
	} catch (...) {
		cerr << "Unknown error\n";
		return 255;
	}
	return 0;
}
