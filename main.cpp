//TODO: Merge files into one. We're not experienced with multi-file programming. May reserve a header file for future use.

#include<curl/curl.h>
#include<iostream>
#include<string>
#include<filesystem>
#include<vector>
#define CAO "http://crawl.akrasiac.org/rawdata/"
#define CKO "https://crawl.kelbi.org/crawl/morgue"
#define CPO "https://crawl.project357.org/morgue"
using namespace std;
using namespace std::filesystem;
CURL* curl_handle;
string username="code2828";

struct TeCombo
{
	string full;
	char s[3]="Aa";
	char b[3]="Zz";
	string Dr_color="";
};

struct TeR
{
	string filename;
	char site[1000];
	TeCombo combo;
	char god;
	size_t score;
	unsigned short xl;
	size_t turn;
	string playtime;
};

vector<TeR> v; // vector that stores every game
const char short_gods[27][20]={"Ash","Beogh","Chei","Dith","Ely","Fedhas","Gozag","Hep","Ignis","Jiyva","Kiku","Lugonu","Makhleb","Nemelex","Oka","Pake","Qazlal","Ru","Sif","Trog","Uskayaw","Veh","Wu Jian","Xom","Yred","Zin","TSO"};
const char long_gods[27][40]={"Ashenzari","Beogh","Cheibriados","Dithmenos","Elyvilon","Fedhas Medash","Gozag Ym Sagoz","Hepliaklqana","Ignis","Jiyva","Kikubaaqudgha","Lugonu","Makhleb","Nemelex Xobeh","Okawaru","Pakellas","Qazlal","Ru","Sif Muna","Trog","Uskayaw","Vehumet","Wu Jian Council","Xom","Yredelemnul","Zin","The Shining One"};
const char full_gods[27][100]={""}; // will be the gods with their titles
char* p[10];
size_t pindex=0;

size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	printf("%zu: %s\n",pindex++,ptr);		
	return size*nmemb;
}

int main(int argc, const char** argv)
{
    /*
     * 1. Fetch information from websites (CAO, CKO)
     * 2. Find the links in fetched indexes
     * 3. Download morgue files
     * 4. Grab the informatino from morgues
     * 5. Output stats, and give some comments (implememt later)
     */
	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle=curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, /* some url */);
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, p);
	curl_easy_perform(curl_handle);
	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();

    return 0;
}
