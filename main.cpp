#include"product.h"
#include"xml2ltl.h"
#include<fstream>
#include<sys/time.h>
#include<unistd.h>
#include<signal.h>
#include<pthread.h>
using namespace std;

int thread_count = 4;

double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1000000.0;
}

typedef struct thread_arg
{
	int threadid;
	Petri *ptnet;
}thread_arg_t;

void handler(int num)
{
	printf("rg time out .\n");
	exit(0);
}

void *LTLF_analysis(void *arg)
{
	int timeleft=0;
	char form[20000];     //store LTL formulae
	thread_arg_t *myarg = (thread_arg_t *)arg;
	int blocksize = 16 / thread_count;
	int start = myarg->threadid * blocksize;
	int end = myarg->threadid * blocksize + blocksize;
	int i;
	for(i = start; i<end && i<16; i++)
	{
		strcpy(form, fireability[i].formula_str.c_str());
		RG *graph = new RG(*(myarg->ptnet));

		Lexer *lex = new Lexer(form, fireability[i].formula_str.length());

		//syntax analysis
		Syntax_Tree *ST;
		ST = new Syntax_Tree;
		formula_stack Ustack;
		ST->reverse_polish(*lex);
		ST->build_tree();
		delete lex;
		//LTL formula rewrite
		ST->simplify_LTL(ST->root->left);

		//syntax tree convert
		ST->negconvert(ST->root->left, Ustack);

		//¹¹ÔìTGBA
		TGBA *Tgba;
		Tgba = new TGBA;
		Tgba->CreatTGBA(Ustack, ST->root->left);
		Tgba->SimplifyStates();
		delete ST;
		//¹¹ÔìTBA
		TBA *tba;
		tba = new TBA;
		tba->CreatTBA(*Tgba, Ustack);
		string filename = fireability[i].propertyid + ".txt";
		delete Tgba;
		//¹¹ÔìSBA
		SBA *sba;
		sba = new SBA;
		sba->CreatSBA(*tba);
		sba->Simplify();
		sba->Compress();
		delete tba;
		//cout << "begin:ON-THE-FLY" << endl;
		Product_Automata *product = new Product_Automata;
		product->ModelChecker(myarg->ptnet, graph, sba, fireability[i].propertyid,timeleft);
		fireability[i].result = product->getresult();
		delete product;
	}
}

void *LTLC_analysis(void *arg)
{
	int timeleft;
    thread_arg_t *myarg = (thread_arg_t *)arg;
    char form[20000];     //store LTL formulae
	int blocksize = 16 / thread_count;
	int start = myarg->threadid * blocksize;
	int end = myarg->threadid * blocksize + blocksize;
	int i;
    for(i = start; i<end && i<16; i++)
    {
        RG *graph = new RG(*(myarg->ptnet));
        strcpy(form, cardinality[i].formula_str.c_str());
        //lexer
        Lexer *lex = new Lexer(form, cardinality[i].formula_str.length());
        //syntax analysis
        Syntax_Tree *ST;
        ST = new Syntax_Tree;
        formula_stack Ustack;
        ST->reverse_polish(*lex);
        ST->build_tree();
        ST->simplify_LTL(ST->root->left);
		delete lex;
        //syntax tree convert
        ST->negconvert(ST->root->left, Ustack);

        //¹¹ÔìTGBA
        TGBA *Tgba;
        Tgba = new TGBA;
        Tgba->CreatTGBA(Ustack, ST->root->left);
        Tgba->SimplifyStates();
		delete  ST;
        //¹¹ÔìTBA
        TBA *tba;
        tba = new TBA;
        tba->CreatTBA(*Tgba, Ustack);
        string filename = cardinality[i].propertyid + ".txt";
		delete Tgba;
        //¹¹ÔìSBA
        SBA *sba;
        sba = new SBA;
        sba->CreatSBA(*tba);
        sba->Simplify();
        sba->Compress();
        delete tba;
        //cout << "begin:ON-THE-FLY" << endl;
        Product_Automata *product = new Product_Automata;
        product->ModelChecker(myarg->ptnet, graph, sba, cardinality[i].propertyid,timeleft);
		cardinality[i].result = product->getresult();
        delete product;
    }

}

