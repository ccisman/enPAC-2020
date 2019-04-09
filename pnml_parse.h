#pragma once
#include <stdio.h>
#include <iostream>
#include<string>
#include<vector>
#include <stdlib.h>
#include<fstream>
#include<map>
#include "tinyxml.h"

using namespace std;

#define max_place_num 5000
#define max_tran_num 5000
#define max_node_num 10000

#define max_incoming 100








struct Arc {
	string id;
	string source;
	string target;
	int sourceNum = 0;
	bool sourceP; //false����Դ�ڵ��Ǳ�Ǩ��true����Դ�ڵ��ǿ���
	int targetNum = 0;
	int weight = 1;
	

};

struct Place
{
	string name;
	int num = 0;
//	map<string, int> placeid;
	int initialMarking = 0;
	bool sig = false;//�Ƿ�����Ҫ������������Ϊtrue
	int solution[max_place_num];//��Ҫ������Ӧ��P������
//	vector<int> solution;
	bool visited = false;//��Ϊ����Ҫ��������ǰβ�����Ƿ񱻸�ֵΪ1��(�����Է������ʱ����)
};

struct Transition
{
	string name;
	int num = 0;
//	map<string, int> transid;
};


string itos(int n);             //������intת����string 
int sum(int m[], int n);

class Petri
{
public:
	int m;   //��������ָ��
	int n;    //��Ǩ����ָ��
	int arcnum;  //������ָ��

	vector<Place> place;
	vector<Transition> transition;
	vector<Arc> arc;

	map<string, int> placeid;
	map<string, int> transid;

	vector<vector<double>> A;
	vector<vector<double>> A1;
	vector<vector<double>> A2;
public:
	Petri();
	void addPlace(Place p);
	void addTransition(Transition t);
	void addArc(Arc a);
	void readPNML(char *filename);  //����Petri
	void getA();           //���ɹ�������
	void printA();         //��ӡ��������
	void printA1();
	void UpperTriangularMatrix();//��������ת���������Ǿ���
	void exchange(double a[], double b[]); //����Ԫ�ػ���
	void sub(double a[], double b[], double k);//����:a[]=a[]-k*b[]
	
};
//�ɴ�ͼ�еĶ����
class RGNode {
public:
	string name;
	int *m;
	int *isfirable;
	int enableNum;                         //�ñ�ʶ���Է����ı�Ǩ��Ŀ
public:
    RGNode();
	~RGNode();
	void freespace();
};

class RG
{
public:
	int node;
	int edge;
	Petri ptnet;
	vector<RGNode> rgnode;

public:
	RG(Petri ptnet);
	RG();
	void addRGNode(RGNode rg);
//	void newReachabilityTree(Petri ptnet);    //���ɿɴ�ͼ
	//void PrintGraph(ofstream &outfile);//�ɴ�ͼ���ɵ��ļ���
	void standardOutput();

	int ReachabilityTree( RGNode currentnode,int rg_i);
	int RGinitialnode();
};

void convertRC(Petri ptnet, char *filename, int *placeInQuery);