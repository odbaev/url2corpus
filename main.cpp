#include "UrlCorpus.h"
#include <iostream>

using namespace std;


int main(int argc, char** argv)
{
    if (argc < 3)
    {
        cout << "usage: url2corpus url corpus_path";
        return 0;
    }

    UrlCorpus urlcorp;

    urlcorp.getCorpus(argv[1], argv[2]);
}