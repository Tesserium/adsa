#include<iostream>
#include<fstream>
#include"h.hpp"
using namespace std;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

bool file_exists(string fn)
{
	ifstream f2;
	f2.open(fn);
	bool retur=f2.good();
	f2.close();
	return retur;
}

void deal(string filename, string remote_name)
{
	if(file_exists(filename)||file_exists(filename.substr(0,filename.length()-5))||file_exists(filename.substr(0,filename.length()-4)))
	{
		cout<<"The file \""<<filename<<"\" already exists, no need to download again.\n";
		return;
	}
	if(filename[0]!='m')return;// not morgue file
	/* set URL to get here */
	curl_easy_setopt(curl_handle, CURLOPT_URL, remote_name.c_str());
	/* Switch on full protocol/debug output while testing */
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
	/* disable progress meter, set to 0L to enable it */
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
	/* open the file */
	FILE* pagefile = fopen(filename.c_str(), "wb");
	if(pagefile) {
		/* write the page body to this file handle */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
		/* get it! */
		curl_easy_perform(curl_handle);
		/* close the header file */
		fclose(pagefile);
	}
}

int download(CrawlSite site)
{
	string path;
	switch(site)
	{
		case CAO:path="http://crawl.akrasiac.org/rawdata/";break;
		case CKO:path="https://crawl.kelbi.org/crawl/morgue/";break;
		case CPO:path="https://crawl.project357.org/morgue/";break;
		default:path="";break;
	}
	path.append(username);
	path.append(static_cast<string>("/"));
	deal("akr.html",path);
	/*while(!fi.eof())
	{
		char c=fi.get();
		string url="";
		deal(url,path);
	}*/
	/* cleanup curl stuff */

	return 0;
}

