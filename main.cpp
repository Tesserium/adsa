#include<curl/curl.h>
#include<iostream>
#include<string>
#include<algorithm>
#include<cstring>
#include<fstream>
#include<filesystem>
#include<vector>
#define TEOPMASK_CAO	(1<<0)
#define TEOPMASK_CKO	(1<<1)
#define TEOPMASK_CPO	(1<<2)
#define TEOPMASK_CXC    (1<<3)
#define TEOPMASK_CUE    (1<<4)
#define TEOPMASK_CBRO   (1<<5)
#define TEOPMASK_VERB	(1<<14)
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
// means IGNore EXisting files
#define TEOPMASK_IGNEX	(1<<29)
#define TEOPMASK_STAT	(1<<30)
#define TEOPMASK_DEBUG	(1<<31)
#define CAO "http://crawl.akrasiac.org/rawdata/"
#define CKO "https://crawl.kelbi.org/crawl/morgue/"
#define CPO "https://crawl.project357.org/morgue/"
// TODO: Put in real links.
#define CBRO "https://example.com/"
#define CXC "https://example.com/"
#define CUE "https://example.com/"
// quick to upper
#define tou(x) (('a'<=(x)&&(x)<='z')?((x)-'a'+'A'):(x))
#define god_to_num(y) (('A'<=(y)&&(y)<='Z')?(int)((y)-'A'):(((y)=='1')?26:-1))
using namespace std;
using namespace std::filesystem;
CURL* curl_handle;
string username="code2828";

struct TeCombo
{
	string combo_string;
	char s[3]="Aa";
	char b[3]="Zz";
	string Dr_color="";
};

// everything a game should have
struct TeR
{
	string v;
	string filen;
	char site[1000];
	TeCombo combo;
	char god;
	size_t score;
	short x;
	size_t turn;
	string playtime;
};

vector<TeR> v; // vector that stores every game
vector<string> q; // vector that stores every morgue file
int gwt[27]={0}; // God Worshipped Times
int spt[100]={0}; // SPecied played Times
int bgt[100]={0}; // BackGround played Times
// short names for displaying
const char short_gods[27][20]={"Ash","Beogh","Chei","Dith","Ely","Fedhas","Gozag","Hep","Ignis","Jiyva","Kiku","Lugonu","Makhleb","Nemelex","Oka","Pake","Qazlal","Ru","Sif","Trog","Uskayaw","Veh","Wu Jian","Xom","Yred","Zin","TSO"};
// trivial names
const char long_gods[27][40]={"Ashenzari","Beogh","Cheibriados","Dithmenos","Elyvilon","Fedhas Medash","Gozag Ym Sagoz","Hepliaklqana","Ignis","Jiyva","Kikubaaqudgha","Lugonu","Makhleb","Nemelex Xobeh","Okawaru","Pakellas","Qazlal","Ru","Sif Muna","Trog","Uskayaw","Vehumet","Wu Jian Council","Xom","Yredelemnul","Zin","The Shining One"};
const char full_gods[27][100]={""}; // will be the gods with their titles
// all the games
vector<string> p;
size_t pindex=0;

// two curl writing functions
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

// TODO: Implement this function
void check_compression()
{
	int p=system("gzip");
	int q=system("bzip2");
	int r=system("xz");
	if(p==1)
	{
		cerr<<"Your system does not have gzip and thus cannot decompress some archived morgues.\n";
	}
}

