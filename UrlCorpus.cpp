#include "UrlCorpus.h"
#include <algorithm>
#include <fstream>
#include <unistd.h>
#include <iconv.h>


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

    if (html.empty())
    {
        throw invalid_argument("Incorrect URL");
    }

    if (access(path.c_str(), F_OK) != 0)
    {
        throw invalid_argument("The specified path does not exist");
    }

    char* effurl;
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effurl);

    url = effurl;
    url.erase(url.find('/', url.find("://") + 3));
    transform(url.begin(), url.end(), url.begin(), ::tolower);

    map<string, string> urls = getUrls(html, url);

    string article;

    long i = 1, j = 1, total = urls.size();

    for(map<string, string>::iterator it = urls.begin(); it != urls.end();)
    {
        cout << "checking page " << j++ << "/" << total << endl;

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
    CURLcode code = curl_easy_perform(curl);

    if (code != CURLE_OK) return "";

    return html;
}

string UrlCorpus::cp1251_to_utf8(string text)
{
    iconv_t conv = iconv_open("UTF-8", "CP1251");

    size_t insize = text.size();
    size_t outsize = insize * 2;

    char* res = new char[outsize + 1]();
    char* inbuf = (char*)text.c_str();
    char* outbuf = res;

    iconv(conv, &inbuf, &insize, &outbuf, &outsize);
    iconv_close(conv);

    string utf8text = string(res);

    delete[] res;

    return utf8text;
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

                if (absurl) url.erase(0, main_url.length());

                if (url.length() > 1 && url.front() == '/' && url[1] != '/')
                {
                    if (url.find_last_of("#;") == string::npos)
                    {
                        desc = "";
                        for(tree<HTML::Node>::iterator p = it.begin(); p != it.end(); p++)
                        {
                            if (!p->isTag() && !p->isComment())
                            {
                                desc += p->text();
                            }
                        }

                        desc = HTML::single_blank(desc);

                        if (!desc.empty())
                        {
                            desc = HTML::decode_entities(desc);

                            if (!HTML::detect_utf8(desc.c_str(), (int)desc.size()))
                            {
                                desc = cp1251_to_utf8(desc);
                            }

                            urls.insert(make_pair(url, desc));
                        }
                    }
                }
            }
        }
    }

    return urls;
}

string UrlCorpus::getArticle(string& html)
{
    const int MIN_ARTICLE_SIZE = 500;
    const int MIN_ARTICLE_SIZE_UTF8 = MIN_ARTICLE_SIZE * 2;
    const int PARAGRAPH_MATCH_SIZE = 150;

    string article;

    tree<HTML::Node> dom = parser.parseTree(html);

    for(tree<HTML::Node>::iterator it = dom.begin(); it != dom.end(); it++)
    {
        if (it->tagName() == "script" || it->tagName() == "style" ||
            it->tagName() == "noindex" || it->tagName() == "footer")
        {
            it.skip_children();
            continue;
        }

        if (it->tagName() == "p" || !it->isTag() && !it->isComment() && it->length() > PARAGRAPH_MATCH_SIZE)
        {
            article = "";

            it = dom.parent(it);
            for(tree<HTML::Node>::iterator p = it.begin(); p != it.end(); p++)
            {
                if (p->tagName() == "script" || p->tagName() == "style" ||
                    p->tagName() == "div" || p->tagName() == "aside")
                {
                    p.skip_children();
                    continue;
                }

                if (!p->isTag() && !p->isComment())
                {
                    article += HTML::single_blank(p->text());
                }
                else if (p->tagName() == "p" || p->tagName() == "h1" || p->tagName() == "h2") article += "\n\n";
            }

            article.erase(article.begin(), find_if_not(article.begin(), article.end(), [](int c) { return isspace(c); }));

            article = HTML::decode_entities(article);

            if (HTML::detect_utf8(article.c_str(), (int)article.size()))
            {
                if (article.size() > MIN_ARTICLE_SIZE_UTF8) return article;
            }
            else if (article.size() > MIN_ARTICLE_SIZE)
            {
                return cp1251_to_utf8(article);
            }

            it.skip_children();
        }
    }

    return "";
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