int main0()
{
	double starttime, endtime;
    starttime = get_time();

	cout << "=================================================" << endl;
	cout << "=====This is our tool-enPAC for the MCC'2019=====" << endl;
	cout << "=================================================" << endl;

	//check if it is a color petri net
	string strcol;
	ifstream readcol("iscolored", ios::in);
	if (!readcol) { cout << "error!"; getchar(); exit(-1); }
	getline(readcol, strcol);
	if (strcol == "TRUE")
	{
		cout << "DO_NOT_COMPETE" << endl;
		return 0;
	}

    //parse xml files
	char Ffile[50] = "LTLFireability.xml";
	char Cfile[50] = "LTLCardinality.xml";
	convertf(Ffile);
	convertc(Cfile);

	//StateSpace
	ofstream outfile("reachablility.txt", ios::out);
	ofstream outprint("ptnet.txt", ios::out);
	char petrifile[20] = "model.pnml";

	//generate petri net
	Petri *ptnet = new Petri;
	ptnet->readPNML(petrifile);
	ptnet->getA();
	ptnet->UpperTriangularMatrix();
	ptnet->printA();

//	int *placeInQuery;
//	placeInQuery = new int[ptnet->m];
//	char filename[50]="ReachabilityCardinality.xml";
//	convertRC(*ptnet, filename, placeInQuery);

	/*------------------------LTLFireability----------------------*/
	//check LTLFireability
	ifstream read("LTLFireability.txt", ios::in);
	if (!read) { cout << "error!"; getchar(); exit(-1); }

	//import LTL formulae to global array fireability
	int i;
	for(i=0; i<16; i++)
	{
		getline(read, fireability[i].propertyid, ':');
		getline(read, fireability[i].formula_str);
	}

	pthread_t thread[thread_count];
	int j;
	//threads works on his own work
	for(j=0; j<thread_count; j++)
	{
		thread_arg_t *arg = new thread_arg_t;
		arg->threadid = j;
		arg->ptnet = ptnet;
		int ret = pthread_create(&thread[j], NULL, LTLF_analysis, (void *)arg);
		if(ret == -1)
			cout<<"Create fireability threads failed!"<<endl;
	}
	//wait for all threads exiting
	for(j=0; j<thread_count; j++)
	{
		pthread_join(thread[j], NULL);
	}//OK, here we get all results of LTLFireability formulae

	//print results
	ofstream outresult("boolresult.txt", ios::out);  //outresult export results to boolresult.txt
	for(i=0; i<16; i++)
	{
		if(fireability[i].result != -1) {
			string ret;
			ret = fireability[i].result ? "TRUE" : "FALSE";
			cout << "FORMULA " + fireability[i].propertyid + " " + ret +
					" TECHNIQUES SEQUENTIAL EXPLICIT TOPOLOGICAL ABSTRACTIONS LTLNFBA" << endl;
			outresult<<(fireability[i].result ? 'T' : 'F');

		}
		else{
			cout << "FORMULA " + fireability[i].propertyid + " " +
					"CANNOT_COMPUTE" << endl;
			outresult<<'?';
		}
	}
	outresult<<endl;

	/*-----------------------LTLCardinality-----------------------*/
	ifstream readF("LTLCardinality.txt", ios::in);
	if (!readF) { cout << "error!"; getchar(); exit(-1); }

	//import LTL Cardinbality formulae to global array cardinality
	for(i=0; i<16; i++)
	{
		getline(readF, cardinality[i].propertyid, ':');
		getline(readF, cardinality[i].formula_str);
	}

	//create threads
	pthread_t threadc[thread_count];
	for(j=0;j<thread_count;j++)
    {
	    thread_arg_t *arg = new thread_arg_t;
	    arg->ptnet = ptnet;
	    arg->threadid = j;
	    int ret = pthread_create(&threadc[j], NULL, LTLC_analysis, (void *)arg);
	    if(ret == -1)
			cout<<"Create cardinality threads failed!"<<endl;
    }
	//wait for threads exiting
	for(j=0; j<thread_count; j++)
	{
		pthread_join(threadc[j], NULL);
	}//OK, here we get all results of LTLFireability formulae

    //print results
    for(i=0; i<16; i++)
    {
        if(cardinality[i].result != -1)
        {
            string ret;
            ret = cardinality[i].result ? "TRUE" : "FALSE";
            cout << "FORMULA " + cardinality[i].propertyid + " " + ret +
                    " TECHNIQUES SEQUENTIAL EXPLICIT TOPOLOGICAL ABSTRACTIONS LTLNFBA" << endl;
            outresult<<(cardinality[i].result ? 'T' : 'F');
        }
        else {
            cout << "FORMULA " + cardinality[i].propertyid + " " +
                    "CANNOT_COMPUTE" << endl;
            outresult<<'?';
        }
    }

	endtime = get_time();
	cout<<"RUNTIME:"<<(endtime - starttime)<<"s"<<endl;
	return 0;
}

