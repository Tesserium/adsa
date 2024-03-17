#include<curl/curl.h>
#include<cmath>
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
#define v1 (verbosity>=1)
#define v2 (verbosity>=2)
#define v3 (verbosity>=3)
#define v4 (verbosity>=4)
#define rnd(x) (floor((double)(x)*100000.0+0.5)/100000.0)
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
int spt[100]={0}; // SPecies played Times
int bgt[100]={0}; // BackGround played Times
int verbosity=0;
char uko='\0';
// short names for displaying
const char short_gods[27][20]={"Ash","Beogh","Chei","Dith","Ely","Fedhas","Gozag","Hep","Ignis","Jiyva","Kiku","Lugonu","Makhleb","Nemelex","Oka","Pake","Qazlal","Ru","Sif","Trog","Uskayaw","Veh","Wu Jian","Xom","Yred","Zin","TSO"};
// trivial names
const char long_gods[27][40]={"Ashenzari","Beogh","Cheibriados","Dithmenos","Elyvilon","Fedhas Medash","Gozag Ym Sagoz","Hepliaklqana","Ignis","Jiyva","Kikubaaqudgha","Lugonu","Makhleb","Nemelex Xobeh","Okawaru","Pakellas","Qazlal","Ru","Sif Muna","Trog","Uskayaw","Vehumet","Wu Jian Council","Xom","Yredelemnul","Zin","The Shining One"};
const char full_gods[27][100]={""}; // will be the gods with their titles
// all the games
vector<string> p;
size_t pindex=0;
string wd="."; // Working Directory

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
 * bits 0,1,2,3,4,5 stands for if downloads from CAO, CKO, CPO, CXC, CUE and CBRO respectively
 * bit 14 is set if any level of verbose (including 0) is specified.
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
		else if(isalpha(argv[i][0]))r|=TEOPMASK_UKCOMM;
		else if(argv[i][0]=='-')
		{
			if(argv[i][1]=='-') // is a username in `--username` syntax
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
						// -a: download from CAO
						// -b: download from CBRO
						// -d: also download character dumps
						// -D: directory to put the files
						// -h: prints this help
						// -k: download from CKO
						// -l: also download morgue.lst/morgue.map files
						// -m: only download morgue.txt files
						// -O<filename>: output result to <filename>
						// -p: download from CPO
						// -s: also download timestamp files
						// -t: also download ttyrecs
						// -u: download from CUE
						// -x: download from CXC
						// -0: verbose level 0 (no effect, only sets the VERB flag)
						// -1: verbose level 1
						// -2: verbose level 2
						// -3: verbose level 3
						// -4: verbose level 4, for developer only
						// -,: for debug use only
						case 'a':r|=TEOPMASK_CAO;nosite=0;break;
						case 'h':r|=TEOPMASK_HELP;break;
						case 'k':r|=TEOPMASK_CKO;nosite=0;break;
						case 'm':r|=TEOPMASK_TXT;nodown=0;break;
						case 'p':r|=TEOPMASK_CPO;nosite=0;break;
						case '0':r|=TEOPMASK_VERB;verbosity=0;break;
						case '1':r|=TEOPMASK_VERB;verbosity=1;break;
						case '2':r|=TEOPMASK_VERB;verbosity=2;break;
						case '3':r|=TEOPMASK_VERB;verbosity=3;break;
						case '4':r|=TEOPMASK_VERB;verbosity=4;break;
						case ',':r|=TEOPMASK_DEBUG;break;
						// options that need special care
						case 'O':case 'D':break;
						// TODO: UNFINISHED ONES
						case 'b':r|=TEOPMASK_CBRO;nosite=0;break;
						case 'u':r|=TEOPMASK_CUE;nosite=0;break;
						case 'x':r|=TEOPMASK_CXC;nosite=0;break;
                        case 'l':r|=TEOPMASK_LST;nodown=0;break;
                        case 's':r|=TEOPMASK_TS;nodown=0;break;
                        case 't':r|=TEOPMASK_TTYREC;nodown=0;break;
						case 'd':r|=TEOPMASK_DUMP;nodown=0;break;
						default:r|=TEOPMASK_UKOPT;uko=argv[i][j];break;
					}
					if(argv[i][j]=='O')
					{
						char outfile[1000]="";
						strcpy(outfile,argv[i]+j+1);
						freopen(outfile,"w",stdout);
						break;
					}
					else if(argv[i][j]=='D')
					{
						// not implemented.
						r|=TEOPMASK_UKOPT;uko='D';
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

// g_: file list as a string; opts: options.
void grab(string g_,int opts)
{
	char sit[1000]=""; // what site is this file?
	switch(g_[g_.length()-1])
	{
		case 'a':strcpy(sit,CAO);break;
		case 'k':strcpy(sit,CKO);break;
		case 'p':strcpy(sit,CPO);break;
		default:break;
	}
	string g=g_; // make a copy cuz we gotta change it int the future
	// morgue.txt
	while(opts&TEOPMASK_TXT)
	{
		// is archived?
		bool archiv=0;
		// position of a string that symbols filename
		size_t po=g.find(static_cast<string>(">morgue-"));
		if(po==string::npos)
		{
			// not found: no more morgues.
			break;
		}
		po++;
		// get the filename
		// a typical morgue filename is `morgue-code2828-20230816-024418.txt.gz`, that'll be 30 chars + length of username 
		string file=g.substr(po,30+username.length()-1);
		// remove the symbolic substr to avoid repeated downloading
		g.erase(0,po+6);
		// is not a txt
		if(file[file.length()-5]!='x')
		{
			continue;
		}
		// is a compressed file
		if(file[file.length()-1]=='z')
		{
			archiv=1;
			// bzip2 files end with bz2 not bz so we need to append this 2 to make it work properly
			if(file[file.length()-2]=='b')file.append("2");
		}
		// else it is not a archive, so we remove that last 3 characters to reveal the .txt extension
		else file.erase(file.length()-3);
		// is the file already there?
		if(opts&TEOPMASK_IGNEX)
		{
			if(exists(file.substr(0,27+username.length()-1)))
			{
				if(v2)cerr<<"File "<<file.substr(0,27+username.length()-1)<<" exists. Ignoring download request.\n";
				if(!archiv)q.push_back(file);
				else q.push_back(file.substr(0,27+username.length()-1));
				continue;
			}
		}
		// fetch & write to file
		FILE* f=fopen(file.c_str(),"wb");
		char site[1000]="";
		strcpy(site,sit);
		strcat(site,username.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, f);
		curl_easy_setopt(curl_handle, CURLOPT_URL, strcat(site,file.c_str()));
		// if not archive: directly push back file.
		if(!archiv)q.push_back(file);
		// if an archive: remove the archive extension for further stat analyzing.
		else q.push_back(file.substr(0,27+username.length()-1));
		// the real job
		if(v1)cerr<<"Fetching file "<<file<<"..."<<endl;
		curl_easy_perform(curl_handle);
		fclose(f);
		if(archiv) // is archive
		{
			// defaults to gzip archive. TODO: implement other archivers later on.
			char cmd[1000]="gzip -d -f  ";
			strncat(cmd,file.c_str(),900);
			int rety=system(cmd);
			if(rety)cerr<<"Decompression failed!\n";
		}
	}
}

void comb(TeR& r)
{
	TeCombo b=r.combo;
    string cbs=b.combo_string;
	// double word species? draconian? these has special abbrs
    bool is_double_sp=false,drc=0;
    b.combo_string.erase(b.combo_string.begin(),b.combo_string.begin()+b.combo_string.find(" ")+1);
    string s1=b.combo_string.substr(0,b.combo_string.find(" "));
    if((s1=="Red"||s1=="White"||s1=="Green"||s1=="Yellow"||s1=="Grey"||s1=="Black"||s1=="Purple"||s1=="Pale") // is a colored Dr
        &&b.combo_string.substr(0,min(b.combo_string.length()-1,8UL))!="Grey Elf") // but not a Grey Elf
        drc=1;
	// remove the first 'word'
	// after removal, colored Dr has the raw combo left, double word Sps has the second word left. Everything else has only the Bg left.
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
    else if(s1=="Fairy")b.s[0]='F',b.s[1]='r';
	else if(s1=="Gnome")b.s[0]='G',b.s[1]='m';
    else if(s1=="Gargoyle")b.s[0]='G',b.s[1]='r';
    else if(s1=="Merfolk")b.s[0]='M',b.s[1]='f';
    else if(s1=="Mayflytaur")b.s[0]='M',b.s[1]='y';
    else if(s1=="Octopode")b.s[0]='O',b.s[1]='p';
    else if(s1=="Vampire")b.s[0]='V',b.s[1]='p';
    else b.s[0]=s1[0],b.s[1]=s1[1];
    // backgrounds
    string s2=b.combo_string;
    size_t tmp=05;
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
	else if(s2=="Hexslinger")b.b[0]='H',b.b[1]='s';
    else b.b[0]=s2[0],b.b[1]=s2[1];
    r.combo=b;
}

void stats(string filename)
{
	if(v3)cerr<<"Going through "<<filename<<"...\n";
	// initialize the game struct
	if(v4)cerr<<"Initializing game struct.\n";
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
	if(v4)cerr<<"Searching "<<filename<<"...\n";
    while(!i.eof())
    {
        if(ve&&sc&&cb&&xl&&go)break;
        char cc[1000]="\0";
        i.getline(&cc[0],1000); // previously 100, that causes problems!!
        c=cc;
		if(v4)cerr<<"Current line:\t<LINE_START>"<<c<<"<LINE_END>\n";
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
                while(!isdigit(c[var++]));
				if(!isdigit(c[var]))r.x=c[var-1]-'0';
				else r.x=(c[var-1]-'0')*10+(c[var]-'0');
				// god
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
			// check turns & spent time (not implemented)
			else if((!tu)&&((var=c.find("The game lasted"))!=string::npos))
			{
				r.turn=0;
				var+=25;
				while(isdigit(c[++var])&&var<c.length())
				{
					r.turn*=10;
					r.turn+=c[var]-'0';
				}
				tu=1;
				break;
			}
        }
    }
	// Figure out what combo the game is.
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
		cerr<<"opts="<<opts<<endl;
		cerr<<"username="<<username<<endl;
		return 0;
	}
	if(opts&TEOPMASK_UKOPT)
	{
		cerr<<"Unknown option `-"<<uko<<"`.\n";
		cerr<<"Supported options: -Oahkmp01234,"<<endl;
		cerr<<"Run `"<<argv[0]<<" -h` for more information."<<endl;
		return 3;
	}
	if(opts&TEOPMASK_UKCOMM)
	{
		cerr<<"Unknown command.\n";
		return 4;
	}
	if(opts&(TEOPMASK_HELP|TEOPMASK_EMPTY))
	{
		cerr<<"Usage: "<<argv[0]<<" [options]\n";
		cerr<<"  where [options] can be `--username` where you specify your username or one of the following:\n";
		cerr<<"\t-O<filename>: redirect stdout to a file with name <filename>\n";
		cerr<<"\t-a: download from CAO\n";
		cerr<<"\t-h: prints this help\n";
		cerr<<"\t-k: download from CKO\n";
		cerr<<"\t-m: download txts (current default behavior)\n";
		cerr<<"\t-p: download from CPO\n";
		cerr<<"\t-0: verbose level 0 (no effect)\n";
		cerr<<"\t-1: verbose level 1\n";
		cerr<<"\t-2: verbose level 2\n";
		cerr<<"\t-3: verbose level 3\n";
		cerr<<"\t-4: verbose level 4, for developer only (prints a good deal of stuff!)\n";
		cerr<<"\t-,: for debug use\n";
		return 0;
	}
	/*
     * 1. Fetch information from websites
     * 2. Find the links in fetched indexes
     * 3. Download morgue files
     * 4. Grab the information from morgues
     * 5. Output stats, and give some comments (implement later)
     */
	username.append(static_cast<string>("/"));
	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle=curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, (long)(verbosity<2));
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, nullptr);
	// download from cao
	// TODO: push strcpy into each if statement. alloc str before everything. free & delete it afterwards.
	// Download list from CAO. Append a after the file to note its origin.
	char str[10000]=CAO;
	if(opts&TEOPMASK_CAO)
	{
		if(v1)cerr<<"Downloading list from CAO (" CAO<<username<<")..."<<endl;
		curl_easy_setopt(curl_handle, CURLOPT_URL, strcat(str,username.c_str()));
		curl_easy_perform(curl_handle),p[pindex].append(static_cast<string>("a"));
		pindex++;
	}
	// Download list from CKO. Append k after the file to note its origin.
	strcpy(str,CKO);
	if(opts&TEOPMASK_CKO)
	{
		if(v1)cerr<<"Downloading list from CKO (" CKO<<username<<")..."<<endl;
		curl_easy_setopt(curl_handle, CURLOPT_URL, strcat(str,username.c_str()));
		curl_easy_perform(curl_handle),p[pindex].append(static_cast<string>("k"));
		pindex++;
	}
	// Download list from CPO. Append p after the file to note its origin.
	strcpy(str,CPO);
	if(opts&TEOPMASK_CPO)
	{
		if(v1)cerr<<"Downloading list from CPO (" CPO<<username<<")..."<<endl;
		curl_easy_setopt(curl_handle, CURLOPT_URL, strcat(str,username.c_str()));
		curl_easy_perform(curl_handle),p[pindex].append(static_cast<string>("p"));
		pindex++;
	}
	// Start getting files.
	if(v3)cerr<<"Start getting files.\n";
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);
	for(vector<string>::iterator it=p.begin();it!=p.end();it++)
	{
		// For every list downloaded, do grab():
		string gra=*it;
		grab(gra,opts);
	}
	if(v3)cerr<<"Start analyzing stats.\n";
	// after grabbing we have the file on local storage & file names in q, a vector<string>.
	// foreach st:q analyze it stats by calling stats().
	for(vector<string>::iterator it=q.begin();it!=q.end();it++)
	{
		string st=*it;
		stats(st);
	}
	if(v3)cerr<<"Starting output.\n";
	sort(v.begin(),v.end(),cmp);
	size_t sum=0,num=0;
	int god_max=-1;
	char god_max_type;
	TeR last;
	cout<<"Score\t\tTurn\tScore per turn\tXL\tPlaytime\t\tCombo\t\t\tVersion"<<endl;
	for(vector<TeR>::iterator it=v.begin();it!=v.end();it++)
	{
		TeR r=*it;
		// for some reason there might be multiple files with the same name. to avoid repetition here we manually skip those of the same name
		if(r.filen==last.filen)continue;
		num++;
		sum+=r.score;
        	string godstr;
		if(r.god)
		{
			godstr.push_back('^');
			int this_god_times=++gwt[god_to_num(r.god)];
			if(this_god_times>=god_max)god_max=this_god_times,god_max_type=r.god;
			godstr.append(static_cast<string>(short_gods[god_to_num(r.god)]));
		}
		cout<<r.score<<"\t\t"<<r.turn<<"\t"<<((r.score||r.turn)?rnd(r.score/(double)r.turn):0)<<"\t\t"<<r.x<<"\t"<<r.playtime<<"\t"<<r.combo.s<<r.combo.b<<((!r.god)?"\t":godstr)<<"\t\t"<<r.v<<endl;
		last=r;
	}
	cout<<"Average score: "<<sum/(double)num<<" pts."<<endl;
	cout<<(god_max==-1?"You are a pure athiest and hasn't worshipped any god until now!\n":"You have worshipped ");
	if(god_max+1)
	{
		cout<<long_gods[god_to_num(god_max_type)]<<" for "<<god_max<<" times, which is the most among all gods."<<endl;
		// TODO: Give comment on god choice. such as (chei): "u sure have a slow-paced and easy life!"
	}
	cout<<flush;

	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
    return 0;
}

