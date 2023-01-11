#ifndef ADSA_H_HPP__
#define ADSA_H_HPP__
#include<string>
#include<curl/curl.h>
enum CrawlSite
{
    CAO,CKO,CPO
};
CURL* curl_handle;
std::string username="code2828";
int download(CrawlSite);
#endif