/*
 * int has 31 non-sign bits.
 * the sign bit is set if the debug option `-d` is given
 * bits 0,1,2,3,4 stands for if downloads from CAO, CKO, CPO, CXC, CUE and CBRO respectively
 * bit 14 is set if verbose is specified.
 * bits 15,16,17,18,19 indicates whether morgue.txt, morgue.lst/map, timestamp, ttyrecs and character dumps should be downloaded, respectively
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
		// command given
		if(!strcmp(argv[i],"fetch"))r|=TEOPMASK_FETCH;
		else if(!strcmp(argv[i],"update"))r|=TEOPMASK_FETCH,r|=TEOPMASK_IGNEX;
		else if(!strcmp(argv[i],"stat"))r|=TEOPMASK_STAT;
		else if(!strcmp(argv[i],"auto"))r|=TEOPMASK_FETCH,r|=TEOPMASK_IGNEX,r|=TEOPMASK_STAT;
		else if(isalpha(argv[i][0]))r|=TEOPMASK_NCOMM;
		else r|=TEOPMASK_UKCOMM;
		// option given
		for(int j=1;argv[i][j];j++)
		{
			switch(argv[i][j])
			{
				case 'C':nosite=false;break;
				case 'c':r|=TEOPMASK_CAO;break;
				case 'K':r|=TEOPMASK_CKO;break;
				case 'P':r|=TEOPMASK_CPO;break;
				case 'X':r|=TEOPMASK_CXC;break;
				case 'U':r|=TEOPMASK_CUE;break;
				case 'B':r|=TEOPMASK_CBRO;break;
				case 't':r|=TEOPMASK_TXT;break;
				case 'm':r|=TEOPMASK_LST;break;
				case 's':r|=TEOPMASK_TS;break;
				case 'T':r|=TEOPMASK_TTYREC;break;
				case 'd':r|=TEOPMASK_DUMP;nodown=0;break;
				case '-':r|=TEOPMASK_DEBUG;break;
				default:r|=TEOPMASK_UKOPT;
			}
		}
		if(nosite)r|=TEOPMASK_NONE;
		if(nodown)r|=TEOPMASK_NONE;
	}
	if((r&(TEOPMASK_FETCH|TEOPMASK_STAT|TEOPMASK_NCOMM))!=0)nosite=false;
	if(nosite)r|=TEOPMASK_NONE;
	return r;
}

void fetch_n(int n)
{
	// fstream owo;
	// owo.open("whatisthis",ios::out|ios::trunc);
	// owo<<p[0];
	// owo.close();
	
	CURLcode res;
	int downbit=0;
	ifstream owo;
	// owo.open("whatisthis");
	// owo>>p[pindex];
	// owo.close();
	// owo.open("thisshouldbe");
	// owo>>p[pindex];
	// owo.close();
	// const char* urls[5]={CBRO,CXC,CUE,CPO,CKO};
	// int downbit=0;
	// if(((t>>2)&1)==1)
	// {
	// 	// p[pindex]=p[pindex].substr(0,p[pindex].size()-1);
	// 	// owo.open("thisshouldbe",ios::out|ios::trunc);
	// 	// owo<<p[pindex]<<endl;
	// 	// owo.close();
	// 	owo.open("thisshouldbe");
	// 	owo>>p[pindex];
	// 	owo.close();
	// 	owo.open("whatisthis",ios::out|ios::trunc);
	// 	owo<<p[pindex]<<endl;
	// 	owo.close();
	// }
	// else
	// {
	// 	owo.open("whatisthis");
	// 	owo>>p[pindex];
	// 	owo.close();
	// }
	// p[0]=""
	
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, NULL);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curl_handle, CURLOPT_AUTOREFERER, 1);
	curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 5);
	
	if(p[pindex].size()==0)return;
	string p_static_copy = p[pindex];
	if(p_static_copy[0]=='\0')return;
	char *urls[5]={(char*)CBRO,(char*)CXC,(char*)CUE,(char*)CPO,(char*)CKO};
	if(((n>>0)&1)==1)
	{
		for(int i=0;i<5;i++)
		{
			string myurl = p_static_copy + urls[i];
			curl_easy_setopt(curl_handle, CURLOPT_URL, myurl.c_str());
			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
			res = curl_easy_perform(curl_handle);
			if (res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}
			else
			{
				owo.open("temp.tar.gz",ios::binary|ios::out|ios::trunc);
				owo.write(p[pindex].c_str(),p[pindex].size());
				owo.close();
				owo.open("temp.tar.gz",ios::binary|ios::in);
				curl_easy_setopt(curl_handle, CURLOPT_URL, urls[i]);
				curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
				FILE *ofile = fopen("temp.tar.gz", "wb");
				if (ofile)
				{
					curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, ofile);
					res = curl_easy_perform(curl_handle);
					fclose(ofile);
				}
				owo.close();
				if (res != CURLE_OK) {
					fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
				}
				owo.open("temp.tar.gz",ios::binary|ios::in);
				owo.read(p[pindex].data(),p[pindex].size());
				owo.close();
				curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, NULL);
			}
		}
	}
	if(((n>>1)&1)==1)
	{
		string myurl = p_static_copy + CAO;
		curl_easy_setopt(curl_handle, CURLOPT_URL, myurl.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
		res = curl_easy_perform(curl_handle);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else
		{
			owo.open("temp.tar.gz",ios::binary|ios::out|ios::trunc);
			owo.write(p[pindex].c_str(),p[pindex].size());
			owo.close();
			owo.open("temp.tar.gz",ios::binary|ios::in);
			curl_easy_setopt(curl_handle, CURLOPT_URL, CAO);
			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
			FILE *ofile = fopen("temp.tar.gz", "wb");
			if (ofile)
			{
				curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, ofile);
				res = curl_easy_perform(curl_handle);
				fclose(ofile);
			}
			owo.close();
			if (res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}
			owo.open("temp.tar.gz",ios::binary|ios::in);
			owo.read(p[pindex].data(),p[pindex].size());
			owo.close();
			curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, NULL);
		}
	}
	if(((n>>2)&1)==1)
	{
		string myurl = p_static_copy + CKO;
		curl_easy_setopt(curl_handle, CURLOPT_URL, myurl.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
		res = curl_easy_perform(curl_handle);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else
		{
			owo.open("temp.tar.gz",ios::binary|ios::out|ios::trunc);
			owo.write(p[pindex].c_str(),p[pindex].size());
			owo.close();
			owo.open("temp.tar.gz",ios::binary|ios::in);
			curl_easy_setopt(curl_handle, CURLOPT_URL, CKO);
			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
			FILE *ofile = fopen("temp.tar.gz", "wb");
			if (ofile)
			{
				curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, ofile);
				res = curl_easy_perform(curl_handle);
				fclose(ofile);
			}
			owo.close();
			if (res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}
			owo.open("temp.tar.gz",ios::binary|ios::in);
			owo.read(p[pindex].data(),p[pindex].size());
			owo.close();
			curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, NULL);
		}
	}
	if(((n>>3)&1)==1)
	{
		string myurl = p_static_copy + CPO;
		curl_easy_setopt(curl_handle, CURLOPT_URL, myurl.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
		res = curl_easy_perform(curl_handle);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else
		{
			owo.open("temp.tar.gz",ios::binary|ios::out|ios::trunc);
			owo.write(p[pindex].c_str(),p[pindex].size());
			owo.close();
			owo.open("temp.tar.gz",ios::binary|ios::in);
			curl_easy_setopt(curl_handle, CURLOPT_URL, CPO);
			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
			FILE *ofile = fopen("temp.tar.gz", "wb");
			if (ofile)
			{
				curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, ofile);
				res = curl_easy_perform(curl_handle);
				fclose(ofile);
			}
			owo.close();
			if (res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}
			owo.open("temp.tar.gz",ios::binary|ios::in);
			owo.read(p[pindex].data(),p[pindex].size());
			owo.close();
			curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, NULL);
		}
	}
	if(((n>>4)&1)==1)
	{
		string myurl = p_static_copy + CXC;
		curl_easy_setopt(curl_handle, CURLOPT_URL, myurl.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
		res = curl_easy_perform(curl_handle);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else
		{
			owo.open("temp.tar.gz",ios::binary|ios::out|ios::trunc);
			owo.write(p[pindex].c_str(),p[pindex].size());
			owo.close();
			owo.open("temp.tar.gz",ios::binary|ios::in);
			curl_easy_setopt(curl_handle, CURLOPT_URL, CXC);
			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
			FILE *ofile = fopen("temp.tar.gz", "wb");
			if (ofile)
			{
				curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, ofile);
				res = curl_easy_perform(curl_handle);
				fclose(ofile);
			}
			owo.close();
			if (res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}
			owo.open("temp.tar.gz",ios::binary|ios::in);
			owo.read(p[pindex].data(),p[pindex].size());
			owo.close();
			curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, NULL);
		}
	}
	if(((n>>5)&1)==1)
	{
		string myurl = p_static_copy + CUE;
		curl_easy_setopt(curl_handle, CURLOPT_URL, myurl.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
		res = curl_easy_perform(curl_handle);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else
		{
			owo.open("temp.tar.gz",ios::binary|ios::out|ios::trunc);
			owo.write(p[pindex].c_str(),p[pindex].size());
			owo.close();
			owo.open("temp.tar.gz",ios::binary|ios::in);
			curl_easy_setopt(curl_handle, CURLOPT_URL, CUE);
			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
			FILE *ofile = fopen("temp.tar.gz", "wb");
			if (ofile)
			{
				curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, ofile);
				res = curl_easy_perform(curl_handle);
				fclose(ofile);
			}
			owo.close();
			if (res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}
			owo.open("temp.tar.gz",ios::binary|ios::in);
			owo.read(p[pindex].data(),p[pindex].size());
			owo.close();
			curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, NULL);
		}
	}
	// owo.open("whatisthis");
	// owo>>p[pindex];
	// owo.close();
	// owo.open("thisshouldbe",ios::out|ios::trunc);
	// owo<<p[pindex];
	// owo.close();
	// if(downbit==1)
	// {
	// 	for(int i=0;i<5;i++)
	// 	{
	// 		string myurl = p[pindex] + urls[i];
	// 		curl_easy_setopt(curl_handle, CURLOPT_URL, myurl.c_str());
	// 		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
	// 		res = curl_easy_perform(curl_handle);
	// 		if (res != CURLE_OK) {
	// 			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	// 		}
	// 	}
	// }
	// if(downbit==2)
	// {
	// 	string myurl = p[pindex] + CAO;
	// 	curl_easy_setopt(curl_handle, CURLOPT_URL, myurl.c_str());
	// 	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
	// 	res = curl_easy_perform(curl_handle);
	// 	if (res != CURLE_OK) {
	// 		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	// 	}
	// }
	// if(downbit==3)
	// {
	// 	string myurl = p[pindex] + CKO;
	// 	curl_easy_setopt(curl_handle, CURLOPT_URL, myurl.c_str());
	// 	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
	// 	res = curl_easy_perform(curl_handle);
	// 	if (res != CURLE_OK) {
	// 		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	// 	}
	// }
	// if(downbit==4)
	// {
	// 	string myurl = p[pindex] + CPO;
	// 	curl_easy_setopt(curl_handle, CURLOPT_URL, myurl.c_str());
	// 	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
	// 	res = curl_easy_perform(curl_handle);
	// 	if (res != CURLE_OK) {
	// 		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	// 	}
	// }
	// if(downbit==5)
	// {
	// 	string myurl = p[pindex] + CXC;
	// 	curl_easy_setopt(curl_handle, CURLOPT_URL, myurl.c_str());
	// 	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
	// 	res = curl_easy_perform(curl_handle);
	// 	if (res != CURLE_OK) {
	// 		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	// 	}
	// }
	// if(downbit==6)
	// {
	// 	string myurl = p[pindex] + CUE;
	// 	curl_easy_setopt(curl_handle, CURLOPT_URL, myurl.c_str());
	// 	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
	// 	res = curl_easy_perform(curl_handle);
	// 	if (res != CURLE_OK) {
	// 		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	// 	}
	// }
	// curl_easy_setopt(curl_handle, CURLOPT_URL, urls[downbit]);
	// curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
	// res = curl_easy_perform(curl_handle);
	// if (res != CURLE_OK) {
	// 	fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	// }
}

void display_usage()
{
	printf("Usage: omiread [-htmsTkKPCXUB] [user] [user]...\n");
	printf("\n");
	printf("  -h    Display this help message.\n");
	printf("  -t    Only download the timestamp file.\n");
	printf("  -m    Only download the morgue list and the map file.\n");
	printf("  -s    Only download ttyrecs.\n");
	printf("  -T    Only download character dumps.\n");
	printf("  -k    Only download morgue.txt (default).\n");
	printf("  -K    Download ttyrecs and character dumps (default).\n");
	printf("  -P    Download morgues from CPO server (default).\n");
	printf("  -C    Download morgues from CAO server.\n");
	printf("  -X    Download morgues from CXC server.\n");
	printf("  -U    Download morgues from CUE server.\n");
	printf("  -B    Download morgues from CBRO server.\n");
}

int main(int argc, const char **argv)
{
	check_compression();
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl_handle = curl_easy_init();
	int option_bits = parse_opts(argc, argv);
	int down_bits = option_bits & 0x1F;
	if (option_bits & TEOPMASK_HELP || option_bits & TEOPMASK_UKCOMM || option_bits & TEOPMASK_UKOPT) {
		display_usage();
		return 0;
	}
	if (option_bits & TEOPMASK_STAT) {
		// Handle stat command
	}
	if (option_bits & TEOPMASK_FETCH) {
		if (option_bits & TEOPMASK_DEBUG) {
			cout << "Downloading options: " << down_bits << endl;
		}
		for (pindex = 0; pindex < p.size(); ++pindex) {
			fetch_n(down_bits);
		}
	}
	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
	return 0;
}
