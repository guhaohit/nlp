#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ext/hash_map>
#include <iomanip>
#include <stdio.h>
#include <time.h>
#include <dirent.h>


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

typedef pair<string,int> wordpair;
hash_map<string,int> wordhash;
hash_map<string,int> otherhash;
hash_map<string,int> bigramhash;
string last="";
string first = "    ";

void read_dict(void)
{
	string strtmp;
	string oneword;
	//typedef pair<string,int> wordpair;
	ifstream infile("lexicon.txt");
	if(!infile.is_open())
	{
		cerr << "Unable to open input file" <<endl;
		exit(-1);
	}
	while (getline(infile, strtmp))
	{
		istringstream stream(strtmp);
		stream >> oneword;
		wordhash.insert(wordpair(oneword,0));
	}
	infile.close();
	infile.open("gb_table.txt");
	if(!infile.is_open())
	{
		cerr << "Unable to open input file" <<endl;
		exit(-1);
	}
	while (getline(infile, strtmp))
	{
		istringstream stream(strtmp);
		stream >> oneword;
		otherhash.insert(wordpair(oneword,0));
	}
	infile.close();
}

string MM_line(string s1)
{
	string s2 = "";
	int num =0;
	string preword;
	while(!s1.empty())
	{
		int len = (int) s1.length();
		if(len > MaxWordLength)
		{
			len = MaxWordLength;
		}

		string w = s1.substr(0,len);
		while(len > CN_LEN && ( wordhash.find(w) == wordhash.end() ))
		{
			len -= CN_LEN;
			w = w.substr(0,len);
		}

		if(wordhash.find(w) != wordhash.end())
		{
			wordhash[w] = wordhash[w]+1;
			if (num == 0)
			{
				num = 1;
				preword = w;
			}
			else
			{
				string biword=preword+" "+w;
				if(bigramhash.find(biword) != bigramhash.end())
				{
					bigramhash[biword]=bigramhash[biword]+1;
				}
				else
				{
					bigramhash.insert(wordpair(biword,1));
				}
				preword = w;
			}
		}
		else if (otherhash.find(w) != otherhash.end())
		{
			otherhash[w] = otherhash[w]+1;

			if (num == 0)
			{
				num = 1;
				preword = w;
			}
			else
			{
				string biword=preword+" "+w;
				if(bigramhash.find(biword) != bigramhash.end())
				{
					bigramhash[biword]=bigramhash[biword]+1;
				}
				else
				{
					bigramhash.insert(wordpair(biword,1));
				}
				preword = w;
			}
		}
		s2 += w+Separator;
		s1 = s1.substr(w.length(), s1.length());
	}
	return s2;
}


string pre_segment(string s1)
{
	string s2;
	string s3 = "";
	int p1 = 0;
	int p2 = 0;
	int count;

	while(p2 < s1.length())
	{
		if(((s1[p2]>>4) & 0xe) ^ 0xe)
		{
			count = 0;
			do
			{
				p2++;
				count++;
			}while((((s1[p2]>>4)&0xe) ^0xe) && p2<s1.length());
			s2 = s1.substr(p1,p2-count-p1);
			s3 += MM_line(s2) + s1.substr(p2-count,count)+Separator;
			if(p2 <= s1.length())
			{
				s1 = s1.substr(p2,s1.length()-p2);
			}
			p1=p2=0;
		}
		else
		{
			p2 += CN_LEN;
		}
		if(p2 != 0)
		{
			s3 += MM_line(s1);
		}
		return s3;
	}
}

string eat_space(string s1)
{
	int p1=0,p2=0;
	int count;

	string s2;
	while(p2<s1.length())
	{
		if(s1[p2] - 0x20 == 0)
		{
			if(p2>p1)
			{
				s2 += s1.substr(p1,p2-p1);
			}
			p2++;
			p1=p2;
		}
		else
		{
			p2++;
		}
	}
	s2+=s1.substr(p1,p2-p1);

	return s2;
}

int GetAllFiles(string dirname)
{
    DIR* dp;
    struct dirent* dirp;
    string strinput;
	string stroutput;
	int count=0;
	string fenci="./分词/fenci";
	string fenci2;


    /* open dirent directory */
    if((dp = opendir(dirname.c_str()) )== NULL)
    {
        perror("opendir");
        return -1;
    }
    while((dirp = readdir(dp)) != NULL)
    {

    	if(dirp->d_name[0] == '.')
            continue;
        cout<<dirp->d_name<<endl;
        count++;
        stringstream ss;
    	string s;
    	ss << count;
    	ss >> s;
        fenci2 = fenci +s+".txt";
        ofstream outfile1(fenci2.c_str());
		if(!outfile1.is_open())
		{
			cerr << "Unable to open output file" <<endl;
			exit(-1);
		}
        string newfile = dirname+"/"+dirp->d_name;
        ifstream infile(newfile.c_str());
		if(!infile.is_open())
		{
			cerr << "Unable to open input file" <<endl;
			exit(-1);
		}
    	while(getline(infile,strinput))
		{
			strinput = first+strinput;
			//cout << strinput <<endl;
			strinput = eat_space(strinput);
			//cout << strinput <<endl;
			//strinput = last+strinput;
			stroutput = pre_segment(strinput);
			//stroutput = MM_line(strinput);
			//cout << stroutput << endl;
			outfile1 << stroutput << endl;
		}
		outfile1.close();
    }
    cout<<count<<endl;
    closedir(dp);

}

int main(int argc, char const *argv[])
{
	/* code */
	string l1,l2,line;
	clock_t start,finish;
	double duration;

	start = clock();
	read_dict();
	finish = clock();
	duration = (double)( finish-start ) / CLOCKS_PER_SEC;
	cout << "read file end, time: " << duration<<"s"<<endl;



	ofstream outfile("unigram.txt");
	if(!outfile.is_open())
	{
		cerr << "Unable to open input file" <<endl;
		exit(-1);
	}

	ofstream outfile2("bigram.txt");
	if(!outfile2.is_open())
	{
		cerr << "Unable to open input file" <<endl;
		exit(-1);
	}

	start = clock();
	GetAllFiles("./人民日报96年语料");

	finish = clock();
	duration = (double)( finish-start ) / CLOCKS_PER_SEC;
	cout << "time: " << duration<< "s"<<endl;



	for (hash_map<string,int>::iterator itb1 = wordhash.begin();  itb1!= wordhash.end(); itb1++)
	{
		outfile<< itb1->first <<" "<< itb1->second << endl;
	}
	for (hash_map<string,int>::iterator itb2 = otherhash.begin();  itb2!= otherhash.end(); itb2++)
	{
		outfile<< itb2->first <<" "<< itb2->second << endl;
	}
	for(hash_map<string,int>::iterator itb3 = bigramhash.begin(); itb3!=bigramhash.end();itb3++)
	{
		outfile2<<itb3->first<<" "<<itb3->second<<endl;
	}


	return 0;
}