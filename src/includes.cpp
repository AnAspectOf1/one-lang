#include "includes.h"

using namespace qi;
using namespace one;


Array<String<>> Includes::searchDirs() {
	auto list = Array<String<>>(2);
	list[0] = "/usr/include";
	list[1] = "/usr/local/include";
	return list;
}
