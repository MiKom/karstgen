/**
  \mainpage
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
  
  
  General input structure:
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
#include<glm/glm.hpp>

#include"fracturenet.h"
#include"common/mathtypes.h"

namespace po = boost::program_options;
using namespace std;

//Constant parameters
static const int BLOCK_LOG_SIZE = 5;
static const float BLOB_SPACING_COEFF =0.75f;
static const float FILLER_BLOB_DIAM_COEFF = 2.6f;


//Variables for parameters
static string outputFile = "-";
static string inputFile = "-";
static int randomSeed = 1;
static unsigned int posDeviationPercent = 0;
static unsigned int sizeDeviationPercent = 0;

/**
 * \brief How many blocks for Marching cubes are there gonna be on X axis
 */
static int g_blocksOnX = 10;

static FractureNet fractureNet; //!< Storage for input data

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
	                 "proportional.")
	                ("positionDeviationPeorcent,p", po::value<unsigned int>(&posDeviationPercent)->default_value(0),
	                 "Maximum percentage of blobs size that the blob's position may "
	                 "be randomly deviated on each axis. E.g. blob with 1.0m "
	                 "diameter and this value set to 10 may be deviated 10cm on "
	                 "each axis. Deviation has uniform distribution within its bounds.")
	                ("sizeDeviationPercent,s", po::value<unsigned int>(&sizeDeviationPercent)->default_value(0),
	                 "Maximum percentage of original blob size that the blob's diameter "
	                 "may be randomly deviated. E.g. blob with 1.0m diameter and this "
	                 "value set to 10 may end up with size between 0.9m and 1.1m. "
	                 "Deviation has uniform distribution within its bounds.")
	                ("seed", po::value<int>(&randomSeed)->default_value(1),
	                 "Random seed used for deviating sizes and positions of blobs");
	
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
		is >> tmp;
		ret.xData.push_back(tmp);
	}
	for(int i=0; i<nY; i++){
		is >> tmp;
		ret.yData.push_back(tmp);
	}
	for(int i=0; i<nZ; i++){
		is >> tmp;
		ret.zData.push_back(tmp);
	}
	return ret;
}

/**
 * @brief Read data from input and save for processing
 *
 * Data is read from provided input stream and saved to \ref fractureNet global
 * variable for processing.
 *
 * @param is inputstream to read from
 *
 */
