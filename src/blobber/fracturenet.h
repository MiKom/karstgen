#pragma once

#include <vector>

struct DataPoint
{
	int x, y, z;
	
	int nX;
	float xData[50]; //!< Diameters of points along the \c x axis
	
	int nY;
	float yData[50]; //!< Diameters of points along the \c y axis
	
	int nZ;
	float zData[50]; //!< Diameters of points along the \c z axis
};

struct FractureNet
{
	int x, y, z;
	std::vector<DataPoint> dataPoints;
};
