#pragma once

#include <vector>
#include <tuple>

/**
 * @brief The DataPoint struct with diameters of fractures in each direction
 *
 * This structure contains data with diameters of fractures in each
 * direction (positive X, negative Y and positive Z).
 */
struct DataPoint
{
	int x, y, z; //!< position of the datapoint in the fracture net
	float midDiam; //!< diameter of the junction of fractures of this datapoint
	std::vector<float> xData; //!< Diameters of points along the \c x axis
	std::vector<float> yData; //!< Diameters of points along the \c y axis
	std::vector<float> zData; //!< Diameters of points along the \c z axis
};

enum class Dimension {
	DIM_X,
	DIM_Y,
	DIM_Z
};

struct FractureNet
{
	int x; //!< Fracture network size in x direction
	int y; //!< Fracture network size in y direction (downwards)
	int z; //!< Fracture network size in z direction
	float xLen; //!< Length of a single fracture in datapoint in x direction (meters)
	float yLen; //!< Length of a single fracture in datapoint in y direction (meters)
	float zLen; //!< Length of a single fracture in datapoint in z direction (meters)
	std::map<std::tuple<int, int, int>, DataPoint> dataPoints;
	
	/**
	 * @brief length of dimension as \c numFractures*fractureLen
	 * @param dim dimension for which length will be returned
	 * @return length of dimension in meters
	 */
	float dimensionLength(Dimension dim) const {
		switch(dim) {
		case  Dimension::DIM_X:
			return x * xLen;
			break;
		case Dimension::DIM_Y:
			return y * xLen;
			break;
		case Dimension::DIM_Z:
			return z * zLen;
		}
	}
};
