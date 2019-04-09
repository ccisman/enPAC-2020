#include"pnml_parse.h"

/**************************Data_Structure*******************************/
RGNode::RGNode() {
	m = new int[max_place_num];
	isfirable = new int[max_tran_num];
	memset(m,0,sizeof(int)*max_place_num);
	memset(isfirable,0, sizeof(int)*max_tran_num);
	enableNum = 0;
}
RGNode::~RGNode() {
//	delete [] m;
//	delete [] isfirable;
}
void RGNode::freespace()
{
	delete [] m;
	delete [] isfirable;
}
/**************************Data_Structure_End***************************/


/***********************************************************/
Petri::Petri()
{
	m = 0;   //库所数组指针
	n = 0;    //变迁数组指针
	arcnum = 0;  //弧数组指针
}

RG::RG(Petri net)
{
	ptnet = net;
	node = 0;
	edge = 0;
}
RG::RG()
{

}


void Petri::addPlace(Place p) {
	place.push_back(p);
}
void Petri::addTransition(Transition t) {
	transition.push_back(t);
}
void Petri::addArc(Arc a) {
	arc.push_back(a);
}
void Petri::readPNML(char *filename) {
	TiXmlDocument mydoc(filename);
	bool loadOK = mydoc.LoadFile();
	
	if (!loadOK) {
		cout << "could not load the test file" << endl;
		exit(1);
	}
	TiXmlElement *PetriElement = mydoc.RootElement();
	TiXmlElement *netElement = PetriElement->FirstChildElement();
	TiXmlElement *pageElement = netElement->FirstChildElement("page");
	for (TiXmlElement *petriElement = pageElement->FirstChildElement();//第一个子元素
		petriElement;
		petriElement = petriElement->NextSiblingElement())
	{
		if (petriElement->ValueTStr() == "place") {
			Place p;
			p.num = m;
			TiXmlAttribute *pnameElement = petriElement->FirstAttribute();
			p.name = pnameElement->Value();

			placeid.insert(make_pair(pnameElement->Value(), m));

			if (petriElement->FirstChildElement("initialMarking")) {
				TiXmlElement *initialMarkingElement = petriElement->FirstChildElement("initialMarking")->FirstChildElement("text");
				p.initialMarking = atoi(initialMarkingElement->GetText());
			}
		//	TiXmlElement *nameElement = petriElement->FirstChildElement("name")->FirstChildElement("text");
		//	p.name = nameElement->GetText();
			m++;
			place.push_back(p);
		}
		if (petriElement->ValueTStr() == "transition") {
			Transition t;
			t.num = n;
//			TiXmlElement *tnameElement = petriElement->FirstChildElement("name")->FirstChildElement("text");
//			t.name = tnameElement->GetText();
			
			TiXmlAttribute *tnameElement = petriElement->FirstAttribute();
			t.name = tnameElement->Value();

			transid.insert(make_pair(tnameElement->Value(), n));
			n++;
			transition.push_back(t);
		}
		if (petriElement->ValueTStr() == "arc") {
			Arc a;
			TiXmlAttribute *arcAttr = petriElement->FirstAttribute();
			a.id = arcAttr->Value();
			arcAttr = arcAttr->Next();
			a.source = arcAttr->Value();
			arcAttr = arcAttr->Next();
			a.target = arcAttr->Value();
			if (petriElement->FirstChildElement("inscription")) {
				TiXmlElement *weightElement = petriElement->FirstChildElement("inscription")->FirstChildElement("text");
				a.weight = atoi(weightElement->GetText());
			}
			arcnum++;
			arc.push_back(a);
		}
	}
	

	for (int i = 0; i < arcnum; i++) {
		if (placeid.count(arc[i].source)) {
			arc[i].sourceNum = placeid.at(arc[i].source);
			arc[i].sourceP = true;
		}
		else{
			arc[i].sourceNum = transid.at(arc[i].source);
			arc[i].sourceP = false;
		}
		if (placeid.count(arc[i].target)) {
			arc[i].targetNum = placeid.at(arc[i].target);
		}
		else {
			arc[i].targetNum = transid.at(arc[i].target);
		}
		
	}
}

