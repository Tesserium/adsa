/*TODO: Fix numerous severe bugs that greatly prevents user from properly running the program
 * Bugs fixed list:
 * 1. Now can deal with compressed morgues.
 * 2. Erase was before assigning the filename, now reversed.
 * 
 * Unfixed known bugs:
 * 3. Akrasiac morgues sometimes is count twice.
 * 4. Produced useless info even without `-v`.
 */
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
#define TEOPMASK_IGNEX	(1<<29)
#define TEOPMASK_STAT	(1<<30)
#define TEOPMASK_DEBUG	(1<<31)
#define CAO "http://crawl.akrasiac.org/rawdata/"
#define CKO "https://crawl.kelbi.org/crawl/morgue/"
#define CPO "https://crawl.project357.org/morgue/"
#define tou(x) (('a'<=x&&x<='z')?(x-'a'+'A'):x)
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
const char short_gods[27][20]={"Ash","Beogh","Chei","Dith","Ely","Fedhas","Gozag","Hep","Ignis","Jiyva","Kiku","Lugonu","Makhleb","Nemelex","Oka","Pake","Qazlal","Ru","Sif","Trog","Uskayaw","Veh","Wu Jian","Xom","Yred","Zin","TSO"};
const char long_gods[27][40]={"Ashenzari","Beogh","Cheibriados","Dithmenos","Elyvilon","Fedhas Medash","Gozag Ym Sagoz","Hepliaklqana","Ignis","Jiyva","Kikubaaqudgha","Lugonu","Makhleb","Nemelex Xobeh","Okawaru","Pakellas","Qazlal","Ru","Sif Muna","Trog","Uskayaw","Vehumet","Wu Jian Council","Xom","Yredelemnul","Zin","The Shining One"};
const char full_gods[27][100]={""}; // will be the gods with their titles
vector<string> p;
size_t pindex=0;

size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	cout<<'.'<<flush;
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
						case 'v':r|=TEOPMASK_VERB;break;		
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
		r|=TEOPMASK_NONE;
	}
	return r;
}

void grab(string g_,int opts)
{
	size_t tmp=q.size();
	char sit[1000]="";
	cout<<4;
	switch(g_[g_.length()-1])
	{
		case 'a':strcpy(sit,CAO);break;
		case 'k':strcpy(sit,CKO);break;
		case 'p':strcpy(sit,CPO);break;
		default:break;
	}
	cout<<5;
	string g=g_;
	// morgue.txt
	while(opts&TEOPMASK_TXT)
	{
		bool archiv=0;
		size_t po=g.find(static_cast<string>(">morgue-"));
		if(po==string::npos)
		{
			break;
		}
		po++;
		string file=g.substr(po,30+username.length()-1);
		g.erase(0,po+2);	
		if(file[file.length()-5]!='x')
		{
			cerr<<"Found file "<<file<<", but it does not seem to be a morgue\n";
			continue;
		}
		if(file[file.length()-1]=='z')
		{
			archiv=1;
			if(file[file.length()-2]=='b')file.append("2");
		}
		else file.erase(file.length()-3);
		FILE* f=fopen(file.c_str(),"wb");
		char site[1000]="";
		strcpy(site,sit);
		strcat(site,username.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, f);
		curl_easy_setopt(curl_handle, CURLOPT_URL, strcat(site,file.c_str()));
		if(!archiv)q.push_back(file);
		else q.push_back(file.substr(0,27+username.length()-1));
		curl_easy_perform(curl_handle);
		fclose(f);
		cerr<<"Fetched "<<file<<endl;
		if(archiv) // is archive
		{
			cerr<<"The file is an archive. Decompressing using command `";
			char cmd[1000]="gzip -d -f  ";
			strncat(cmd,file.c_str(),900);
			cerr<<cmd<<"`\n";
			int rety=system(cmd);
			if(rety)cerr<<"Decompression failed!\n";
		}
	}
	cout<<"Added "<<q.size()-tmp<<" more games on site "<<sit<<endl;
}

