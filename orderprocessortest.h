#ifndef orderprocessortest_h
#define orderprocessortest_h

#include <vector>
#include <string>
#include <set>
    
class Random
{
public:
    int gen(int min, int max) const;
};

class WeightedRandom : public Random
{
public:    
    void set(const std::vector<std::string>& values,
		   const std::vector<int>& weights);
    std::string get() const;


    void getTest() const;
private:
    std::vector<std::string> values;
    std::vector<int> weights;
    std::vector<int> sums;
};

struct Counter
{
    static unsigned int id;
};

class OrderBookTest
{
public: 
    OrderBookTest(const std::string& symbol);

    void next();

    void clear(unsigned int left = 10);

private:
    std::string symbol;
    std::set<unsigned int> ids;
    WeightedRandom op;    
};

class OrderProcessorTest
{
public:
    OrderProcessorTest();

    void testAll();
    void testOrderBook();
    void testOrderBookManager();

private:
};

#endif
