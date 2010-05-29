/*! @file
 * @brief Software versioning of source code tree.
 *
 * This file is automatically mentained by script.
 */
#ifndef VERSION_H_
#define VERSION_H_

/* stl */
#include <string>
#include <sstream>
using namespace std;


#define VERSION_MAJOR	0	/*<! @brief Major version number. */
#define VERSION_MINOR	1	/*<! @brief Minor version number. */
#define VERSION_PATCH	0	/*<! @brief patch number. */


struct VERSION {
	VERSION() : bSet(false) {}
	VERSION(int vmajor, int vminor, int vpatch) : bSet(true) {
		this->major=vmajor; this->minor=vminor; this->patch=vpatch;
	}
	
	bool bSet;
	
	int major;
	int minor;
	int patch;
	
	
	string toStr() {
		// version has the format: v<major>.<minor>[-p<patch>]
		if(!bSet) return("");
		ostringstream stream;
		stream << "v" << major << "." << minor;
		if(patch!=0) stream << "-p" << patch;
		return(stream.str());
	}
};

VERSION getAppVersion();


#endif /* VERSION_H_ */
