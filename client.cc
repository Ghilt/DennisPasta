#include <iostream>

using namespace std;

#define UNUSED_PARAM(p) (void)p

int main(int argc, char** argv) {

	UNUSED_PARAM(argc);
	UNUSED_PARAM(argv);

	cout << "Hello client!" << endl;

	return 0;
}
