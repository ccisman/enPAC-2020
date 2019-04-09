
#include "product.h"
#define FILE_BUFFER_LENGTH 30000
#include <fstream>
#include <unistd.h>
#include <signal.h>

/******************************Global_fucntions*********************************/

bool Product_Automata::judgeF(string s)//�ж���F����c���� 
{
	int pos = s.find("<=");
	if (pos == string::npos)
	{
		return true;            //��F���� 
	}
	else return false;          //c���� 
}
int Product_Automata::convert( int i, int j)//rgnode[i].m[j]
{
	int x = 0;
	int sum1 = 0;
	int sum2 = 0;

	for (int ii = 0; ii < ptnet->m; ii++)
	{
		sum1 += ptnet->place[j].solution[ii] * rg->rgnode[0].m[ii];
		//cout << "ptnet.place["<<j<<"].solution["<<ii<<"]: " << ptnet.place[j].solution[ii]<<endl;
		//cout << "rg.rgnode[0].m[" << ii << "]: " << rg.rgnode[0].m[ii] << endl;
	}
	//cout << "sum1:" << sum1 << endl;
	for (int jj = 0; jj < ptnet->m; jj++)
	{
		if (jj == j)
			continue;
		else
			sum2 += ptnet->place[j].solution[jj] * rg->rgnode[i].m[jj];
	}
	//cout << "sum2:" << sum2 << endl;
	x = (sum1 - sum2) / ptnet->place[j].solution[j];

	return x;
}


int Product_Automata::sumtoken( string s, int statenum)//���������token�ĺ� ��������s��  p1,p2,p3,p4,  ����ʽ   
{
	int sum = 0;
	while (1)
	{
		int pos = s.find_first_of(",");
		if (pos == string::npos)
			break;//��û���ҵ� ��˵��û���� 
		string subs = s.substr(0, pos);     //ȡ��һ��p1 
		//	cout<<"   "<<subs<<" ";
		int idex = ptnet->placeid.at(subs);                 //�õ������ı��
		if (rg->rgnode[statenum].m[idex] != -100)
			sum += rg->rgnode[statenum].m[idex];
		else//��Ϊ-100 ����Ҫת��  
		{
			int x;
			x = convert(statenum, idex);//statenum����i   idex����j
			//cout << "ת���õ�x: " << x << endl << endl;
			sum += x;

		}
			
		s = s.substr(pos + 1, s.length() - pos);          //��ǰ����ù���P1ȥ�� ��p2��ʼ��Ϊ�µ�s�� 
	}
	return sum;
}

