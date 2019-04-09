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
	m = 0;   //��������ָ��
	n = 0;    //��Ǩ����ָ��
	arcnum = 0;  //������ָ��
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
	for (TiXmlElement *petriElement = pageElement->FirstChildElement();//��һ����Ԫ��
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
			A2[arc[i].targetNum][arc[i].sourceNum]= arc[i].weight;//�������
		}
		else{
			A[arc[i].sourceNum][arc[i].targetNum]= arc[i].weight;
			A1[arc[i].sourceNum][arc[i].targetNum]= arc[i].weight;//�������
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
void Petri::exchange(double a[], double b[]) {//����Ԫ�ػ���
	double *temp;
	temp = new double[m];
	for (int i = 0; i < m; i++) {
		temp[i] = a[i];
		a[i] = b[i];
		b[i] = temp[i];
	}
}
void Petri::sub(double a[], double b[], double k) {//����:a[]=a[]-k*b[]
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
	double multiple;//�洢������ϵ
	int site = 0;//site������ڱ������к�
	for (int i = 0; (i < m)&&(site<n); i++) {//�������е��С�i������ڱ������к�
		if (B[site][i] == 0) {//������е�һ��Ԫ��Ϊ0������ѡһ����Ϊ��ķ�����ͬʱ���л���
			for (int j = site + 1; j < n; j++) {
				if (B[j][i] != 0) {
					exchange(B[j], B[site]);//site i
					break;
				}
			}
		}
		if (B[site][i] == 0) {//���в��ò����Ѿ�ȫ��Ϊ��,�򻯼���һ��
			continue;
		}
		else {//��������Ҫ������Ԫ�أ�����������
			for (int j = site + 1; j < n; j++) {
				multiple = B[j][i] / B[site][i];
				if (!((multiple - (int)multiple) < 1e-15 || (multiple - (int)multiple) < -0.999999999999999|| 
					-(multiple - (int)multiple) < 1e-15 || -(multiple - (int)multiple) < -0.999999999999999) )
				{//multiple ��������
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
	int signum = 0;//�洢��Ҫ�����ĸ���
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (B[i][j] != 0) {
				//Ѱ�ҵ�һ����Ϊ��ķ�������ͷ����������j����Ҫ��
				place[j].sig = true;
				signum++;
				break;
			}
		}
	}
	
	//��ӡsignificant��������
	//cout << "the num of significant places is:"<<signum << endl;
	/*��ӡ�����Ǿ���*/
	/*for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			cout << B[i][j] << "  ";
		}
		cout << endl;
	}
	*/

	/*
	������Է�����
	*/
	int sum = 0;
	int x = 0;//��ǰ����õĽ�ĸ���

	int story;//�洢��ǰ��������Ӧ�Ŀ���λ��

	for (int equ = 0; equ <m-signum; equ++) {//һ��Ҫ�⼸�η���?β�����м����ͽ⼸��
		int flag = 0;
		int *solution;
		solution = new int[m];
		for (int i = 0; i < m; i++) {//һά�����ʼ��
			solution[i] = -1;
		}
//		vector<int> solution(m, -1);
		for (int i = 0; i < m; i++) {//ע��flag=0����λ��
			if (place[i].sig == false) {//����β����
				if (flag == 0 && place[i].visited == false) {//����ǵ�һ��β��������ֵΪ1
					solution[i] = 1;
					story = i;//����һ��β����i��ֵΪ1������β������ֵΪ0���������������P��������Ӧ�ڿ���story
					flag = 1;
					place[i].visited = true;
					continue;
				}
				else {//����β������ֵΪ0
					solution[i] = 0;
				}
			}
		}
		sum = 0;
		for (int j = signum - 1; j >= 0; j--) {//���¶��Ͻⷽ�̣�j��Ƿ�����������
			int ele;
			for (int find = 0; find < m; find++) {//���е�һ������Ԫ�أ���Ҫ���ı���
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
	vector<int> M(ptnet.m, 0);//��ǰ�ڵ�
	//vector<int> M0(ptnet.m, 0);//�ɴ������ڵ㣬����ʼ��ʶ
	RGNode rgnode_temp;//��ʱ�ڵ㣬���ں�����rgnode����ӽڵ�

	//�ɴ�ͼ�ĵ�һ���ڵ㣨һ���ڵ㣺��ʼ��ʶ��
	for (int i = 0; i < ptnet.m; i++) 
	{
		rgnode_temp.m[i] = ptnet.place[i].initialMarking;//��ʱ�ڵ㸳ֵ
	}
	rgnode_temp.name = "M0";
	addRGNode(rgnode_temp);//����б������ӵ�һ���ڵ�
	node++;

		//��ѡ��ʶΪ�µĽڵ�newNode����ΪM
		for (int i = 0; i < ptnet.m; i++) 
		{
			M[i] = rgnode[0].m[i];
		}
		
		//rgnode[0].flag = 0;//�������ڵ�
		//if ��M�����б�Ǩ�����ܷ���
		bool enable;
		int enableNumber = 0;
		for (int i = 0; i < ptnet.n; i++)//n�Ǳ�Ǩ����Ĵ�С
		{
			//�������еı�Ǩ
			enable = true;
			for (int j = 0; j < ptnet.m; j++)
			{
				if (M[j] < ptnet.A2[i][j])
				{//��Ǩti���ܷ���
					enable = false;
					break;
				}
			}
			//��Ǩ��t0��ʼ���
			if (enable)
			{//��Ǩti���Է���
				rgnode[0].isfirable[enableNumber] = i;
				rgnode[0].enableNum++;
				enableNumber++;
			}
		}
	//rgnode_temp.freespace();
		return 0;
}
//���ɺ�̽ڵ�
int RG::ReachabilityTree( RGNode currentnode,int rg_i) {//PN���ɿɴ���
		vector<int> M(ptnet.m, 0);//��ǰ�ڵ�
		vector<int> M1(ptnet.m, 0);//��̽ڵ�
        //RGNode temp;
		//currentnode��ΪnewNode����ΪM
		for (int i = 0; i < ptnet.m; i++) 
		{
			M[i] = currentnode.m[i];
		}

			//if ��M�����б�Ǩ�����ܷ���

		

			//�ɷ�����Ǩti�ı��iΪhang
			//����M1
			for (int j = 0; j < ptnet.m; j++) 
			{
				int hang = currentnode.isfirable[rg_i];//���rg_i ��Ǩ
				if (ptnet.A[hang][j] == 100) {//�Ի�
					M1[j] = M[j];
				}
				else if (M[j] == 10000) {//�޽�
					M1[j] == 10000;
				}
				else {//��ͨ���
					M1[j] = M[j] + ptnet.A[hang][j];
				}
			}

			//���޽�
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
				//������б���ֹ�<=M1��
				if (unbound) {

					for (int pj = 0; pj < ptnet.m; pj++)
					{
						if (M1[pj] > rgnode[q].m[pj]) {
							M1[pj] = 10000;
						}
					}
				}
			}

			//if����б��г��ֹ�M1
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
				//������б���ֹ�M1
				if (repeated) 
				{
					return ii;//���غ�̽ڵ���±�
				}
			}

			//��û���ֹ�
			RGNode G;//G�ڵ����洢M1����Ϣ
			for (int i = 0; i < ptnet.m; i++) {
				G.m[i] = M1[i];
			}


			bool enable;
			int enableNumber = 0;
			//����M1(G)��isfirable���鸳ֵ
			for (int i = 0; i < ptnet.n; i++)//n�Ǳ�Ǩ����Ĵ�С
			{//�������еı�Ǩ
				enable = true;
				for (int j = 0; j < ptnet.m; j++)
				{
					if (M1[j] < ptnet.A2[i][j])
					{//��Ǩti���ܷ���
						enable = false;
						break;
					}
				}
				//��Ǩ��t0��ʼ���
				if (enable)
				{//��Ǩti���Է���
					G.isfirable[enableNumber] = i;
					G.enableNum++;
					enableNumber++;
				}
			}

			//��M1��û�г��ֹ�����״̬
			if (!repexist) {
				//�������½ڵ�M1
				G.name = "M" + itos(node);
				addRGNode(G);
				node++;
				//G.freespace();
				return (node-1);//���غ�̽ڵ���±�
			}
			//G.freespace();
}
//������׼�����ʽ
void RG::standardOutput() {
	cout << "STATE_SPACE " << "STATES " << node << endl;
	cout << "STATE_SPACE " << "TRANSITIONS " << ptnet.n << endl;
}

//�ɴ�ͼ���ɵ��ļ���
/*
void RG::PrintGraph( ofstream &outfile)
{
	outfile << "�ɴ�ͼ�ڵ����" << node << endl;
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
 * in: Petri ptnet, petri���� TiXmlElement *p, xml�ļ��ڵ�ָ��; placeInQuery, ��������
 * out: placeInQuery, ��������
 * �������ܣ��ݹ��ҳ����п���
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
 * in:ptnet,��ȡpnml�ļ������ɵ�petri��; filename, Ҫ������xml�ļ��� placeInQuery, ��������
 * out:placeInQuery
 * �������ܣ�����ReachabilityCardinality.xml�ļ�����ȡ���������п�����ÿ��ȡ��һ�����������ÿ���
 * ��placeInQuery�����е���Ӧλ����Ϊ1.
 * ע���ڵ��ô˺���ǰ������������placeInQuery��������ȫ����ʼ��Ϊ0��Ȼ���ٽ��δ������Բ�����ʽ������
 * ������������Զ����������е�ֵ
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

