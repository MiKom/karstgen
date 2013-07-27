/**
  \file 
  Blobber is a program that can take description of fracture net and produce
  set of blobs in form of input for karstgen program.
  
  Input to this program is structured as follows:
  
  On the first line there are tree integers \c x_size \c y_size and \c z_size.
  These three integers denote size of the fracture net in each dimension
  
  Second line contains single integer \c num_entries that denotes number of
  of data entries. Single data entry describes diameters of fractures originating
  in one point and going in positive \c x, \c y and \c z directions.
  
  Next, data entries appear in the input.
  
  Each data entry is structured as follows:
  in the first line there are three integers \c x_pos, \c y_pos and \c z_pos
  that denote position of this entry in the fracture net (respective values 
  should not exceed \c x_size, \c y_size and \c z_size).
  In the second line, three integers \c x_num, \c y_num and \c z_num are present.
  These three values denote number of diameter data points in each axis.
  
  After that, <tt>x_num*y_num*z_num</tt> floats appear. First there are \c x_num
  floats for \c x direction, then \c y_num floats for \c y direction and \c
  z_num floats for \c z direction.
  
  Set of diameters for given direction is assumed to be distributed uniformly 
  along the fracture.
  If set size for given direction is 0, than there is no fracture in this direction
  
  
  General output structure:
  \verbatim
  x_size y_size z_size
  num_entries
  x_pos y_pos z_pos --------------+
  x_num y_num z_num               |
  diam_x_1 -----+                 |
  diam_x_2      |                 |
  ...           +-x_num entries   |
  damx_x_x_num--+                 |
  diam_y_1 -----+                 +--- num_entries times
  diam_y_2      |                 |
  ...           +-y_num entries   |
  damx_y_y_num--+                 |
  diam_z_1 -----+                 |
  diam_z_2      |                 |
  ...           +-x_num entries   |
  damx_z_z_num--+-----------------|
  \endverbatim
  
  Example:
  \verbatim
  2 2 2
  3
  0 0 0
  1 0 1
  0.5
  0.5
  1 0 0
  0 1 0
  0.5
  1 1 0
  0 1 0
  0.5
  \endverbatim
  
  */
#include<iostream>
#include<string>
#include<boost/program_options.hpp>

namespace po = boost::program_options;
using namespace std;

//Variables for parameters
static string outputFile = "-";
static string inputFile = "-";

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
	                 "be written to standard output");
	
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	
	if(vm.count("help")) {
		cerr << desc << "\n";
		exit(1);
	}
}

int main(int argc, char** argv)
{
	parse_options(argc, argv);
	
	return 0;
}
