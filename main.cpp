#include "UrlCorpus.h"
#include <iostream>

using namespace std;


int main(int argc, char** argv)
{
    if (argc < 3)
    {
        cout << "Usage: ./url2corpus url corpus_path" << endl;
        return 0;
    }

    UrlCorpus urlcorp;

    try
    {
        urlcorp.getCorpus(argv[1], argv[2]);
    }
    catch (const invalid_argument& ia)
    {
        cerr << "Error: " << ia.what() << endl;
    }

    return 0;
}