# url2corpus

Getting text corpus from URL.

It can be used for extracting articles from online newspapers.

## Installation

You'll need to install **curl** and **htmlcxx** libraries:

    $ sudo apt-get install libcurl4-openssl-dev
    $ sudo apt-get install libhtmlcxx-dev

Then you can build **url2corpus** by typing:

    $ cmake url2corpus
    $ make

## Usage

    $ ./url2corpus url corpus_path

*url*: url of the page with links to pages containing some text information  
*corpus_path*: existing path to save corpus

For example:

    $ ./url2corpus lenta.ru ./corpus

As a result there will be text corpus with its description file *corpus-info.txt* in the specified path.

## Contributors

* Oleg Baev
* Mstislav Maslennikov
