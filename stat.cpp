/*
 * =====================================================================================
 *
 *       Filename:  stat.cpp
 *
 *    Description:  Statistics counter for my dcss games 
 *
 *        Version:  1.0
 *        Created:  2023/01/10
 *       Revision:  none
 *       Compiler:  g++
 *
 * =====================================================================================
 */
#include<iostream>
#include<algorithm>
#include<string>
#include<stack>
#include<fstream>
#include<filesystem>
#include<vector>
#include<cstdint>
using namespace std;
using namespace std::filesystem;

struct cob
{
	string combo_string;
	uint8_t sp=UINT8_MAX;
	uint8_t bg=UINT8_MAX;
	char s[3]={'A','a','\0'};
	char b[3]={'Z','z','\0'};
	string Dr_color="";
};

struct R
{
	string filen;
	string v;
	cob combo;
	char god;
	long score;
	string x;
	long turn;
	string playtime;
};

vector<R> v;

const char short_gods[27][20]={"Ash","Beogh","Chei","Dith","Ely","Fedhas","Gozag","Hep","Ignis","Jiyva","Kiku","Lugonu","Makhleb","Nemelex","Oka","Pake","Qazlal","Ru","Sif","Trog","Uskayaw","Veh","Wu Jian","Xom","Yred","Zin","TSO"};

string username="code2828";
stack<string> s;

char tou(char x)
{
	if('a'<=x&&x<='z')return x-'a'+'A';
	else return x;
}

void comb(R& r)
{
	cob b=r.combo;
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
void here(string filename)
{
	R r;
	r.filen=filename;
	r.score=0;
	r.god='P';
	r.x="";
	if(filename[2]!='m'||filename[max(static_cast<unsigned long>(0),filename.length()-3)]!='t')
	{
		return;
	}
	r.playtime=filename.substr(10+username.length(),15);
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
				r.x.insert(r.x.begin(),c[var]);
				r.x.insert(r.x.begin(),c[var-1]);
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

bool cmp(R a,R b)
{
	if(a.score>b.score)return 1;
	if(a.score<b.score)return 0;
	for(int z=0;z<a.playtime.length();z++)if(a.playtime[z]>b.playtime[z])return 1;
	return 0;
}

int main()
{
	string path=".";
    for(const auto& entry:directory_iterator(path))s.push(entry.path());
while(!s.empty())
{
	here(s.top());
	s.pop();
}
	sort(v.begin(),v.end(),cmp);
	size_t sum=0;
	for(vector<R>::iterator it=v.begin();it!=v.end();it++)
	{
		R r=*it;
		sum+=r.score;
		string godstr;
		godstr.push_back('^');
		if(r.god)godstr.append(static_cast<string>(short_gods[r.god=='1'?26:r.god-'A']));
		cout<<r.score<<"\t\t"<<r.x<<"\t"<<r.playtime<<"\t"<<r.combo.s<<r.combo.b<<((!r.god)?"\t":godstr)<<"\t\t"<<r.v<<endl;
	}
	cout<<"Avg: "<<sum/v.size()<<endl;

	return 0;
}