void comb(TeR& r)
{
	TeCombo b=r.combo;
    string cbs=b.combo_string;
    bool is_double_sp=false,drc=0;
    b.combo_string.erase(b.combo_string.begin(),b.combo_string.begin()+b.combo_string.find(" ")+1);
    string s1=b.combo_string.substr(0,b.combo_string.find(" "));
    if((s1=="Red"||s1=="White"||s1=="Green"||s1=="Yellow"||s1=="Grey"||s1=="Black"||s1=="Purple"||s1=="Pale") // is a colored Dr
        &&b.combo_string.substr(0,min(b.combo_string.length()-1,8UL))!="Grey Elf") // but not a Grey Elf
        drc=1;
    b.combo_string.erase(0,b.combo_string.find(" ")+1);
    if(drc)
    {   
        b.Dr_color=s1;
        s1=b.combo_string.substr(0,b.combo_string.find(" "));
        b.combo_string.erase(0,b.combo_string.find(" ")+1);
    }   
    if(s1=="Deep"||s1=="Hill"||s1=="Mountain"||s1=="Sludge"||s1=="Grey"||s1=="Vine"||s1=="Lava") // is a 2-word species
    {   
        is_double_sp=true;
        s1.append(" ");
        s1.append(b.combo_string.substr(0,b.combo_string.find(" ")));
    }   
    if(is_double_sp)
    {   
        b.s[0]=s1[0];
        b.s[1]=s1[s1.find(" ")+1];
        b.combo_string.erase(0,b.combo_string.find(" ")+1);
    }   
    else if(s1=="Armataur")b.s[0]='A',b.s[1]='t';
    else if(s1=="Demigod")b.s[0]='D',b.s[1]='g';
    else if(s1=="Demonspawn")b.s[0]='D',b.s[1]='s';
    else if(s1=="Gnome")b.s[0]='G',b.s[1]='m';
    else if(s1=="Gargoyle")b.s[0]='G',b.s[1]='r';
    else if(s1=="Merfolk")b.s[0]='M',b.s[1]='f';
    else if(s1=="Mayflytaur")b.s[0]='M',b.s[1]='y';
    else if(s1=="Octopode")b.s[0]='O',b.s[1]='p';
    else if(s1=="Vampire")b.s[0]='V',b.s[1]='p';
    else b.s[0]=s1[0],b.s[1]=s1[1];
    // backgrounds
    //TODO: add backgrounds. currently replaceing it with the first 2 letters
    string s2=b.combo_string;
    size_t tmp=114514;
    b.combo_string=cbs;
    if(s2=="Hedge Wizard") // Hedge Wizard
    {
        if(r.v[1]<'9'&&r.v[1]>='6')b.b[0]='W',b.b[1]='z';// old hw
        else b.b[0]='H',b.b[1]='W';//new hw
    }
    else if((tmp=s2.find(' '))!=string::npos)b.b[0]=s2[0],b.b[1]=s2[tmp+1]; // is a double bg
    else if(s2=="Transmuter")b.b[0]='T',b.b[1]='m';
    else if(s2=="Warper")b.b[0]='W',b.b[1]='r';
    else if(s2=="Conjurer")b.b[0]='C',b.b[1]='j';
    else if(s2=="Wanderer")b.b[0]='W',b.b[1]='n';
    else if(s2=="Wizard")b.b[0]='W',b.b[1]='z';
    else b.b[0]=s2[0],b.b[1]=s2[1];
    r.combo=b;
}

