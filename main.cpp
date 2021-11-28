#include "CSVParser.h"

using namespace std;

int main()
{
    ifstream a("input.csv");
    CSVParser<string, double> parser(a,0);
    for(tuple<string, double> rs : parser)
        cout << rs << endl;

    return 0;
}

