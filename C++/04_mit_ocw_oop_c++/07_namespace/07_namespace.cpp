//============================================================================
// Name        : 07_namespace.cpp
// Author      : 
// Version     :
// Copyright   : 
// Description : namespace is an abstract type that contains a set of names
// useful for resolving name conflicts.
//============================================================================
namespace ford {
	class SUV {
	};
	class Compact{
	};

}

namespace dodge {
	class SUV {
	};
}

int main(void) {
	//using namespace ford; //<-exposes SUV and Compact
	using ford::SUV;
	SUV *s1 = new SUV();
	delete (s1);
	//ford::SUV s1 = new ford::SUV();
	dodge::SUV *s2 = new dodge::SUV();
	delete (s2);

}

