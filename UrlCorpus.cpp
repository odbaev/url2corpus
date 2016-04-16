#include "UrlCorpus.h"
#include <algorithm>
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


void UrlCorpus::getCorpus(string url, string path)
{
    string html = getHtml(url);

    char* effurl;
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effurl);

    url = effurl;

    map<string, string> urls = getUrls(html, url);

    mkdir(path.c_str(), ACCESSPERMS);

    string article;

    int i = 1;
    for(map<string, string>::iterator it = urls.begin(); it != urls.end();)
    {
        html = getHtml(url + it->first);

        article = getArticle(html);

        if (article.empty())
        {
            it = urls.erase(it);
            continue;
        }

        ofstream out(path + '/' + to_string(i++) + ".txt");

        out << article << endl;

        out.close();

        it++;
    }

    writeCorpusInfo(path, url, urls);
}

size_t UrlCorpus::writeCallback(char* contents, size_t size, size_t nmemb, string* userp)
{
    userp->append(contents, size * nmemb);
    return size * nmemb;
}

string UrlCorpus::getHtml(string url)
{
    string html;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);
    curl_easy_perform(curl);

    return html;
}

map<string, string> UrlCorpus::getUrls(string& html, string main_url)
{
    map<string, string> urls;
    string url, desc;

    tree<HTML::Node> dom = parser.parseTree(html);

    for(tree<HTML::Node>::iterator it = dom.begin(); it != dom.end(); it++)
    {
        if (it->tagName() == "a")
        {
            it->parseAttributes();
            if (it->attribute("href").first)
            {
                url = it->attribute("href").second;

                bool absurl = url.compare(0, main_url.length(), main_url) == 0;

                if (url.length() > 1 && url.front() == '/' || absurl)
                {
                    if (url.find_last_of("#?;") == string::npos)
                    {
                        if (absurl) url.erase(0, main_url.length() - 1);

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

string UrlCorpus::getArticle(string html)
{
    string article;



    return article;
}

void UrlCorpus::writeCorpusInfo(string path, string main_url, map<string, string>& urls)
{
    ofstream out(path + "/corpus-info.txt");

    out << main_url << "\n" << endl;

    int i = 1;
    for(map<string, string>::iterator it = urls.begin(); it != urls.end(); it++)
    {
        out << "File: " << i++ << ".txt" << endl;
        out << "URL: " << it->first << endl;
        out << "Description: " << it->second << "\n" << endl;
    }

    out.close();
}