void Petri::getA() {
	A.resize(n);
	for (int i = 0; i < n; i++) {
		A[i].resize(m, 0);
	}

	A1.resize(n);
	for (int i = 0; i < n; i++) {
		A1[i].resize(m, 0);
	}

	A2.resize(n);
	for (int i = 0; i < n; i++) {
		A2[i].resize(m, 0);
	}

	for (int i = 0; i < arcnum; i++) {
		if (arc[i].sourceP) {
			A[arc[i].targetNum][arc[i].sourceNum]=-arc[i].weight;
			A2[arc[i].targetNum][arc[i].sourceNum]= arc[i].weight;//输入矩阵
		}
		else{
			A[arc[i].sourceNum][arc[i].targetNum]= arc[i].weight;
			A1[arc[i].sourceNum][arc[i].targetNum]= arc[i].weight;//输出矩阵
		}
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if ((A1[i][j] == A2[i][j]) && (A1[i][j]!=0)) {
				A[i][j] = 100;
			}
		}
	}


}
void Petri::printA() {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			cout << A[i][j] << "  ";
		}
		cout << endl;
	}
}
void Petri::printA1() {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			cout << A1[i][j] << "  ";
		}
		cout << endl;
	}
}
string itos(int n)
{
	string val = to_string(n);
	return val;
}
void Petri::exchange(double a[], double b[]) {//两行元素互换
	double *temp;
	temp = new double[m];
	for (int i = 0; i < m; i++) {
		temp[i] = a[i];
		a[i] = b[i];
		b[i] = temp[i];
	}
}
void Petri::sub(double a[], double b[], double k) {//减法:a[]=a[]-k*b[]
	for (int i = 0; i < m; i++) {
		a[i] = a[i] - k * b[i];
	}
}

int sum(int m[], int n)
{
	int sum = 0;
	for (int i = 0; i < n; i++)
	{
		sum += m[i];
	}
	return sum;
}

void Petri::UpperTriangularMatrix() {
	
	double **B;
    B = new double*[n];
	for (int k = 0; k < n; k++)
	{
		B[k] = new double[m];
	}
	
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			B[i][j] = A[i][j];
		}
	} 
//	vector<vector<double>> B = A;
	double multiple;//存储倍数关系
	int site = 0;//site标记现在遍历的行号
	for (int i = 0; (i < m)&&(site<n); i++) {//对于所有的列。i标记现在遍历的列号
		if (B[site][i] == 0) {//如果该列第一个元素为0，则挑选一个不为零的分量，同时两行互换
			for (int j = site + 1; j < n; j++) {
				if (B[j][i] != 0) {
					exchange(B[j], B[site]);//site i
					break;
				}
			}
		}
		if (B[site][i] == 0) {//本列不用操作已经全部为零,则化简下一列
			continue;
		}
		else {//本列有需要操作的元素，则逐行相消
			for (int j = site + 1; j < n; j++) {
				multiple = B[j][i] / B[site][i];
				if (!((multiple - (int)multiple) < 1e-15 || (multiple - (int)multiple) < -0.999999999999999|| 
					-(multiple - (int)multiple) < 1e-15 || -(multiple - (int)multiple) < -0.999999999999999) )
				{//multiple 不是整数
					for (int k = 0; k < m; k++) {
						B[j][k] = B[j][k] * B[site][i];
					}
					multiple = B[j][i] / B[site][i];
				}
				sub(B[j], B[site], multiple);
			}
			site++;
		}
	}
	int signum = 0;//存储重要库所的个数
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (B[i][j] != 0) {
				//寻找第一个不为零的分量，即头变量。库所j是重要的
				place[j].sig = true;
				signum++;
				break;
			}
		}
	}
	
	//打印significant库所个数
	//cout << "the num of significant places is:"<<signum << endl;
	/*打印上三角矩阵*/
	/*for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			cout << B[i][j] << "  ";
		}
		cout << endl;
	}
	*/

	/*
	求解线性方程组
	*/
	int sum = 0;
	int x = 0;//当前已求得的解的个数

	int story;//存储当前不变量对应的库所位置

	for (int equ = 0; equ <m-signum; equ++) {//一共要解几次方程?尾变量有几个就解几次
		int flag = 0;
		int *solution;
		solution = new int[m];
		for (int i = 0; i < m; i++) {//一维数组初始化
			solution[i] = -1;
		}
//		vector<int> solution(m, -1);
		for (int i = 0; i < m; i++) {//注意flag=0语句的位置
			if (place[i].sig == false) {//对于尾变量
				if (flag == 0 && place[i].visited == false) {//如果是第一个尾变量，则赋值为1
					solution[i] = 1;
					story = i;//其中一个尾变量i赋值为1，其余尾变量赋值为0，则这样求出来的P不变量对应于库所story
					flag = 1;
					place[i].visited = true;
					continue;
				}
				else {//其余尾变量赋值为0
					solution[i] = 0;
				}
			}
		}
		sum = 0;
		for (int j = signum - 1; j >= 0; j--) {//从下而上解方程，j标记方程所在行数
			int ele;
			for (int find = 0; find < m; find++) {//该行第一个非零元素，即要求解的变量
				if (B[j][find] != 0) {
					ele = find;
					break;
				}
			}
			for (int k = ele + 1; k < m; k++) {
				sum += B[j][k] * solution[k];
			}
			solution[ele] = -sum / B[j][ele];
		}
		for (int i = 0; i < m; i++) {
			place[story].solution[i] = solution[i];
		}
	}
}
void RG::addRGNode(RGNode rg) {
	rgnode.push_back(rg);
}

