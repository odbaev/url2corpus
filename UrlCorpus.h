#ifndef URL2CORPUS_URLCORPUS_H
#define URL2CORPUS_URLCORPUS_H

#include <iostream>
#include <curl/curl.h>
#include <htmlcxx/html/ParserDom.h>
#include <htmlcxx/html/utils.h>

using namespace std;
using namespace htmlcxx;

class UrlCorpus
{
public:
    UrlCorpus();
    ~UrlCorpus();
    void getCorpus(string url, string path);

private:
    static size_t writeCallback(char* contents, size_t size, size_t nmemb, string* userp);
    string getHtml(string url);
    map<string, string> getUrls(string& html, string main_url);
    void writeCorpusInfo(string path, string main_url, map<string, string>& urls);

    CURL* curl;
    HTML::ParserDom parser;
};


#endif //URL2CORPUS_URLCORPUS_H
