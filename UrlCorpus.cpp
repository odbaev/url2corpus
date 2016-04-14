#include "UrlCorpus.h"

UrlCorpus::UrlCorpus()
{
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
}

UrlCorpus::~UrlCorpus()
{
    curl_easy_cleanup(curl);
}


void UrlCorpus::getCorpus(const char* url, const char* path)
{

}

size_t UrlCorpus::writeCallback(char* contents, size_t size, size_t nmemb, string* userp)
{
    userp->append(contents, size * nmemb);
    return size * nmemb;
}

string UrlCorpus::getHtml(const char* url)
{
    string html;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);
    curl_easy_perform(curl);

    return html;
}