int main() {
    double starttime, endtime;
    starttime = get_time();

    //alarm
    int formula_num = 32;
    int totalruntime = 3600;
	int timeleft;
	cout << "=================================================" << endl;
	cout << "=====This is our tool-enPAC for the MCC'2019=====" << endl;
	cout << "=================================================" << endl;

	//check if it is a color petri net
	string strcol;
	ifstream readcol("iscolored", ios::in);
	if (!readcol) {
		cout << "error!";
		getchar();
		exit(-1);
	}
	getline(readcol, strcol);
	if (strcol == "TRUE") {
		cout << "DO_NOT_COMPETE" << endl;
		return 0;
	}

	//parse xml files
	char Ffile[50] = "LTLFireability.xml";
	char Cfile[50] = "LTLCardinality.xml";
	convertf(Ffile);
	convertc(Cfile);

	//StateSpace
	ofstream outfile("reachablility.txt", ios::out);
	ofstream outprint("ptnet.txt", ios::out);
	char petrifile[20] = "model.pnml";
	Petri *ptnet;
	//RG graph;
    ptnet = new Petri;
	ptnet->readPNML(petrifile);   //generate petri net
	ptnet->getA();
	ptnet->UpperTriangularMatrix();
	ptnet->printA();


	/*graph.ReachabilityTree(ptnet);
	graph.standardOutput(ptnet);
	graph.PrintGraph(ptnet, outfile);*/


	//LTLFireability check£º
	string S, propertyid; //propertyid stores names of LTL formulae
	char form[20000];     //store LTL formulae

	ofstream outresult("boolresult.txt", ios::out);  //outresult export results to boolresult.txt
	//int timeleft = alarm(0) / 32;

	//check LTLFireability
    ifstream read("LTLCardinality.txt", ios::in);
	if (!read) {
		cout << "error!";
		getchar();
		exit(-1);
	}

	while (getline(read, propertyid, ':')) {
		timeleft  = totalruntime/formula_num;
		int timetemp = timeleft;
		RG *graph = new RG(*ptnet);
		cout << propertyid << ':';
		getline(read, S);
		strcpy(form, S.c_str());
		//cout << form << endl;
		cout << endl;
		//lexer
		Lexer *lex = new Lexer(form, S.length());
		//syntax analysis
		Syntax_Tree *ST;
		ST = new Syntax_Tree;
		formula_stack Ustack;
		ST->reverse_polish(*lex);
		ST->build_tree();
		/*cout << "The syntax tree of unsimplified formula£º" << endl;
		ST.print_syntax_tree(ST.root, 0);*/
		//LTL formula rewrite
		ST->simplify_LTL(ST->root->left);
		/*cout << endl;
		cout << "The syntax tree of simplified formula£º" << endl;
		ST.print_syntax_tree(ST.root, 0);*/
		//syntax tree convert
		ST->negconvert(ST->root->left, Ustack);
		delete lex;
		/*cout << endl;
		cout << "The converted formula£º" << endl;
		cout << ST.root->left->formula << endl;
		cout << endl;*/
		//Êä³öU×ÓÊ½
		/*cout << "The subformulas of LTL whose main operator is \'U\'£º" << endl;
		vector<STNode>::iterator Uiter;
		for (Uiter = Ustack.loc.begin(); Uiter != Ustack.loc.end(); Uiter++)
		{
			cout << (*Uiter)->formula << endl;
		}
		cout << endl;*/
		//¹¹ÔìTGBA
		TGBA *Tgba;
		Tgba = new TGBA;
		Tgba->CreatTGBA(Ustack, ST->root->left);
		Tgba->SimplifyStates();
		delete ST;
		//cout << endl;
		//¹¹ÔìTBA
		TBA *tba;
		tba = new TBA;
		tba->CreatTBA(*Tgba, Ustack);
		delete Tgba;
		string filename = propertyid + ".txt";
		//tba.PrintBuchi(filename);
		/*cout << "Please check the file" + filename + ". In this file you can see the Buchi automata related to the LTL formula!";
		cout << endl;*/
		//¹¹ÔìSBA
		SBA *sba;
		sba = new SBA;
		sba->CreatSBA(*tba);
		sba->Simplify();
		sba->Compress();
		delete tba;
		//cout << "begin:ON-THE-FLY" << endl;
		Product_Automata *product = new Product_Automata;
		product->ModelChecker(ptnet, graph, sba, propertyid,timeleft);
		int ret = product->getresult();

		outresult << (ret==-1?'?':(ret == 0?'F':'T'));
		delete product;
		formula_num--;
		totalruntime = totalruntime - (timetemp - timeleft);
	}

	cout << endl << endl;

    ifstream readF("LTLFireability.txt", ios::in);
	if (!readF) {
		cout << "error!";
		getchar();
		exit(-1);
	}

	//cout<<"timeleft:"<<timeleft*16<<endl;
	outresult << endl;
	while (getline(readF, propertyid, ':')) {
		timeleft  = totalruntime/formula_num;
		int timetemp;
        RG *graph = new RG(*ptnet);
        cout << propertyid << ':';
        getline(readF, S);
        strcpy(form, S.c_str());
        //cout << form << endl;
        cout << endl;
        //lexer
        Lexer *lex = new Lexer(form, S.length());
        //syntax analysis
        Syntax_Tree *ST;
        ST = new Syntax_Tree;
        formula_stack Ustack;
        ST->reverse_polish(*lex);
        ST->build_tree();
        /*cout << "The syntax tree of unsimplified formula£º" << endl;
        ST.print_syntax_tree(ST.root, 0);*/
        //LTL formula rewrite
        ST->simplify_LTL(ST->root->left);
        /*cout << endl;
        cout << "The syntax tree of simplified formula£º" << endl;
        ST.print_syntax_tree(ST.root, 0);*/
        //syntax tree convert
        ST->negconvert(ST->root->left, Ustack);
        /*cout << endl;
        cout << "The converted formula£º" << endl;
        cout << ST.root->left->formula << endl;
        cout << endl;*/
        //Êä³öU×ÓÊ½
        /*cout << "The subformulas of LTL whose main operator is \'U\'£º" << endl;
        vector<STNode>::iterator Uiter;
        for (Uiter = Ustack.loc.begin(); Uiter != Ustack.loc.end(); Uiter++)
        {
            cout << (*Uiter)->formula << endl;
        }
        cout << endl;*/
        //¹¹ÔìTGBA
        TGBA *Tgba;
        Tgba = new TGBA;
        Tgba->CreatTGBA(Ustack, ST->root->left);
        Tgba->SimplifyStates();
        delete ST;
        //cout << endl;
        //¹¹ÔìTBA
        TBA *tba;
        tba = new TBA;
        tba->CreatTBA(*Tgba, Ustack);
        delete Tgba;
        string filename = propertyid + ".txt";
        //tba.PrintBuchi(filename);
        /*cout << "Please check the file" + filename + ". In this file you can see the Buchi automata related to the LTL formula!";
        cout << endl;*/
        //¹¹ÔìSBA
        SBA *sba;
        sba = new SBA;
        sba->CreatSBA(*tba);
        sba->Simplify();
        sba->Compress();
        delete tba;
        //cout << "begin:ON-THE-FLY" << endl;
        Product_Automata *product = new Product_Automata;
        product->ModelChecker(ptnet, graph, sba, propertyid,timeleft);
        int ret =product->getresult();
        outresult << (ret==-1?'?':(ret == 0?'F':'T'));
        delete product;
		formula_num--;
		totalruntime = totalruntime - (timetemp - timeleft);
	}
	delete ptnet;
	endtime = get_time();
	cout<<"RUNTIME:"<<(endtime - starttime)<<"s"<<endl;
	return 0;
}
