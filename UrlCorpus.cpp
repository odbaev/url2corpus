#include "UrlCorpus.h"
#include <algorithm>
#include <regex>

#include <fstream>

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
    char* effurl;

    string html = getHtml(url, &effurl);

    map<string, string> urls = getUrls(html, effurl);


}

size_t UrlCorpus::writeCallback(char* contents, size_t size, size_t nmemb, string* userp)
{
    userp->append(contents, size * nmemb);
    return size * nmemb;
}

string UrlCorpus::getHtml(const char* url, char** effurl)
{
    string html;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);
    curl_easy_perform(curl);

    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, effurl);

    return html;
}

map<string, string> UrlCorpus::getUrls(string& html, char* effurl)
{
    map<string, string> urls;
    string url;
    string desc;

    tree<HTML::Node> dom = parser.parseTree(html);

    for(tree<HTML::Node>::iterator it = dom.begin(); it != dom.end(); it++)
    {
        if (it->tagName() == "a")
        {
            it->parseAttributes();
            if (it->attribute("href").first)
            {
                url = it->attribute("href").second;

                bool absurl = url.compare(0, strlen(effurl), effurl) == 0;

                if (url.length() > 1 && url.front() == '/' || absurl)
                {
                    if (url.find_last_of("#?;") == string::npos)
                    {
                        if (absurl) url.erase(0, strlen(effurl) - 1);

                        long slashes = count(url.begin(), url.end(), '/');

                        if (slashes > 2 || slashes == 2 && url.back() != '/')
                        {
                            desc = "";
                            for(tree<HTML::Node>::sibling_iterator sit = dom.begin(it); sit != dom.end(it); sit++)
                            {
                                if (!sit->isTag() && !sit->isComment())
                                {
                                    desc = sit->text();
                                    break;
                                }
                            }

                            desc = HTML::single_blank(desc);

                            if (!desc.empty())
                            {
                                desc = HTML::decode_entities(desc);

                                urls.insert(make_pair(url, desc));
                            }
                        }
                    }
                }
            }
        }
    }

    return urls;
}








