/**
  \file 
  Blobber is a program that can take description of fracture net and produce
  set of blobs in form of input for karstgen program.
  
  Input to this program is structured as follows:
  
  On the first line there are tree integers \c x_size \c y_size and \c z_size.
  These three integers denote size of the fracture net in each dimension
  
  Second line contains three floating point numbers <tt>x_length y_length</tt>
  and \c z_length that denote lenghts of fractures
  in each data point in <tt>x, y</tt> and \c z directions respectively.
  
  Third line contains single integer \c num_entries that denotes number of
  of data entries. Single data entry describes diameters of fractures originating
  in one point and going in positive \c x, \c y and \c z directions.
  
  Next, data entries appear in the input.
  
  Each data entry is structured as follows:
  in the first line there are three integers \c x_pos, \c y_pos and \c z_pos
  that denote position of this entry in the fracture net (respective values 
  should not exceed \c x_size, \c y_size and \c z_size).
  In the second line, three integers \c x_num, \c y_num and \c z_num are present.
  These three values denote number of diameter data points in each axis.
  
  In the third line, there is a single float \c diam_mid that denotes diameter 
  of the sphere in the junction point of the three axes.
  
  After that, <tt>x_num*y_num*z_num</tt> floats appear. First there are \c x_num
  floats for \c x direction, then \c y_num floats for \c y direction and \c
  z_num floats for \c z direction.
  
  Set of diameters for given direction is assumed to be distributed uniformly 
  along the fracture.
  If set size for given direction is 0, than there is no fracture in this direction
  
  
  General output structure:
  \verbatim
  x_size y_size z_size
  x_lenght y_length z_length
  num_entries
  x_pos y_pos z_pos --------------+
  x_num y_num z_num               |
  diam_mid                        |
  diam_x_1 -----+                 |
  diam_x_2      |                 |
  .             +-x_num entries   |
  .             |                 |
  damx_x_x_num--+                 |
  diam_y_1 -----+                 +--- num_entries times
  diam_y_2      |                 |
  .             +-y_num entries   |
  .             |                 |
  damx_y_y_num--+                 |
  diam_z_1 -----+                 |
  diam_z_2      |                 |
  .             +-x_num entries   |
  .             |                 |
  damx_z_z_num--+-----------------+
  \endverbatim
  
  Example:
  \verbatim
  2 2 2
  5.0 5.0 5.0
  3
  0 0 0
  1 0 1
  0.5
  0.5
  0.5
  1 0 0
  0 1 0
  0.5
  0.5
  1 1 0
  0 1 0
  0.5
  0.5
  \endverbatim
  
  */
#include<iostream>
#include<fstream>
#include<string>
#include<tuple>
#include<cmath>
#include<boost/program_options.hpp>

#include"fracturenet.h"

namespace po = boost::program_options;
using namespace std;

//Constant parameters

//Variables for parameters
static string outputFile = "-";
static string inputFile = "-";

/**
 * @brief How many blocks for Marching cubes are thereg onna be on X axis
 */
static int g_blocksOnX = 10;

//Storage for input data
static FractureNet fractureNet;

void parse_options(int argc, char** argv)
{
	po::options_description desc("Available options");
	desc.add_options()
	               	("help,h", "Print this message")
	                ("output,o", po::value<string>(&outputFile),
	                 "Name of the file to which the output will be saved "
	                 "If not specified or spedcified as \"-\" output will "
	                 "be written to standard output")
	                ("input,i", po::value<string>(&inputFile),
	                 "Input file with fracture net data. "
	                 "If not specified or spedcified as \"-\" output will "
	                 "be written to standard output")
	                ("blocksOnX,b", po::value<int>(&g_blocksOnX)->default_value(10),
	                 "How many blocks for marching cubes algorithm will be "
	                 "present along X axis. Number on other axes will be "
	                 "proportional.");
	
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	
	if(vm.count("help")) {
		cerr << desc << "\n";
		exit(1);
	}
}

DataPoint
readDataPoint(istream& is)
{
	DataPoint ret;
	
	is >> ret.x >> ret.y >> ret.z;
	int nX, nY, nZ;
	is >> nX >> nY >> nZ;
	
	ret.xData.reserve(nX);
	ret.yData.reserve(nY);
	ret.zData.reserve(nZ);
	 
	is >> ret.midDiam;
	
	float tmp;
	for(int i=0; i<nX; i++){
		is >> ret.xData[i];
	}
	for(int i=0; i<nY; i++){
		is >> ret.yData[i];
	}
	for(int i=0; i<nZ; i++){
		is >> ret.zData[i];
	}
	return ret;
}

void
readInput(istream& is)
{

	is >> fractureNet.x >> fractureNet.y >> fractureNet.z;
	is >> fractureNet.xLen >> fractureNet.yLen >> fractureNet.zLen;
	int nDataPoints;
	is >> nDataPoints;
	
	for(int i=0; i<nDataPoints; i++){
		DataPoint&& dp = readDataPoint(is);
		fractureNet.dataPoints[make_tuple(dp.x, dp.y, dp.z)] = dp;
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
}

/**
 * @brief blobber main algorithm
 */
void blobber(ostream& os)
{
	float xBlockLen = fractureNet.dimensionLength(Dimension::DIM_X) / g_blocksOnX;
	
	float xLen = fractureNet.dimensionLength(Dimension::DIM_X);
	float yLen = fractureNet.dimensionLength(Dimension::DIM_Y);
	float zLen = fractureNet.dimensionLength(Dimension::DIM_Z);
	
	int blocksOnX = g_blocksOnX;
	int blocksOnY = static_cast<int>(std::ceil(yLen / xBlockLen));
	int blocksOnZ = static_cast<int>(std::ceil(zLen / xBlockLen));
	
	//TODO: finish blobber algorithm
	
}

int main(int argc, char** argv)
{
	try {
		parse_options(argc, argv);
		
		if(inputFile == "-") {
			readInput(cin);
		} else {
			ifstream inputStream(inputFile);
			readInput(inputStream);
		}
		
		if(outputFile == "-") {
			blobber(cout);
		} else {
			ofstream outputStream(outputFile);
			blobber(outputStream);
		}
		
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
