#include <iostream>
#include <fstream>
#include <exception>
#include <sstream> 
#include <string>
#include <map>
#include "processline.h"
#include "orderprocessortest.h"
#include "orderbookmanager.h"

using namespace std;

// g++ -Wall -Werror -o orderprocessor orderprocessor.cpp
int main(int argc, char *argv[])
{
    try {

	//OrderProcessorTest orderProcessorTest;
	//orderProcessorTest.testAll();

        if (argc != 2) { 
            cout << "Need an input filename argument" << endl; 
	    return 1;
        }

        ifstream ifs(argv[1], std::ifstream::in);
        if (!ifs.good()) { 
            cout << "Can not open the file " << argv[1] << endl; 
	    return 1;
        }

        string line;
        while (getline(ifs, line)) {
            try {
                processLine(line);
            }
	    catch (exception& e) {
                cout << e.what() << ":" << line << endl;
            }
	}

        ifs.close();


        // This will printout one OrderBook
        //OrderBookManager::instance().print(cout, "ABB");

        // This will printout all OrderBooks
        OrderBookManager::instance().print(cout);
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
