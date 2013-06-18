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

string outputFormat;
string outputFile;
string inputFile;

void parse_options(int argc, char** argv)
{
	po::options_description desc("Available options");
	desc.add_options()
	    ("help", "Print this message")
	    ("format,f", po::value<string>()->default_value(string("obj")),
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
		cerr << "No output file specified, aborting\n";
		exit(1);
	}
}

/**
  \todo Error checking
*/
tuple<unique_ptr<float4[]>, int> read_input(istream& is)
{
	int nBlobs;
	is >> nBlobs;
	
	float4* blobs = new float4[nBlobs];
	for(int i{0}; i<nBlobs; i++) {
		is >> blobs[i].x >> blobs[i].y >> blobs[i].z >> blobs[i].w;
	}
	return make_tuple(unique_ptr<float4[]>(blobs), nBlobs);
}

int main(int argc, char** argv)
{
	try {
		parse_options(argc, argv);
		Context ctx;
		
		unique_ptr<float4[]> blobs;
		int nBlobs;
		if(inputFile == "-") {
			tie(blobs, nBlobs) = read_input(cin);
		} else {
			ifstream inputStream(inputFile);
			tie(blobs, nBlobs) = read_input(inputStream);
		}
		
	} catch ( cl::Error &e ) {
		cerr
		  << "OpenCL runtime error at function " << endl
		  << e.what() << endl
		  << "Error code: "<< endl
		  << errorString(e.err()) << endl;
	} catch (runtime_error &e) {
		cerr << e.what();
		return 1;
	}
	return 0;
}
