#include "config.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <avr/avr++.h>

#include "util.h"
#include "context.h"

using namespace AVR;
using namespace std;

int main()
{
	try {
		Context ctx;
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