bool Product_Automata::handleFLTLF( string s, int statenum)//����F����  
{
	int i, j;//whileѭ������ 
	if (s[0] == '!')//��ԭ����Ļ����� ��ȡ��
	{

		s = s.substr(2, s.length() - 2);//ȥ�� !, { 
		bool flag = true;             //��ʼ��Ϊtrue  
		while (1)
		{
			int pos = s.find_first_of(",");
			if (pos < 0)
				break;
			string subs = s.substr(0, pos);//ȡ��һ��t1 ȥ�� ��Ǩ����Ƚ� �õ���� ��ȥisfirable������ ����Ƿ�ȫ������	
			
					int idex = ptnet->transid.at(subs);//�õ��˱�Ǩ�ı��
					//��ȥisfirabe�������Ƿ���������
					for (j = 0; j < rg->rgnode[statenum].enableNum; j++)//30��isfirable����Ĵ�С
					{
						if (idex == rg->rgnode[statenum].isfirable[j])
						{
							break;
						}
					}
					if (j >= rg->rgnode[statenum].enableNum) {
						flag = false;//isfirable������û�ҵ� ����Ϊfalse ������ѭ�� 
					}
					
			if (flag == false)
			{
				//��t1�Ҳ�����Ӧ��  ��һ��t2Ҳ���ÿ��� ֱ������while 
				break;
			}

			//cout<<subs<<endl;
			s = s.substr(pos + 1, s.length() - pos);
		}
		return flag;
		//ȫ�ҵ�Ϊtrue  ��֮Ϊfalse
	}
	else
	{
		s = s.substr(1, s.length() - 1);//ȥ��{ 
		bool flag = false;//��ʼ״̬Ϊfalse  t1,t2,t3, ֻҪ����һ��t�Ҳ��� ��Ϊtrue ������(����ѭ��) 
		while (1)
		{
			int pos = s.find_first_of(",");
			if (pos < 0)break;
			string subs = s.substr(0, pos);//ȡ��һ��t1 ȥ�� ��Ǩ����Ƚ� �õ���� ��ȥisfirable������ ����Ƿ�ȫ������
			//bool flag=true;//���t1�Ƿ����ҵ� 

			
					int idex = ptnet->transid.at(subs);//�õ��˱�Ǩ�ı��
					//��ȥisfirabe�������Ƿ���������
					for (j = 0; j < rg->rgnode[statenum].enableNum; j++)
					{
						if (idex == rg->rgnode[statenum].isfirable[j])
						{
							break;
						}
					}
					if (j >= rg->rgnode[statenum].enableNum) {
						flag = true;//isfirable������û�ҵ� 
					}
					
			if (flag == true)
			{
				//��t1�Ҳ�����Ӧ��  ��һ��t2Ҳ���ÿ��� ֱ������while 
				break;
			}
			//cout<<subs<<endl;
			s = s.substr(pos + 1, s.length() - pos);
		}
		return flag;
		//ȫ�Ҳ���Ϊfalse  
	}
}
void Product_Automata::handleLTLCstep(int &front_sum, int &latter_sum, string s, int statenum)
{
	if (s[0] == 't')//��ǰ�벿��Ϊtoken-count��ʽ  
	{
		int pos = s.find_first_of("<=");//��λ��<=,ȡ��ǰ�벿��  
		string s_tokon = s.substr(12, pos - 13);//ȥ�� "token-count(" ")"  ֻʣp1,p2,    
		//cout<<" "<<s_tokon<<" ";
		front_sum = sumtoken(s_tokon, statenum);//����token�ĺ� 

		//�����벿��  
		s = s.substr(pos + 2, s.length() - pos - 2); //ȡ�ú�벿��  ���ǳ��� �������һ���ೣ������һ��'}'

		if (s[0] == 't')//����벿�ֻ���token-count 
		{
			string s_tokon = s.substr(12, s.length() - 14);//ȥ�� token-count(
		//	cout<<" "<<s_tokon<<" ";
			latter_sum = sumtoken(s_tokon, statenum);//��ͬ���� 

		}
		else//��벿���ǳ��� 
		{
			s = s.substr(0, s.length() - 1);//ȥ��}  ֻʣһ������ 
			latter_sum = atoi(s.c_str());
			//	 cout<<latter_sum;
		}

	}
	else//ǰ�벿���ǳ��� �Ǻ���һ����token-count��ʽ 
	{
		//����ǰ�벿��  ���еĴ� �Ǵӳ�����ʼ�� 
		int pos = s.find_first_of("<=");//��λ��<=,ȡ��ǰ�벿��
		string num = s.substr(0, pos);//ȡ������ 
		front_sum = atoi(num.c_str());
		//	 cout<< " "<<front_sum<<" ";

			 //�����벿��
		s = s.substr(pos + 14, s.length() - pos - 16);
		//	 cout<<" "<<s<<" ";
		latter_sum = sumtoken(s, statenum);
	}
}
bool Product_Automata::handleLTLC(string s, int statenum)//����C����  
{
	int front_sum, latter_sum;//ǰ�벿�ֺ� ��벿�ֵĺ� 
	if (s[0] == '!')//����Ϊ�� ��ȡ��
	{
		//	cout<<" �� ";
		s = s.substr(2, s.length() - 2);//ȥ�� "!{" 
	//	cout<<s;
		handleLTLCstep(front_sum, latter_sum, s, statenum);
		//ofe << "ǰ�벿�ֺͣ�" << front_sum << " ��벿�ֺͣ�" << latter_sum << endl;
		if (front_sum <= latter_sum)
			return true;
		else
			return false;

	}
	else
	{
		//	cout<<" �� ";
		s = s.substr(1, s.length() - 1);//ȥ�� "{"
	//	cout<<" "<<s;
		handleLTLCstep( front_sum, latter_sum, s, statenum);
		//ofe << "ǰ�벿�ֺͣ�" << front_sum << " ��벿�ֺͣ�" << latter_sum << endl;
		if (front_sum <= latter_sum)
			return false;
		else
			return true;
	}
}
/****************************Global_fucntions End*******************************/
bool timeflag;//�ж��Ƿ�ʱ 

