#include "includes.h"

using namespace chi;
using namespace one;


Array<String<>> Includes::searchDirs() {
	auto list = Array<String<>>(2);
	list[0] = "/usr/include";
	list[1] = "/usr/local/include";
	return list;
}