void stats(string filename)
{
    TeR r;
    r.filen=filename;
    r.score=0;
    r.god='P';
    r.x=-1;
    r.playtime=filename.substr(7+username.length(),15);
    r.playtime=r.playtime.insert(4,static_cast<string>("."));
    r.playtime=r.playtime.insert(7,static_cast<string>("."));
    r.playtime=r.playtime.insert(13,static_cast<string>(":"));
    r.playtime=r.playtime.insert(16,static_cast<string>(":"));
    ifstream i(filename);
    string c="nothing here!";
    bool ve=0, // VErsion
         sc=0, // SCore
         cb=0, // ComBo
         tu=0, // TUrn
         xl=0, // XL
         go=0; // GOd
    while(!i.eof())
    {
        if(ve&&sc&&cb&&xl&&go)break;
        char cc[100]="\0";
        i.getline(&cc[0],100);
        c=cc;
        size_t var=0;
        int j=0;
        while(c[j++]==0&&j<c.length());
        if(j==c.length())continue;
        if(j==0)j++;
        for(;j<c.length();j++)
        {
            // check version
            if((!ve)&&c[j-1]=='0'&&c[j]=='.')
            {
                j++;
                if(j>=c.length()-2)break;
                string k;
                while(j<c.length()&&c[j-2]!='-')
                {
                    k.insert(k.end(),c[j++]);
                }
                r.v=k;
                ve=1;
            }
            // check score
            else if(c.find(" HPs)")!=string::npos&&(!sc))
            {
                for(int z=0;z<c.length()&&c[z]>='0';z++)
                {
                    r.score*=10;
                    r.score+=c[z]-'0';
                }
                sc=1;
                break;
            }
            // check combo
            else if((!cb)&&(var=c.find("Began as "))!=string::npos)
            {
                size_t end=c.find(" on ");
                if(end==string::npos)break;
                var+=9;
                r.combo.combo_string=c.substr(var,end-var);
                cb=1;
                break;
            }
            // check XL & god
            else if((!xl)&&(!go)&&((var=c.find("XL: "))!=string::npos))
            {
                // xl
                //TODO: XL NEED WORK!! scan: start counting when a number char is found, stop counting otherwise
                while(!isdigit(c[var++]));
                //r.x.insert(r.x.begin(),c[var]);
                //r.x.insert(r.x.begin(),c[var-1]);
				if(!isdigit(c[var]))r.x=c[var-1]-'0';
				else r.x=(c[var-1]-'0')*10+(c[var]-'0');
				// god
                //TODO: OLD VERSIONS
                i.getline(&cc[0],100);
                c=cc;
                var=c.find("God: ");
                if(var==string::npos)return;
                //var+=8;//! <--here
                var++;
                bool athe=0;
                while(!((c[var]<='Z'&&c[var]>='A')||c[var]=='t'))
                {
                    var++;
                    if(var==c.length()-1)
                    {
                        athe=1;
                        break;
                    }
                }
                r.god=athe?0:tou(c[var]);
                if(r.god=='T'&&(c[min(var+1,c.length()-1)]=='h'))r.god='1'; // is TSO (1)
                xl=go=1;
                break;
            }
        }
    }
    comb(r);
    v.push_back(r);
}

bool cmp(TeR a,TeR b)
{
	if(a.score!=b.score)return a.score>b.score;
	return strcmp(a.playtime.c_str(),b.playtime.c_str())<0;
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
		cout<<"\t-v: show progress and other informations\n";
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
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, (long)(!static_cast<bool>(opts&TEOPMASK_VERB)));
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, nullptr);
	char str[10000]=CAO;
	cout<<"Fetching file list from " CAO;
	if(opts&TEOPMASK_CAO)curl_easy_setopt(curl_handle, CURLOPT_URL, strcat(str,username.c_str())),curl_easy_perform(curl_handle),p[pindex].append(static_cast<string>("a")),pindex++;
	strcpy(str,CKO);
	cout<<"\netching file list from " CKO;
	if(opts&TEOPMASK_CKO)curl_easy_setopt(curl_handle, CURLOPT_URL, strcat(str,username.c_str())),curl_easy_perform(curl_handle),p[pindex].append(static_cast<string>("k")),pindex++;
	strcpy(str,CPO);
	cout<<"\nFetching file list from " CPO;
	if(opts&TEOPMASK_CPO)curl_easy_setopt(curl_handle, CURLOPT_URL, strcat(str,username.c_str())),curl_easy_perform(curl_handle),p[pindex].append(static_cast<string>("p")),pindex++;
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
	for(vector<string>::iterator it=p.begin();it!=p.end();it++)
	{
		string gra=*it;
		grab(gra,opts);
	}
	for(vector<string>::iterator it=q.begin();it!=q.end();it++)
	{
		string st=*it;
		stats(st);
	}
	sort(v.begin(),v.end(),cmp);
	size_t sum=0;
	for(vector<TeR>::iterator it=v.begin();it!=v.end();it++)
	{
		TeR r=*it;
        sum+=r.score;
        string godstr;
        godstr.push_back('^');
        if(r.god)godstr.append(static_cast<string>(short_gods[r.god=='1'?26:r.god-'A']));
        cout<<r.score<<"\t\t"<<r.x<<"\t"<<r.playtime<<"\t"<<r.combo.s<<r.combo.b<<((!r.god)?"\t":godstr)<<"\t\t"<<r.v<<endl;
    }   

	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
    return 0;
}