void  sig_handler(int num)
{
    //printf("time out .\n");
    timeflag=false;
  
}


string to_String(int n)//������intת����string 
{
	int m = n;
	char s[max_to_string];
	char ss[max_to_string];
	int i = 0, j = 0;
	if (n < 0)// ������
	{
		m = 0 - m;
		j = 1;
		ss[0] = '-';
	}
	while (m > 0)
	{
		s[i++] = m % 10 + '0';
		m /= 10;
	}
	s[i] = '\0';
	i = i - 1;
	while (i >= 0)
	{
		ss[j++] = s[i--];
	}
	ss[j] = '\0';
	return ss;
}

void hashtable::insert(Product q)
{
	int idex = hashfunction(q.BAname_id+q.RGname_id);

	table[idex].push_back(q);
}

int hashtable::hashfunction(int   s)
{
	return (s) % hash_table_num;
}

int hashtable::search(Product  n)
{
	int idex = hashfunction(n.BAname_id+n.RGname_id);
	for (int i = 0; i < table[idex].size(); ++i)
	{
		if ( table[idex][i].BAname_id == n.BAname_id 
			&& table[idex][i].RGname_id == n.RGname_id)
		{
			//cout<< table[idex][i].first<<endl;
			return 1;
		}
	}
	//cout << "find not" << s << "  in hash" << endl;
	return -1;
}


void Product_Automata::getProduct()//����ɴ�ͼ���Զ��� 
{
	//recursive_count = 0;
	//recursive_count2 = 0;
	/*ofstream outfile("getproduct.txt", ios::out);
	ofstream ofe("islabel.txt", ios::out);
	outfile << endl;
	outfile << "���BA�ڵ��ϵ�label��\n";
	for (int i = 0; i < ba.svex_num; i++)
	{
		outfile << "�ڵ�" << i << "��label:" << ba.svertics[i].label << endl;
		outfile << "�ڵ�" << i << "��isaccept:" << ba.svertics[i].isAccept << endl;
		outfile << "�ڵ�" << i << "��isinitial:" << ba.svertics[i].isInitial << endl;


	}
	outfile << endl;*/
	rg->RGinitialnode();//���ɿɴ�ͼ�ĳ�ʼ״̬�Ľڵ㣨ֻ��һ���ڵ㣩
	addinitial_status();   //���ɽ��Զ����ĳ�ʼ״̬�ڵ�
	//cout << "��ʼ�ڵ������" << initial_status.size()<<endl;
	vector<Product>::iterator it_P;       //����ǰ������� 
	for (it_P = initial_status.begin(); it_P != initial_status.end(); it_P++)
	{
		//cout << "chushi:" << endl;
		dfs1((*it_P));
		
		if (!result || !timeflag)break;
	}
	//cout<<"dfs1递归层数："<<recursive_count<<endl;
	//cout<<"dfs2递归层数："<<recursive_count2<<endl;
}