int RG::RGinitialnode()
{
	vector<int> M(ptnet.m, 0);//当前节点
	//vector<int> M0(ptnet.m, 0);//可达树根节点，即初始标识
	RGNode rgnode_temp;//临时节点，用于后续向rgnode中添加节点

	//可达图的第一个节点（一个节点：初始标识）
	for (int i = 0; i < ptnet.m; i++) 
	{
		rgnode_temp.m[i] = ptnet.place[i].initialMarking;//临时节点赋值
	}
	rgnode_temp.name = "M0";
	addRGNode(rgnode_temp);//结点列表中增加第一个节点
	node++;

		//任选标识为新的节点newNode，设为M
		for (int i = 0; i < ptnet.m; i++) 
		{
			M[i] = rgnode[0].m[i];
		}
		
		//rgnode[0].flag = 0;//标记这个节点
		//if 在M下所有变迁都不能发生
		bool enable;
		int enableNumber = 0;
		for (int i = 0; i < ptnet.n; i++)//n是变迁数组的大小
		{
			//对于所有的变迁
			enable = true;
			for (int j = 0; j < ptnet.m; j++)
			{
				if (M[j] < ptnet.A2[i][j])
				{//变迁ti不能发生
					enable = false;
					break;
				}
			}
			//变迁从t0开始编号
			if (enable)
			{//变迁ti可以发生
				rgnode[0].isfirable[enableNumber] = i;
				rgnode[0].enableNum++;
				enableNumber++;
			}
		}
	//rgnode_temp.freespace();
		return 0;
}
//生成后继节点
int RG::ReachabilityTree( RGNode currentnode,int rg_i) {//PN生成可达树
		vector<int> M(ptnet.m, 0);//当前节点
		vector<int> M1(ptnet.m, 0);//后继节点
        //RGNode temp;
		//currentnode作为newNode，设为M
		for (int i = 0; i < ptnet.m; i++) 
		{
			M[i] = currentnode.m[i];
		}

			//if 在M下所有变迁都不能发生

		

			//可发生变迁ti的编号i为hang
			//计算M1
			for (int j = 0; j < ptnet.m; j++) 
			{
				int hang = currentnode.isfirable[rg_i];//针对rg_i 变迁
				if (ptnet.A[hang][j] == 100) {//自环
					M1[j] = M[j];
				}
				else if (M[j] == 10000) {//无界
					M1[j] == 10000;
				}
				else {//普通情况
					M1[j] = M[j] + ptnet.A[hang][j];
				}
			}

			//判无界
			int q = 0;
			bool unbound;
			for (q = 0; q < node; q++)
			{
				unbound = true;
				for (int pj = 0; pj < ptnet.m; pj++)
				{
					if (M1[pj] < rgnode[q].m[pj]) {
						unbound = false;
						break;
					}
				}
				//若结点列表出现过<=M1的
				if (unbound) {

					for (int pj = 0; pj < ptnet.m; pj++)
					{
						if (M1[pj] > rgnode[q].m[pj]) {
							M1[pj] = 10000;
						}
					}
				}
			}

			//if结点列表中出现过M1
			bool repeated;
			int ii;
			bool repexist = false;
			for (ii = 0; ii < node; ii++)
			{
				repeated = true;
				for (int jj = 0; jj < ptnet.m; jj++)
				{
					if (M1[jj] != rgnode[ii].m[jj]) {
						repeated = false;
						break;
					}
				}
				//若结点列表出现过M1
				if (repeated) 
				{
					return ii;//返回后继节点的下标
				}
			}

			//若没出现过
			RGNode G;//G节点来存储M1的信息
			for (int i = 0; i < ptnet.m; i++) {
				G.m[i] = M1[i];
			}


			bool enable;
			int enableNumber = 0;
			//对于M1(G)的isfirable数组赋值
			for (int i = 0; i < ptnet.n; i++)//n是变迁数组的大小
			{//对于所有的变迁
				enable = true;
				for (int j = 0; j < ptnet.m; j++)
				{
					if (M1[j] < ptnet.A2[i][j])
					{//变迁ti不能发生
						enable = false;
						break;
					}
				}
				//变迁从t0开始编号
				if (enable)
				{//变迁ti可以发生
					G.isfirable[enableNumber] = i;
					G.enableNum++;
					enableNumber++;
				}
			}

			//若M1是没有出现过的新状态
			if (!repexist) {
				//则引入新节点M1
				G.name = "M" + itos(node);
				addRGNode(G);
				node++;
				//G.freespace();
				return (node-1);//返回后继节点的下标
			}
			//G.freespace();
}
//比赛标准输出格式
void RG::standardOutput() {
	cout << "STATE_SPACE " << "STATES " << node << endl;
	cout << "STATE_SPACE " << "TRANSITIONS " << ptnet.n << endl;
}

