//TODO: Merge files into one. We're not experienced with multi-file programming. May reserve a header file for future use.

#include<curl/curl.h>
#include<iostream>
#include<string>
#include<cstring>
#include<filesystem>
#include<vector>
#define TEOPMASK_CAO	(1<<0)
#define TEOPMASK_CKO	(1<<1)
#define TEOPMASK_CPO	(1<<2)
#define TEOPMASK_TXT	(1<<15)
#define TEOPMASK_LST	(1<<16)
#define TEOPMASK_TS		(1<<17)
#define TEOPMASK_TTYREC	(1<<18)
#define TEOPMASK_DUMP	(1<<19)
#define TEOPMASK_HELP	(1<<20)
#define TEOPMASK_NONE	(1<<21)
#define TEOPMASK_EMPTY	(1<<22)
#define TEOPMASK_USER	(1<<23)
#define TEOPMASK_INVALU	(1<<24)
#define TEOPMASK_NCOMM	(1<<25)
#define TEOPMASK_UKCOMM	(1<<26)
#define TEOPMASK_UKOPT	(1<<27)
#define TEOPMASK_FETCH	(1<<28)
#define TEOPMASK_IGNEX	(1<<29)
#define TEOPMASK_STAT	(1<<30)
#define TEOPMASK_DEBUG	(1<<31)
#define CAO "http://crawl.akrasiac.org/rawdata/"
#define CKO "https://crawl.kelbi.org/crawl/morgue/"
#define CPO "https://crawl.project357.org/morgue/"
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
vector<pair<int,int> >q; // vector that stores every file need to download
const char short_gods[27][20]={"Ash","Beogh","Chei","Dith","Ely","Fedhas","Gozag","Hep","Ignis","Jiyva","Kiku","Lugonu","Makhleb","Nemelex","Oka","Pake","Qazlal","Ru","Sif","Trog","Uskayaw","Veh","Wu Jian","Xom","Yred","Zin","TSO"};
const char long_gods[27][40]={"Ashenzari","Beogh","Cheibriados","Dithmenos","Elyvilon","Fedhas Medash","Gozag Ym Sagoz","Hepliaklqana","Ignis","Jiyva","Kikubaaqudgha","Lugonu","Makhleb","Nemelex Xobeh","Okawaru","Pakellas","Qazlal","Ru","Sif Muna","Trog","Uskayaw","Vehumet","Wu Jian Council","Xom","Yredelemnul","Zin","The Shining One"};
const char full_gods[27][100]={""}; // will be the gods with their titles
vector<string> p;
size_t pindex=0;

size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	string hey=static_cast<string>(ptr);
	if(p.size()==pindex)p.push_back(static_cast<string>(""));
	p[pindex].append(hey);
	return size*nmemb;
}

size_t write_file(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t written=fwrite(ptr,size,nmemb,(FILE*)userdata);
    return written;
}

/*
 * int has 31 non-sign bits.
 * the sign bit is set if the debug option `-d` is given
 * bits 0,1,2 stands for if downloads from CAO, CKO and CPO, respectively
 * bits 16,17,18,19 indicates whether morgue.lst/map, timestamp, ttyrecs and character dumps should be downloaded, respectively
 * bit 20 is set if `-h` is found in the options
 * bit 21 is set if none of -mMst, is given
 * bit 22 is set if nothing at all is given
 * bit 23 is set if a username is given, bit 24 is set if this username is invalid
 * bit 25 is set if no command is given, bit 26 is set if an unknown command is given, bit 27 is set if an unknown option is given
 * bit 28 is set if fetch or update, 29 if update, 30 if stat
 */
int parse_opts(int argc, const char** argv)
{
	int r=0;
	bool nosite=true,nodown=true;
	if(argc==1)r|=TEOPMASK_EMPTY;
	for(int i=1;i<argc;i++)
	{
		if(!strcmp(argv[i],"fetch"))r|=TEOPMASK_FETCH;
		else if(!strcmp(argv[i],"update"))r|=TEOPMASK_FETCH,r|=TEOPMASK_IGNEX;
		else if(!strcmp(argv[i],"stat"))r|=TEOPMASK_STAT;
		else if(!strcmp(argv[i],"auto"))r|=TEOPMASK_FETCH,r|=TEOPMASK_STAT;
		else if(isalpha(argv[i][0]))r|=TEOPMASK_UKCOMM;
		else if(argv[i][0]=='-')
		{
			if(argv[i][1]=='-') // username
			{
				r|=TEOPMASK_USER;
				username=static_cast<string>(argv[i]);
				username.erase(username.begin());
				username.erase(username.begin());
			}
			else // option
			{
				int j=1;
				while(argv[i][j])
				{
					switch(argv[i][j])
					{
						case 'a':r|=TEOPMASK_CAO;nosite=0;break;
						case 'b':break;
						case 'd':r|=TEOPMASK_DEBUG;break;
						case 'h':r|=TEOPMASK_HELP;break;
						case 'k':r|=TEOPMASK_CKO;nosite=0;break;
						case 'p':r|=TEOPMASK_CPO;nosite=0;break;
						case 'm':r|=TEOPMASK_TXT;nodown=0;break;
						case 'M':r|=TEOPMASK_LST;nodown=0;break;
						case 's':r|=TEOPMASK_TS;nodown=0;break;
						case 't':r|=TEOPMASK_TTYREC;nodown=0;break;
						case ',':r|=TEOPMASK_DUMP;nodown=0;break;
						default:r|=TEOPMASK_UKOPT;break;
					}
					j++;
				}
			}
		}
	}
	if(nosite)
	{
		r|=(TEOPMASK_CAO|TEOPMASK_CPO|TEOPMASK_CKO);
	}
	if(nodown)
	{
		r|=(TEOPMASK_TXT|TEOPMASK_LST|TEOPMASK_TS|TEOPMASK_TTYREC|TEOPMASK_DUMP);
	}
	return r;
}