void Product_Automata::dfs1(Product q)
{
	//recursive_count++;
	if(!timeflag){
		//recursive_count--;
		return ;
	}
	h.insert(q);//�����ϣ��
	stack.push_back(q);//����dfs1 �� stack��
	SArcNode *pba = new SArcNode;        //ָ��Ba�Զ������ڽӱ�Ľڵ� (��ͷ�ڵ�) 
	pba = ba->svertics[q.BAname_id].firstarc;
	while (pba != NULL)
	{
		//���ɿɴ�ͼ�ĺ�̽ڵ�
		for(int rg_i=0;rg_i< rg->rgnode[q.RGname_id].enableNum; rg_i++)//��ǰ�ڵ����еĿɷ����ı�Ǩ
		{
			int rgseednum = rg->ReachabilityTree(rg->rgnode[q.RGname_id], rg_i);		//�õ�rg��̽ڵ���rgnode�����е����
			if (isLabel(rgseednum, pba->adjvex))//petri�� �ɴ�ͼ �Զ��� �ɴ�ͼ��� �Զ������
			{

				Product qs;                                   //����һ��״̬�ڵ�N 
				qs.BAname_id = pba->adjvex;
				qs.RGname_id = rgseednum;
				if (h.search(qs) != 1)//
				{
					dfs1(qs);
				}
				if (ba->svertics[q.BAname_id].isAccept)//p�ɽ���
				{
					dfs2(qs);
				}
				if (!result || !timeflag)
				{
					//recursive_count--;
					return;
				}
			}
		}
		pba = pba->nextarc;
	}
	delete pba;
	stack.pop_back();//ɾ�����һ��  stackջ����վ;
	//recursive_count--;
}


void Product_Automata::dfs2(Product q)
{
	//recursive_count2++;
	if(!timeflag){
		//recursive_count2--;
		return ;
	}
	flag.push_back(q);//�������q���б��
	SArcNode *pba = new SArcNode;        //ָ��Ba�Զ������ڽӱ�Ľڵ� (��ͷ�ڵ�) 
	pba = ba->svertics[q.BAname_id].firstarc;
	while (pba != NULL )
	{
		for (int rg_i = 0; rg_i < rg->rgnode[q.RGname_id].enableNum; rg_i++)//��ǰ�ڵ����еĿɷ����ı�Ǩ
		{
			bool stackfound = false;
			bool flagfound = false;
			int rgseednum = rg->ReachabilityTree(rg->rgnode[q.RGname_id], rg_i);		//�õ�rg��̽ڵ���rgnode�����е����
			if (isLabel(rgseednum, pba->adjvex))
			{
				vector<Product>::iterator stackit;
				vector<Product>::iterator flagit;
				Product qs;                                   //����һ��״̬�ڵ�N 
				
				qs.BAname_id = pba->adjvex;
				qs.RGname_id = rgseednum;

				//�ж��Ƿ���stackջ��
				for (stackit = stack.begin(); stackit != stack.end(); stackit++)
				{
					if (stackit->BAname_id == qs.BAname_id && stackit->RGname_id == qs.RGname_id)
					{
						stackfound = true;
					}
				}
				//�ж��Ƿ���flag��
				for (flagit = flag.begin(); flagit != flag.end(); flagit++)
				{
					if (flagit->BAname_id == qs.BAname_id && flagit->RGname_id == qs.RGname_id)
					{
						flagfound = true;
					}
				}
				if (stackfound)
				{
					result = false;
					//recursive_count2--;
					return;
				}
				//qsδ�����
				else if (!flagfound)
				{
					dfs2(qs);
				}
			}
		}
		pba = pba->nextarc;
	}
    delete pba;
	//recursive_count2--;
}





void  Product_Automata::addinitial_status()//���ӳ�ʼ״̬ 
{

	for (int i = 0; i < ba->svex_num; i++)//�����Զ���ÿһ���ڵ�  i���Զ��� j�ǿɴ�ͼ
		if (ba->svertics[i].isInitial)//��i����Ŀɴ�ͼ�ڵ�  �� j����Ŀɴ�ͼ�ڵ�  ���Ǹ��ڵ㣨��ʼ״̬�ڵ㣩
		{
			int j = 0;
			if (isLabel(j, i))
			{
				Product N;                                   //����һ��״̬�ڵ�N 
				
				N.BAname_id = i;
				N.RGname_id = j;                            //���Զ��������+1 
				initial_status.push_back(N);
				//status.push_back(N);
			}

		}
}


