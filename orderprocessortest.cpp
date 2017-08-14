#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <cassert>
#include <algorithm>
#include <sstream>
#include "orderprocessortest.h"
#include "orderbook.h"
#include "orderbookmanager.h"
#include "processline.h"


using namespace std;

unsigned int Counter::id = 0;

int Random::gen(int min, int max) const
{
    int range = max - min + 1;
    return rand() % range + min;
}


void WeightedRandom::set(const vector<string>& values_in,
                   const vector<int>& weights_in)
{
    values = values_in;
    weights = weights_in;
    sums.assign(weights.size(), 0);
    assert(values.size() == weights.size()); 
    for (unsigned int i = 0; i < weights.size(); ++i) {
        sums[i] = weights[i] + ( (i == 0) ?  0 : sums[i-1] ); 
    }
}


string WeightedRandom::get() const
{
    int number = gen(0, sums[sums.size()-1] - 1);
    vector<int>::const_iterator iter = upper_bound(sums.begin(), sums.end(), number);
    assert(iter != sums.end());
    unsigned int dis = distance(sums.begin(), iter);
    assert(dis <= values.size()-1);
    return values[ dis ];
}

void WeightedRandom::getTest() const
{
    typedef map<string, pair<unsigned int, int> > ValueCount;
    ValueCount valueCount;

    for (unsigned int i = 0; i < values.size(); ++i) 
        valueCount[ values[i] ] = make_pair(i, 0);

    const int NUM = 50000;
    for (int i = 0; i < NUM; ++i) {
        string v = get();
	++(valueCount[v].second);
    }

    double ratio = 1.0 * NUM / sums[sums.size()-1];
    for (ValueCount::const_iterator i = valueCount.begin(); i != valueCount.end(); ++i) {
        cout << i->first << ", expected count: " << ratio * weights[i->second.first] << ", acutal: " << i->second.second << endl;
    }
}

//-----------------------------------------------------------------------------------------------------
OrderBookTest::OrderBookTest(const string& symbol)
  : symbol(symbol)
{
    const char* arr[] = {"A", "M", "D"};
    vector<string> values(arr, arr + sizeof(arr)/sizeof(const char*));
    int arr2[] = { 90, 5, 5 };
    vector<int> weights(arr2, arr2 + sizeof(arr2)/sizeof(int));
    op.set(values, weights);

    //op.getTest();
}

void OrderBookTest::next()
{
    stringstream ss;
    string operation = op.get();
    if (ids.empty()) operation = "A";

    ss << symbol << "|" << operation << "|";	

    if (operation == "A") {

        unsigned int id = (Counter::id)++;
        char side = (( id % 2) ? 'S' : 'B' );
        int size = op.gen(100, 200);
        double price = 1.0 * op.gen(1000, 3000) / 100.0;
        ss << side << "|" <<  id << "|" << size << "|" << price;

        ids.insert(id);
    }
    else { 
        assert( ids.size() > 0);
        unsigned int index = static_cast<unsigned int>(op.gen(0, ids.size()-1));
        set<unsigned int>::iterator iter =  ids.begin();
        advance(iter, index);
        unsigned int id = *iter;

        if (operation == "M") {
            int size = op.gen(100, 200);
            double price = 1.0 * op.gen(1000, 3000) / 100.0;
            ss << id << "|" << size << "|" << price;
        }
        else {  // "D"
            ss << id;  	    
            ids.erase(id);
	}
    }
    string line = ss.str();
    processLine(line); 
    //cout << line << endl;	
}


void OrderBookTest::clear(unsigned int left)
{
    unsigned int total = ids.size();
    if (total <= left) return;
    unsigned int size = total - left; 

    set<unsigned int>::iterator iter = ids.begin();
    for (unsigned i = 0; i < size; ++i, ++iter) {
        stringstream ss;
        ss << symbol << "|D|" << *iter;
        string line = ss.str();
        processLine(line); 
        //cout << line << endl;	
    }

    iter = ids.begin();
    advance(iter, size);
    ids.erase(ids.begin(), iter);
}


//-----------------------------------------------------------------------------------------------------




//-----------------------------------------------------------------------------------------------------
OrderProcessorTest::OrderProcessorTest()
{
    srand(time(0));
}

void OrderProcessorTest::testAll()
{
    cout << "Tests start ..." << endl;
    testOrderBook();
    testOrderBookManager();
    cout << "Tests end" << endl;
}	

void OrderProcessorTest::testOrderBook()
{
    OrderBookTest orderBookTest("ABB");

    const int NUM = 50000;
    for (unsigned int i = 0; i < NUM; ++i) {
        orderBookTest.next();
    }
    orderBookTest.clear(10);
    OrderBookManager::instance().print(cout, "ABB");
    orderBookTest.clear(0);
}

void OrderProcessorTest::testOrderBookManager()
{
    typedef map<string, OrderBookTest*> OrderBookManagerTest;
    OrderBookManagerTest orderBookManagerTest;
    
    vector<string> symbols;
    for (unsigned int i = 0; i < 26; ++i) {
	string s(1, 'A'+i);
        symbols.push_back(s);
        orderBookManagerTest[s] = new OrderBookTest(s); 
    }
    
    for (unsigned int i = 0; i < 5; ++i) {
        vector<string> temp;
        for (unsigned int j = 0; j < 10; ++j) {
            for (unsigned int k = 0; k < 26; ++k) {
                string s = symbols[j] + string(1, 'A' + k);
		temp.push_back(s);
                orderBookManagerTest[s] = new OrderBookTest(s); 
	    }  
	}
	symbols = temp;
    }

    unsigned int symbolSize = orderBookManagerTest.size();
    cout << "Symbol size=" << symbolSize << endl;

    Random random;
    const int NUM = 50000;
    for (unsigned int i = 0; i < NUM; ++i) {
        unsigned int index = static_cast<unsigned int>(random.gen(0, orderBookManagerTest.size()-1)); 
        OrderBookManagerTest::iterator iter = orderBookManagerTest.begin();
	advance(iter, index);
        iter->second->next();
    }

    cout << "left symbols: ";
    OrderBookManagerTest::iterator iter = orderBookManagerTest.begin(); 
    for (unsigned int i = 0; iter != orderBookManagerTest.end(); ++iter, ++i) {
        if ( i < symbolSize - 5 ) {
            iter->second->clear(0);
	}
	else {
            cout << iter->first << " ";
            iter->second->clear(3);
	}
    }
    cout << endl;

    OrderBookManager::instance().print(cout);

    for (iter = orderBookManagerTest.begin(); iter != orderBookManagerTest.end(); ++iter) {
	iter->second->clear(0);
        delete iter->second;
    } 
}

