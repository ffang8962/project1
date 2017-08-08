#include <iostream>
#include <exception>

using namespace std;

// g++ -std=c++11 -Wall -Werror hello.cpp
int main(int argc, char *argv[])
{
    try {
        cout << "hello" << endl;
    }
    catch (exception& e) {
        cout << "Exception: " << e.what() << endl;
	return 1;
    }
    catch (...) {
        cout << "Unexpected Exception" << endl;
	return 1;
    }
    return 0;
}
