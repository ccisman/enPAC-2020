
#ifndef PRODUCT_H
#define PRODUCT_H
#define TIME_LEFT 112
#define max_to_string 30
#include<cstdio>
#include<iostream>
#include<algorithm>
#include<set>
#include<vector>
#include<string>
#include<cstdlib>
#include "pnml_parse.h"
#include"SBA.h"
#define max_array_num 10000
#define hash_table_num 100000
using namespace std;

/*********************************Global_functions***********************************/
bool judgeF(string s);            //�ж���F����c����
int convert(RG rg, Petri ptnet, int i, int j);
int sumtoken(Petri ptnet, RG rg, string s, int statenum);         //���������token�ĺ� ��������s��  p1,p2,p3,p4,  ����ʽ                                                //����data  ���ؿɴ�ͼ �ڽӱ���±� 
bool handleFLTLF(Petri ptnet, RG rg, string s, int statenum);  //����F����  
void handleLTLCstep(Petri ptnet, RG rg, int &front_sum, int &latter_sum, string s, int statenum);//����ǰ�벿�ֺ�;��벿�ֺ� ;����  ��{�ĺ�һ���ַ���ʼ; handleLTLC�Ķ��� 
bool handleLTLC(Petri ptnet, RG rg, string s, int statenum);   //����C����  
/*******************************Global_functions End**********************************/

typedef struct Product
{                                      //���Զ�����״̬�ڵ� �� <RGname,BAname> ���� 
	
	int RGname_id;
	int BAname_id;
	
	//����< �Զ���������� (set������Ҫ�õ�) 
	/*bool operator <(const Product &a)const
	{
		if (a.RGname != RGname)//��RGname��ͬ ����RGname���� 
			return a.RGname < RGname;
		else                              //RGname��ͬ������� ���� BAname���� 
			return a.BAname < BAname;
	}*/
};



class hashtable
{
	//out�Ƕ�ostream��������ã� �ڸö����Ͻ���������� ostreamΪ��const�� ��Ϊд�뵽����ı�����״̬
	//friend ostream& operator<<(ostream &out, const hashtable& h);
public:
	//���ϣ���в���Ԫ��
	void insert(Product n);
	//����Ҫ�����λ��
	int hashfunction(int  s);

	//����hash���е�Ԫ��
	int search(Product n);
private:
	vector<Product> table[hash_table_num]; //һ����ʮ������
};



class Product_Automata
{
private:
//	vector<Product> status;      //���Զ���״̬����<RGname,BAname> 
//	vector<T> transition;           //��Ǩ���� 
	vector<Product> initial_status; //��ʼ״̬����
//	vector<Product> isaccept;       //�ɽ���״̬���ϼ���
//	vector<int> isAccept_id;           //�ɽ���״̬���ϵ�id��
	vector<Product>  stack;
	vector<Product>  flag;
	hashtable h;
	Petri *ptnet;
	RG *rg;
	SBA *ba;
	//int recursive_count;
	//int recursive_count2;
	bool result;
	int ret;
public:
	Product_Automata();
	void getProduct();//���ɽ��Զ���
	void addinitial_status();//���ӳ�ʼ״̬ 
	
	void ModelChecker(Petri *ptnet, RG *rg, SBA *ba,string propertyid,int &timeleft);
	void dfs1(Product q);
	void dfs2(Product q);
	//void initate_array(Petri ptnet,RG graph,SBA sba);
	bool isLabel(int vi, int sj);       //vi�ǿɴ�ͼ���±� sj���Զ������±� 
	~Product_Automata();
	//void garbowbfs(int cur, int temper);
	bool judgeF(string s);            //�ж���F����c����
	int convert( int i, int j);
	int sumtoken(string s, int statenum);         //���������token�ĺ� ��������s��  p1,p2,p3,p4,  ����ʽ                                                //����data  ���ؿɴ�ͼ �ڽӱ���±� 
	bool handleFLTLF(string s, int statenum);  //����F����  
	void handleLTLCstep(int &front_sum, int &latter_sum, string s, int statenum);//����ǰ�벿�ֺ�;��벿�ֺ� ;����  ��{�ĺ�һ���ַ���ʼ; handleLTLC�Ķ��� 
	bool handleLTLC( string s, int statenum);   //����C����  
	int getresult();
};

#endif // !_HASHTABLE_h


