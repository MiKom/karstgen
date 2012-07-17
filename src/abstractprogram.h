#ifndef __KARSTGEN__ABSTRACTPROGRAM_H__
#define __KARSTGEN__ABSTRACTPROGRAM_H__

/**
  This is base class for all operations with OpenCL. Every operation must be
  initialized before use.
  */
class AbstractProgram {
protected:
	cl::Program mProgram;
	/**
	  Constructor of this class compiles program with source from path
	  parameter with given context
	  \param path path to file containing program source
	  \param context OpenCL context with which the program will be compiled
	  \throws BuildError thrown at build errors
	  */
	AbstractProgram(const std::string& path, cl::Context& context);
public:
	virtual ~AbstractProgram() { }
};

#endif
