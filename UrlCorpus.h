#ifndef URL2CORPUS_URLCORPUS_H
#define URL2CORPUS_URLCORPUS_H

#include <iostream>
#include <curl/curl.h>

using namespace std;

class UrlCorpus
{
public:
    UrlCorpus();
    ~UrlCorpus();
    void getCorpus(const char* url, const char* path);

private:
    static size_t writeCallback(char* contents, size_t size, size_t nmemb, string* userp);
    string getHtml(const char* url);

    CURL* curl;
};


#endif //URL2CORPUS_URLCORPUS_H