void grab(string g_,int opts)
{
	char sit[1000]="";
	switch(g_[g_.length()-1])
	{
		case 'a':strcpy(sit,CAO);break;
		case 'k':strcpy(sit,CKO);break;
		case 'p':strcpy(sit,CPO);break;
		default:break;
	}
	string g=g_;
	// morgue.txt
	while(opts&TEOPMASK_TXT)
	{
		size_t po=g.find(static_cast<string>(">morgue-"));
		if(po==string::npos)break;
		g.erase(0,po+2);
		po++;
		string file=g.substr(po,27+username.length()-1);
		if(file[file.length()-2]!='x')continue;
		FILE* f=fopen(file.c_str(),"wb");
		char site[1000]="";
		strcpy(site,sit);
		strcat(site,username.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, f);
		curl_easy_setopt(curl_handle, CURLOPT_URL, strcat(site,file.c_str()));
		cout<<file<<'\t'<<site<<endl;
		curl_easy_perform(curl_handle);
		fclose(f);
	}
}

int main(int argc, const char** argv)
{
	int opts=parse_opts(argc,argv);
	if(opts<0)
	{
		cout<<"opts="<<opts<<endl;
		cout<<"username="<<username<<endl;
		return 0;
	}
	if(opts&TEOPMASK_UKOPT)cout<<"Unknown option.\n";
	if(opts&TEOPMASK_UKCOMM)cout<<"Unknown command.\n";
	if(opts&(TEOPMASK_HELP|TEOPMASK_EMPTY))
	{
		cout<<"Usage: "<<argv[0]<<" <fetch/update/auto/stat> [options]\n";
		cout<<"  where [options] can be `--username` where you specify your username or one of the following:\n";
		cout<<"\t-a: download from CAO\n";
		cout<<"\t-b: download from CBRO (ignored)\n";
		cout<<"\t-d: for debug use only\n";
		cout<<"\t-h: prints this help\n";
		cout<<"\t-k: download from CKO\n";
		cout<<"\t-m: only download morgue.txt files\n";
		cout<<"\t-M: also download morgue.lst/morgue.map files (ignored)\n";
		cout<<"\t-p: download from CPO\n";
		cout<<"\t-s: also download timestamp files (ignored)\n";
		cout<<"\t-t: also download ttyrecs (ignored)\n";
		cout<<"\t-,: also download character dumps (ignored)\n";
		cout<<"The program downloads everything in you user directory of every known webtiles site by default. To select your desired files, use the options -m, -M, -s, -t and -, above.\n";
		return 0;
	}
	/*
     * 1. Fetch information from websites (CAO, CKO)
     * 2. Find the links in fetched indexes
     * 3. Download morgue files
     * 4. Grab the informatino from morgues
     * 5. Output stats, and give some comments (implememt later)
     */
	username.append(static_cast<string>("/"));
	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle=curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, nullptr);
	char str[10000]=CAO;
	if(opts&TEOPMASK_CAO)curl_easy_setopt(curl_handle, CURLOPT_URL, strcat(str,username.c_str())),curl_easy_perform(curl_handle),p[pindex].append(static_cast<string>("a")),pindex++;
	strcpy(str,CKO);
	if(opts&TEOPMASK_CKO)curl_easy_setopt(curl_handle, CURLOPT_URL, strcat(str,username.c_str())),curl_easy_perform(curl_handle),p[pindex].append(static_cast<string>("k")),pindex++;
	strcpy(str,CPO);
	if(opts&TEOPMASK_CPO)curl_easy_setopt(curl_handle, CURLOPT_URL, strcat(str,username.c_str())),curl_easy_perform(curl_handle),p[pindex].append(static_cast<string>("p")),pindex++;
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
	for(vector<string>::iterator it=p.begin();it!=p.end();it++)
	{
		string gra=*it;
		grab(gra,opts);
	}

	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
    return 0;
}