//可达图生成到文件中
/*
void RG::PrintGraph( ofstream &outfile)
{
	outfile << "可达图节点个数" << node << endl;
	for (int i = 0; i < node; i++) {
		outfile << "M[" << i << "]" << "(";
		for (int j = 0; j < ptnet.m; j++) {
			outfile << rgnode[i].m[j] << " ";
		}
		outfile << ")";
		PRGEdge p;
		p = rgnode[i].firstEdge;
		for (int k = 0; k < rgnode[i].enableNum; k++) {
			outfile << "[t";
			outfile << p->t;
			outfile << "M";
			outfile << p->target;
			outfile << ",";
			p = p->nextedge;
		}
		outfile << endl;
	}
}
*/




void evaluate(int a[], int b[], int num) {
	for (int i = 0; i < num; i++) {
		a[i] = b[i];
	}
}

/*function:void GetRCPlace(Petri ptnet, TiXmlElement *p, int *placeInQuery)
 * in: Petri ptnet, petri网； TiXmlElement *p, xml文件节点指针; placeInQuery, 库索数组
 * out: placeInQuery, 库索数组
 * 函数功能：递归找出所有库所
 * */
void GetRCPlace(Petri ptnet, TiXmlElement *p, int *placeInQuery)
{
	TiXmlElement *ptr = p;
	if(ptr == NULL)
		return;
	string value = ptr->Value();
	if(value == "place")
	{
		string placename = ptr->GetText();
		int index = ptnet.placeid.at(placename);
		placeInQuery[index] = 1;
	}
	else{
		GetRCPlace(ptnet, ptr->FirstChildElement(), placeInQuery);
	}
	ptr=ptr->NextSiblingElement();
	if(ptr!=NULL)
	{
		GetRCPlace(ptnet, ptr, placeInQuery);
	}
}
/*function: void convertRC(Petri ptnet, char *filename, int *placeInQuery)
 * in:ptnet,读取pnml文件后生成的petri网; filename, 要解析的xml文件； placeInQuery, 库索数组
 * out:placeInQuery
 * 函数功能：解析ReachabilityCardinality.xml文件，提取出里面所有库所；每提取出一个库所，将该库所
 * 在placeInQuery数组中的相应位置设为1.
 * 注：在调用此函数前，先申请数组placeInQuery，并给它全部初始化为0，然后再将次此数组以参数形式传进来
 * 函数结束后会自动更新数组中的值
 * */
void convertRC(Petri ptnet, char *filename, int *placeInQuery) {
	TiXmlDocument *doc = new TiXmlDocument(filename);
	if (!doc->LoadFile()) {
		cerr << doc->ErrorDesc() << endl;
	}
	TiXmlElement *root = doc->RootElement();
	if (root == NULL) {
		cerr << "Failed to load file: no root element!" << endl;
		doc->Clear();
	}
	GetRCPlace(ptnet, root, placeInQuery);
}

