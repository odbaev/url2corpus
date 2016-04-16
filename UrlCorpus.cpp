#include "UrlCorpus.h"
#include <algorithm>
#include <regex>

#include <fstream>
#include <sys/stat.h>


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
    string html = getHtml(url);

    char* effurl;
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effurl);

    map<string, string> urls = getUrls(html, effurl);

    mkdir(path, ACCESSPERMS);

    writeCorpusInfo(path, effurl, urls);
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

void UrlCorpus::writeCorpusInfo(const char* path, char* url, map<string, string>& urls)
{
    ofstream out(string(path) + "/corpus-info.txt");

    out << url << "\n" << endl;

    int i = 1;
    for(map<string, string>::iterator it = urls.begin(); it != urls.end(); it++)
    {
        out << "File: " << i++ << ".txt" << endl;
        out << "URL: " << it->first << endl;
        out << "Description: " << it->second << "\n" << endl;
    }

    out.close();
}