void
readInput(istream& is)
{

	is >> fractureNet.x >> fractureNet.y >> fractureNet.z;
	is >> fractureNet.xLen >> fractureNet.yLen >> fractureNet.zLen;
	int nDataPoints;
	is >> nDataPoints;
	
	for(int i=0; i<nDataPoints; i++){
		DataPoint&& dp = readDataPoint(is);
		auto dpPos = make_tuple(dp.x, dp.y, dp.z);
		if(fractureNet.dataPoints.find(dpPos) != fractureNet.dataPoints.end()) {
			ostringstream ss;
			ss << "Malformed inptut, more than one datapoint in position ("
			   << dp.x <<", " << dp.y << ", " << dp.z <<").";
			throw runtime_error(ss.str());
		}
		
		if(dp.x > fractureNet.x || dp.y > fractureNet.y || dp.z > fractureNet.z ) {
			ostringstream ss;
			ss << "Malformed inptut, data point at ("
			   << dp.x <<", " << dp.y << ", " << dp.z <<") beyond fracture "
			      "net domain";
			throw runtime_error(ss.str());
		}
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
 * @brief extract blobs from 1D diameter vector.
 *
 * This function takes vector of diameters along single axis and returns
 * list of blobs for this axis. Returned blobs are represented by two values
 * i.e. position on the vector (in meters) and diameter. Positions of returned
 * blobs are kept in x component and diameters in y component.
 *
 * @param firstPointDiam diameter of midpoint of dataPoint
 * @param data vector with diameter values
 * @param nextDpMidDiam diameter of midpoint of next DataPoint on this vector.
 *        if it doesn't exis, pass 0.0f.
 * @param vectorLen Length of this vector in meters
 * @return vector of 2D vectors that have position of the blob in x component
 *         and diameter in y componen. Positions are within (0.0 ... vectorLen)
 *
 */
vector<glm::vec2>
blobsOnVector(float firstPointDiam, const vector<float>& data, float nextDpMidDiam, float vectorLen)
{
	vector<glm::vec2> ret;
	
	if(data.empty()) {
		return ret;
	}
	
	float pos = firstPointDiam * BLOB_SPACING_COEFF;
	float lastBlobBorder = firstPointDiam / 2.0f;
	float limit = vectorLen - (nextDpMidDiam / 2.0f);
	int nPoints = data.size() + 1;
	float segmentLen = vectorLen / nPoints;
	while (pos <= limit) {
		//Calculate between which two data points will this blob fall
		int segmentStartIdx = std::floor(pos / vectorLen * nPoints);
		
		float frac = std::fmod(pos, segmentLen) / segmentLen;
		float diam = 0.0f;
		
		if(segmentStartIdx == 0) { //pos in first segment
			diam = glm::mix(firstPointDiam, data[0], frac);
		} else if(segmentStartIdx == nPoints - 1) { //pos in last segment
			diam = glm::mix(data[data.size() - 1], nextDpMidDiam, frac);
		} else {
			diam = glm::mix(data[segmentStartIdx - 1], data[segmentStartIdx], frac);
		}
		
		//If diameter of calculated blob is too small, makes sure, that
		//pos is moved by some moderate value on each step.
		if(diam > 0.1f) {
			//If next blob is too small to connect with previous one
			//move back to a point where it would potentially
			//connect
			if(pos - (diam / 2.0f) > lastBlobBorder) {
				pos -= pos - (diam/2.0) - lastBlobBorder;
				continue;
			}
			ret.push_back(glm::vec2{pos, diam});
			lastBlobBorder = pos + diam/2.0f;
			
			//If this is the last blob to be added on this vector,
			//check if space to the next blob is filled well.
			//If not, place another blob that will hopefully fill it
			if(pos + diam * BLOB_SPACING_COEFF > limit) {
				if(pos + diam / 2.0 < limit) {
					float fillerDiam = FILLER_BLOB_DIAM_COEFF * (limit - (pos + diam / 2.0f));
					float fillerPos = pos+diam/2.0f + fillerDiam / 2.0f;
					ret.push_back(glm::vec2{fillerPos, fillerDiam / 2.0f});
					break;
				}
			}
			pos += diam * BLOB_SPACING_COEFF;
		} else {
			pos += 0.1f;
		}
	}
	return ret;
}

/**
 * @brief Calculate blobs for one datapoint
 *
 * This function will calculate list of blobs for given data point. Note however
 * that output will have intersection of axes attached at origin (0,0,0). Axe
 * X goes in positive direction, Y in negative (considered downwards) and Z in
 * positive (considered away from the viewer).
 *
 * @param dp data point to extract blobs from
 * @param fn fracture net map to get neighbouring datapoints (if exist)
 * @return vector with blobs generated from this data point.
 */
vector<glm::vec4>
blobsFromDataPoint(const DataPoint& dp, const FractureNet& fn)
{
	vector<glm::vec4> ret;
	ret.push_back(glm::vec4{0.0f, 0.0f, 0.0f, dp.midDiam});
	//Adding blobs along X axis
	float nextXDiam = 0.0f;
	auto itX = fn.dataPoints.find(make_tuple(dp.x+1, dp.y, dp.z));
	if(itX != fn.dataPoints.end()) {
		nextXDiam = itX->second.midDiam;
	}
	vector<glm::vec2> xBlobs = blobsOnVector(dp.midDiam, dp.xData, nextXDiam, fn.xLen);
	for(auto& blob : xBlobs) {
		ret.push_back(glm::vec4{blob.x, 0.0f, 0.0f, blob.y});
	}
	
	// Adding blobs along Y axis
	float nextYDiam = 0.0f;
	auto itY = fn.dataPoints.find(make_tuple(dp.x, dp.y+1, dp.z));
	if(itY != fn.dataPoints.end()) {
		nextYDiam = itY->second.midDiam;
	}
	vector<glm::vec2> yBlobs = blobsOnVector(dp.midDiam, dp.yData, nextYDiam, fn.yLen);
	for(auto& blob : yBlobs) {
		ret.push_back(glm::vec4{0.0f, -1.0f * blob.x, 0.0f, blob.y});
	}
	
	//Adding blobs along Z axis
	float nextZDiam = 0.0f;
	auto itZ = fn.dataPoints.find(make_tuple(dp.x, dp.y, dp.z+1));
	if(itZ != fn.dataPoints.end()) {
		nextZDiam = itZ->second.midDiam;
	}
	vector<glm::vec2> zBlobs = blobsOnVector(dp.midDiam, dp.zData, nextZDiam, fn.zLen);
	for(auto& blob : zBlobs) {
		ret.push_back(glm::vec4(0.0f, 0.0f, blob.x, blob.y));
	}
	return ret;
}

/**
 * @brief blobber main algorithm
 */
void blobber(ostream& os)
{
	//Initialize RNG engine
	std::minstd_rand rng(randomSeed);
	
	float posRandCoeff = static_cast<float>(std::min((uint) 100, posDeviationPercent)) / 100.0f;
	std::uniform_real_distribution<float> posDis(-posRandCoeff, posRandCoeff);
	
	float sizeRandCoeff = static_cast<float>(std::min((uint) 100, sizeDeviationPercent)) / 100.0f;
	std::uniform_real_distribution<float> sizeDis(-sizeRandCoeff, sizeRandCoeff);
	
	//lengths on X and Z are enlarge so blobs on boundaries are wholly within the are
	//where marching cubes will work
	float xLen = fractureNet.dimensionLength(Dimension::DIM_X) + 2 * fractureNet.xLen;
	float yLen = fractureNet.dimensionLength(Dimension::DIM_Y);
	float zLen = fractureNet.dimensionLength(Dimension::DIM_Z) + 2 * fractureNet.zLen;
	
	//Length of a single block is derieved from length of a block on X dimension.
	float blockLen = xLen / g_blocksOnX;
	
	//On Y and Z dimensions, number of blocks is proportional to number of bloks on X
	int blocksOnX = g_blocksOnX;
	int blocksOnY = static_cast<int>(std::ceil(yLen / blockLen));
	int blocksOnZ = static_cast<int>(std::ceil(zLen / blockLen));
	
	//Real size of the realm on which Marching cubes will work.
	float xMcLen = blocksOnX * blockLen;
	float yMcLen = blocksOnY * blockLen;
	float zMcLen = blocksOnZ * blockLen;
	
	//print starting point. Bottom of the structure will be on y = 0.0
	os << -xMcLen / 2.0f << " " << 0.0f << " " << -zMcLen / 2.0f << "\n";
	
	//Print number of blocks on each axis
	os << blocksOnX << " " << blocksOnY << " " << blocksOnZ << "\n";
	
	//Print size of block on each axis
	os << blockLen << " " << blockLen << " " << blockLen << "\n";
	os << BLOCK_LOG_SIZE << "\n";
	
	glm::vec3 startPoint{-xLen / 2.0f, yLen, -zLen / 2.0f};
	vector<glm::vec4> blobs;
	
	for(auto& elem: fractureNet.dataPoints) {
		DataPoint& dp = elem.second;
		glm::vec4 offset {
			fractureNet.xLen * dp.x - fractureNet.dimensionLength(Dimension::DIM_X) / 2.0f,
			fractureNet.dimensionLength(Dimension::DIM_Y) - fractureNet.yLen * dp.y,
			fractureNet.zLen * dp.z - fractureNet.dimensionLength(Dimension::DIM_Z) / 2.0f,
			0.0f
		};
		
		vector<glm::vec4> dpBlobs = blobsFromDataPoint(dp, fractureNet);
		for(auto& blob : dpBlobs) {
			blobs.push_back(blob + offset);
		}
		
	}
	
	os << blobs.size() << "\n";
	for(auto& blob : blobs) {
		os << blob.x + posDis(rng) * blob.w << " "
		   << blob.y + posDis(rng) * blob.w << " "
		   << blob.z + posDis(rng) * blob.w << " "
		   << blob.w + sizeDis(rng) * blob.w << "\n";
	}
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
