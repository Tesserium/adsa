#include<iostream>
#include<curl/curl.h>
#include<fstream>
#include<string>
using namespace std;

ifstream fi("code2828.akrasiac.html");
CURL *curl_handle;

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
	/* set URL to get here */
	curl_easy_setopt(curl_handle, CURLOPT_URL, remote_name.c_str());
	/* Switch on full protocol/debug output while testing */
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);
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

int main(int argc, char* argv[])
{
	curl_global_init(CURL_GLOBAL_ALL);
	/* init the curl session */
	curl_handle = curl_easy_init();
	while(!fi.eof())
	{
		char pyy[1000];
		fi.getline(pyy,1000);
		string lxh=pyy;
		size_t pos=lxh.find("</td><td><a href=");
		if(pos==string::npos)continue;
		lxh=lxh.substr(pos-1);
		pos=19;
		size_t pos2=lxh.find("\">")-1;
		size_t len=pos2-pos+1;
		string url=lxh.substr(pos,len);
		if((pos=url.find("./"))!=string::npos)url.erase(pos,pos+2);
		if(url[0]=='/')continue;
		string path="http://crawl.akrasiac.org/rawdata/code2828/";
		path.append(url);
		cout<<"Fetching file "<<path<<"...\n";
		deal(url,path);
	}
	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
	return 0;
}

