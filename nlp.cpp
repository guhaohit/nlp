#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ext/hash_map>
#include <iomanip>
#include <stdio.h>
#include <time.h>
#include <map>

using namespace __gnu_cxx;
using namespace std;

#define MaxWordLength 24
#define CN_LEN 3
#define Separator "/"

namespace __gnu_cxx
{
	template<> struct hash<string>
	{
		size_t operator()(const string& s) const
		{
			return __stl_hash_string(s.c_str());
		}
	};
}

typedef pair<double,string> indexpair;
typedef pair<string,double> wordpair;
typedef pair< string,std::vector<string> > pinyinpair;

hash_map< string,std::vector<string> > pinyinhash;
hash_map<string,double> unigramhash;
hash_map<string,double> bigramhash;
hash_map<double,string> indexhash;

double unigramsum=0;
double bigramsum=0;
map<string, string> mapresult;
map<int, map<string, double> > mapwordnet;
typedef pair< int, map<string, double> > wordnetpair;

void read_dict()
{
	string strline,oneword;
	string str1;
	string strpinyin = "";

	ifstream infile("lexicon.txt");
	if(!infile.is_open())
	{
		cerr << "Unable to open input file" <<endl;
		exit(-1);
	}
	int index = 0;
	while(getline(infile, strline))
	{
		index++;
		cout<<index<<endl;
		istringstream stream(strline);
		stream >> oneword;
		//indexhash.insert(indexpair(index,oneword));
		while(stream >> str1)
		{
			str1=str1.substr(0,str1.size()-1);
			strpinyin = strpinyin + " " +str1;
		}
		if (pinyinhash.find(strpinyin) == pinyinhash.end())
		{
			/* code */
			std::vector<string> pinyinvec;
			pinyinvec.push_back(oneword);
			pinyinhash.insert(pinyinpair(strpinyin,pinyinvec));
		}
		else
		{
			pinyinhash[strpinyin].push_back(oneword);
		}
	}
	infile.close();
	cout<<"1"<<endl;

	infile.open("unigram_sorted.txt");
	if(!infile.is_open())
	{
		cerr << "Unable to open input file" <<endl;
		exit(-1);
	}

	string str2;
	double num;
	while(getline(infile, strline))
	{
		istringstream stream(strline);
		stream >> str1;
		stream >> str2;
		num = atof(str2.c_str())+0.5;
		unigramsum += num;

		unigramhash.insert(wordpair(str1,num));
	}
	infile.close();
	cout<<"2"<<endl;

	infile.open("bigram_sorted.txt");
	if(!infile.is_open())
	{
		cerr << "Unable to open input file" <<endl;
		exit(-1);
	}

	while(getline(infile, strline))
	{
		istringstream stream(strline);
		stream >> str1;
		stream >> str2;
		num = atof(str2.c_str());
		bigramsum += num;

		bigramhash.insert(wordpair(str1,num));
	}
	infile.close();
	cout<<"3"<<endl;
}

int viterbi(string input_pinyin)
{
	string str1;
	std::vector<string> pinyinvec;
	istringstream stream(input_pinyin);
	while(stream >> str1)
	{
		pinyinvec.push_back(str1);
	}

	string now = "";
	for (int len = 0; len < pinyinvec.size(); ++len)
	{
		if (len==0)
		{
			now=now+pinyinvec[len];
		}
		else
		{
			now = now+" "+pinyinvec[len];
		}
		if(pinyinhash.find(now) != pinyinhash.end())
		{
			std::vector<string> nowvec = pinyinhash[now];
			map<string, double> maponeword;
			for (int i = 0; i < nowvec.size(); ++i)
			{
				string oneword = nowvec[i];
				double p=unigramhash[oneword]/unigramsum;
				maponeword.insert(pair<string,double>(oneword,p));
				mapresult.insert(pair<string,string>(oneword,"0"));
			}

			mapwordnet.insert(wordnetpair(len+1,maponeword));
		}
	}

	for (int start = 1; start < pinyinvec.size(); ++start)
	{
		string strnow = pinyinvec[start];
		for (int end = start; end < pinyinvec.size(); ++end)
		{
			if (end > start)
			{
				strnow = strnow + " " + pinyinvec[end];
			}
			if (pinyinhash.find(strnow) != pinyinhash.end())
			{
				std::vector<string> nowvec=pinyinhash[strnow];
				map<string, double> maponeword;
				if (mapwordnet.find(start+1) != mapwordnet.end())
				{
					maponeword = mapwordnet[start+1];
				}

				for (int i = 0; i < nowvec.size(); ++i)
				{
					string maxpreword="0";
					double maxp=0;

					string oneword = nowvec[i];
					double p=unigramhash[oneword]/unigramsum;
					map<string, double> mappreword = mapwordnet[start];
					for (map<string,double>::iterator iter = mappreword.begin(); iter != mappreword.end(); iter++)
					{
						string strbigram = iter->first + oneword;
						if (bigramhash.find(strbigram) !=bigramhash.end())
						{
							double p_c = bigramhash[strbigram]/bigramsum;
							if (p_c*(iter->second)>maxp)
							{
								maxp = p_c*(iter->second);
								maxpreword = iter->first;
							}
						}
					}
					maponeword.insert(pair<string,double>(oneword,maxp));
					mapresult.insert(pair<string,string>(oneword,maxpreword));
				}

				mapwordnet.insert(wordnetpair(start+1,maponeword));
			}

			/* code */
		}
	}
	return pinyinvec.size();

}

void print_result(string maxword)
{
	if (mapresult[maxword] != "0")
	{
		print_result(mapresult[maxword]);
		cout<<maxword;
	}
	else
	{
		cout<<endl;
	}


}

int main(int argc, char const *argv[])
{
	read_dict();

	cout<<"please input pinyin"<<endl;
	string input_pinyin;
	cin>>input_pinyin;
	int len = viterbi(input_pinyin);
	map<string,double> maplast = mapwordnet[len];
	int maxp = 0;
	string maxword = "";
	for (map<string,double>::iterator iter = maplast.begin(); iter != maplast.end(); iter++)
	{
		if (iter->second > maxp)
		{
			maxp = iter->second;
			maxword = iter->first;
		}

	}
	print_result(maxword);

	return 0;
}