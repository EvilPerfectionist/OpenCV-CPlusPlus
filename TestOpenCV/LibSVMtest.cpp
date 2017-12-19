#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <list>
#include <fstream>
#include <iostream>
#include "svm.h"
using namespace std;

#define XLEN 10  //生产测试数据
#define YLEN 10


ofstream outdata;
ifstream indata;
ofstream outdata_lable;
ifstream indata_lable;

int NUM = 1440;

//char default_param[] = "-t 2 -c 100";
char default_param[] = "-t 2 -c 4 -g 32";
struct point {
	double *feature;
	//signed char value;
	int value;
};

list<point> point_list;
int current_value = 1;

void clear_all()
{
	point_list.clear();
}


void readFile2(char *file,char *file_lable)
{

	indata.open(file,ios::in);
	indata_lable.open(file_lable,ios::in);

	cout <<"read test data begin"<<endl;
	clear_all();
	while(!indata.eof())
	{
		
		double *line = new double[NUM];
		for(int i =0;i<NUM;i++)
		{
			indata >> line[i];
		}
		point p;
		indata_lable >>p.value;

		p.feature = line;
		point_list.push_back(p);
	}
	point_list.pop_back();
	cout <<"read test data end size = "<<point_list.size()<<endl;
	indata.close();
	indata_lable.close();
}


void testData()
{

	cout <<" test data begin"<<endl;

	svm_model *model = svm_load_model("modle.txt");
	svm_node *x_space = new svm_node[NUM+1];
	double d;
	int i;
	int k =0;
	for (list<point>::iterator q = point_list.begin(); q != point_list.end(); q++)
	{
		
		for(i =0 ;i<NUM;i++)
		{

			x_space[i].index = i+1;
			x_space[i].value = q->feature[i];

		}
		x_space[NUM].index = -1;


		d = svm_predict(model, x_space);
		if(d == q->value)
			cout <<"right = "<<d <<endl;
		else
		{
			cout<<"error"<<"   "<<d<<endl;
			k++;
		}
	}
	cout << "error times = "<<k <<endl;
	cout <<"read test data end"<<endl;
	delete [] x_space;
}
//1440
void readFile1(char *file,char *file_lable)
{

	indata.open(file,ios::in);
	indata_lable.open(file_lable,ios::in);
	cout <<"read data begin"<<endl;
	
	clear_all();
	while(!indata.eof())
	{
		
		double *line = new double[NUM];
		for(int i =0;i<NUM;i++)
		{
			indata >> line[i];
		}
		point p;
		indata_lable >>p.value;

		p.feature = line;
		point_list.push_back(p);
	}
	
	point_list.pop_back();
	indata.close();
	indata_lable.close();
	cout <<"read data end"<<endl;
}

/*void readFile( char * file)
{
	//char filename[1024];
	FILE *fp = fopen(file,"r");
	if(fp)
	{
		clear_all();

		char buf[4096];
		while(fgets(buf,sizeof(buf),fp))
		{
			int v;
			double x,y;
			if(sscanf(buf,"%d%*d:%lf%*d:%lf",&v,&x,&y)==3)
			{
				point p = {x,y,v};
				point_list.push_back(p);
			}
			else if(sscanf(buf,"%lf%*d:%lf",&y,&x)==2)
			{
				point p = {x,y,current_value};
				point_list.push_back(p);
			}
			else
				break;
		}
		fclose(fp);
	}
	else
	{
		printf("can't find file");
	}
}*/