bool Product_Automata::isLabel( int vi, int sj)//vi�ǿɴ�ͼ���±� sj���Զ������±� 
{

	string str = ba->svertics[sj].label;//�����Զ����ڵ�������
	if ("" == str)
		return true;//������Ϊ��  
	bool  mark = false; //mark����Ƿ���������� ��һֱ��false �ǽ����Ƿ�false
	while (1)
	{
		int pos = str.find_first_of("&&");
		//ofe << "RG���:" << vi << " BA���:" << sj << " BA��label:" << str << endl;
		if (pos == string::npos)//�������һ�� 
		{
			//cout<<str;
			if (judgeF(str))//F���� 
			{
				//cout<<"F����"; 
				mark = handleFLTLF(str, vi);
				if (mark == true)
				{
					break;//����whileѭ��  
				}
			}
			else {//c���� 
				//cout<<"c����";
				mark = handleLTLC( str, vi);
				if (mark == true)
				{
					break;//����whileѭ��  
				}
			}
			break;//��Ϊ�����һ�� ����ѭ�� 
		}

		string subprop = str.substr(0, pos);//ȡ��һ������ 
		//cout<<subprop<<" ";

		if (judgeF(subprop))//F���� 
		{
			//	cout<<"F����"; 
			mark = handleFLTLF(subprop, vi);
			if (mark == true)//���ҵ�һ������ ���� ����ѭ��   
			{
				break;
			}
		}
		else//c���� 
		{
			//	cout<<"c����"; 
			mark = handleLTLC(subprop, vi);
			if (mark == true)//���ҵ�һ������ ���� ����ѭ��   
			{
				break;
			}
		}
		//cout<<endl;
		str = str.substr(pos + 2, str.length() - pos -2);
	}

	if (mark == true)//��һ�����������
	{

		//ofe << "��Ϊfalse" << endl << endl;
		return false;
	}

	else //�������ⶼ������ markһֱ��false�� �����˷�false  
	{
		//ofe << "��Ϊ��false ���ɽڵ�" << endl << endl;
		return true;
	}

}



Product_Automata::Product_Automata()
{
	
}



void Product_Automata::ModelChecker(Petri *p, RG *g, SBA *s, string propertyid,int &timeleft)
{
    signal(SIGALRM, sig_handler);
    //cout<<"each_lefttime:"<<timeleft<<endl;
    alarm(timeleft);
    ptnet = p;
    rg = g;
    ba = s;
    timeflag=true;
    result = true;//���������ʼĬ��Ϊtrue
    rg->RGinitialnode();
    getProduct();
    //cout<<"ok \n";

    string re;
    if(timeflag)
    {
        if(result)
        {
            re="TRUE";

            cout << "FORMULA " + propertyid + " " + re +
                    " TECHNIQUES SEQUENTIAL EXPLICIT TOPOLOGICAL ABSTRACTIONS LTLNFBA" << endl;
            ret = 1;
        }
        else
        {
            re="FALSE";
            cout << "FORMULA " + propertyid + " " + re +
                    " TECHNIQUES SEQUENTIAL EXPLICIT TOPOLOGICAL ABSTRACTIONS LTLNFBA" << endl;
            ret = 0;
        }
    }
    else
    {
        cout<<"FORMULA "+propertyid+" "+"CANNOT_COMPUTE"<<endl;
        ret = -1;
    }

    timeleft = alarm(0);//Cancel alarm
    return;
}

int Product_Automata::getresult() {
	return ret;
}

Product_Automata::~Product_Automata()
{
    delete ba;
    delete rg;
}


