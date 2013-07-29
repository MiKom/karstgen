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
	int x, y, z;
	float midDiam;
	std::vector<float> xData; //!< Diameters of points along the \c x axis
	std::vector<float> yData; //!< Diameters of points along the \c y axis
	std::vector<float> zData; //!< Diameters of points along the \c z axis
};

struct FractureNet
{
	int x, y, z;
	std::map<std::tuple<int, int, int>, DataPoint> dataPoints;
};