void run()
{
	if(point_list.empty()) return;

	svm_parameter param;
	int i,j;
	
	// default values
	param.svm_type = C_SVC;
	param.kernel_type = RBF;
	param.degree = 3;
	param.gamma = 0;
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = 1;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;

	// parse options
	//char str[1024];
	//Edit_GetLine(edit, 0, str, sizeof(str));
	const char *p = default_param;//default_param;

	while (1) {
		while (*p && *p != '-')
			p++;

		if (*p == '\0')
			break;

		p++;
		switch (*p++) {
			case 's':
				param.svm_type = atoi(p);
				break;
			case 't':
				param.kernel_type = atoi(p);
				break;
			case 'd':
				param.degree = atoi(p);
				break;
			case 'g':
				param.gamma = atof(p);
				break;
			case 'r':
				param.coef0 = atof(p);
				break;
			case 'n':
				param.nu = atof(p);
				break;
			case 'm':
				param.cache_size = atof(p);
				break;
			case 'c':
				param.C = atof(p);
				break;
			case 'e':
				param.eps = atof(p);
				break;
			case 'p':
				param.p = atof(p);
				break;
			case 'h':
				param.shrinking = atoi(p);
				break;
			case 'b':
				param.probability = atoi(p);
				break;
			case 'w':
				++param.nr_weight;
				param.weight_label = (int *)realloc(param.weight_label,sizeof(int)*param.nr_weight);
				param.weight = (double *)realloc(param.weight,sizeof(double)*param.nr_weight);
				param.weight_label[param.nr_weight-1] = atoi(p);
				while(*p && !isspace(*p)) ++p;
				param.weight[param.nr_weight-1] = atof(p);
				break;
		}
	}
	
	// build problem
	svm_problem prob;

	prob.l = point_list.size();
	prob.y = new double[prob.l];

	if(param.kernel_type == PRECOMPUTED)
	{
	}
	else if(param.svm_type == EPSILON_SVR ||
		param.svm_type == NU_SVR)
	{
		if(param.gamma == 0) param.gamma = 1;
		svm_node *x_space = new svm_node[2 * prob.l];
		prob.x = new svm_node *[prob.l];

		i = 0;
		for (list<point>::iterator q = point_list.begin(); q != point_list.end(); q++, i++)
		{
			/*x_space[2 * i].index = 1;
			x_space[2 * i].value = q->x;
			x_space[2 * i + 1].index = -1;
			prob.x[i] = &x_space[2 * i];
			prob.y[i] = q->y;*/
		}

		// build model & classify
		svm_model *model = svm_train(&prob, &param);
		svm_node x[2];
		x[0].index = 1;
		x[1].index = -1;
		int *j = new int[XLEN];

		for (i = 0; i < XLEN; i++)
		{
			x[0].value = (double) i / XLEN;
			j[i] = (int)(YLEN*svm_predict(model, x));
		}
		
		//DrawLine(buffer_dc,0,0,0,YLEN,colors[0]);
		//DrawLine(window_dc,0,0,0,YLEN,colors[0]);
		
		int p = (int)(param.p * YLEN);
		for(int i=1; i < XLEN; i++)
		{
			//DrawLine(buffer_dc,i,0,i,YLEN,colors[0]);
			//DrawLine(window_dc,i,0,i,YLEN,colors[0]);
			
			//DrawLine(buffer_dc,i-1,j[i-1],i,j[i],colors[5]);
			//DrawLine(window_dc,i-1,j[i-1],i,j[i],colors[5]);

			if(param.svm_type == EPSILON_SVR)
			{			
				//DrawLine(buffer_dc,i-1,j[i-1]+p,i,j[i]+p,colors[2]);
				//DrawLine(window_dc,i-1,j[i-1]+p,i,j[i]+p,colors[2]);

				//DrawLine(buffer_dc,i-1,j[i-1]-p,i,j[i]-p,colors[2]);
				//DrawLine(window_dc,i-1,j[i-1]-p,i,j[i]-p,colors[2]);
			}
		}
		
		svm_free_and_destroy_model(&model);
		delete[] j;
		delete[] x_space;
		delete[] prob.x;
		delete[] prob.y;
	}
	else
	{
		if(param.gamma == 0) param.gamma = 0.5;
		
		cout <<"load data begin"<<endl;
		prob.x = new svm_node *[prob.l];
		int k =0;
		for (list<point>::iterator q = point_list.begin(); q != point_list.end(); q++,k++)
		{
			svm_node *x_space = new svm_node[NUM+1];
			for(i =0 ;i<NUM;i++)
			{
				
				x_space[i].index = i+1;
				x_space[i].value = q->feature[i];
			
			}
			x_space[NUM].index = -1;
			prob.x[k] = x_space;
			prob.y[k] = q->value;

		}
		cout <<"load data end"<<endl;
		//svm_node *x_space = new svm_node[3 * prob.l];
		//prob.x = new svm_node *[prob.l];
		/*i = 0;
		for (list<point>::iterator q = point_list.begin(); q != point_list.end(); q++, i++)
		{
			x_space[3 * i].index = 1;
			x_space[3 * i].value = q->x;
			x_space[3 * i + 1].index = 2;
			x_space[3 * i + 1].value = q->y;
			x_space[3 * i + 2].index = -1;
			prob.x[i] = &x_space[3 * i];
			prob.y[i] = q->value;
		}*/

		// build model & classify

		cout <<"train model begin"<<endl;
		svm_model *model = svm_train(&prob, &param);
		cout <<"train model end;"<<endl;


		///////////////////////////////////////////////////////////////////////////////////////
		/*readFile2("1.txt","1_lable.txt");

		svm_node *x_space = new svm_node[NUM+1];
		list<point>::iterator q1 = point_list.begin(); 
		for(i =0 ;i<NUM;i++)
		{

			x_space[i].index = i+1;
			x_space[i].value = q1->feature[i];

		}
		x_space[NUM].index = -1;


		double d = svm_predict(model, x_space);

		cout <<d <<endl;*/
		////////////////////////////////////////////////////////////////////////////////
		cout <<"save model begin"<<endl;
		svm_save_model("modle.txt", model);
		cout <<"save model end"<<endl;
		/*svm_node x[3];
		x[0].index = 1;
		x[1].index = 2;
		x[2].index = -1;
		for (i = 0; i < XLEN; i++)
			for (j = 0; j < YLEN; j++) {
				x[0].value = (double) i / XLEN;
				x[1].value = (double) j / YLEN;
				double d = svm_predict(model, x);
				printf("%f\n",d);
			}*/

		svm_free_and_destroy_model(&model);
		//delete[] x_space;
		/*for( i = 0 ;i < point_list.size();i++)
		{
			delete [] prob.x[i];
		}*/
		for( i = 0 ; i < int(point_list.size());i++)
		{
			delete [] prob.x[i];
		}
		delete[] prob.x;
		delete[] prob.y;
	}
	free(param.weight_label);
	free(param.weight);
	//draw_all_points();
}



int main()
{
	int choice;
	cout<<"1 train model\n2 test1\n3 test2\n"<<endl;
	cin >>choice;
	switch(choice)
	{
	case 1:
		{
			readFile1("traindata.txt","trainlabel.txt");
			run();
			break;
		}
	case 2:
		{
			readFile2("1.txt","1_label.txt");
			testData();
			break;
		}
	case 3:
		{
			readFile2("11.txt","11_label.txt");
			testData();
			break;
		}

	}
	//readFile("heart_scale");

	//run();

	/*indata.open("ab.txt",ios::in);
	int num;
	while(!indata.eof())
	{
		indata >> num;
		cout<<num <<endl;
	}
	indata.close();*/
	//ofile_buf_receive.open("e://buf_receive.txt",ios::out);
	//com_configure.open("e:/com.txt",ios::in);

	//readFile1("traindata.txt","trainlabel.txt");
	//run();
	//readFile2("2.txt","2_lable.txt");

	//testData();
	system("pause");
	return 0;
}