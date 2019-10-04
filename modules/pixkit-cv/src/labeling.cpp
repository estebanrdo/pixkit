#include "../include/pixkit-cv.hpp"

using namespace std;

bool pixkit::labeling::twoPass(const cv::Mat &src,cv::Mat &dst,const int offset){

	//////////////////////////////////////////////////////////////////////////
	///// EXCEPTION
	if(src.type()!=CV_8UC1){
		CV_Error(cv::Error::StsBadArg,"[xxx] src's type should be CV_8UC1.");
	}

	//���Ҭ��q1�}�l
	int LableNumber = 1;
	bool **Table = NULL;
	bool *assitT = NULL;
	bool *checkTable = NULL;
	int *refTable = NULL;
	int **result = NULL;
	int *resultT = NULL;

	int C[5],min,temp;
	int W,A,Q,E,S;

	dst = src.clone();
	//Mat convert to [][]
	result = new int *[src.rows];
	resultT = new int [src.rows*src.cols];
	memset(resultT,0,src.rows*src.cols);
	for(int i=0;i<src.rows;i++,resultT+=src.cols)
		result[i] = resultT;

	for(int i=0;i<src.rows;i++)
	{
		int temp = i*src.cols;
		for(int j=0;j<src.cols;j++)
		{
			result[i][j] = (int)src.data[temp+j];
		}
	}


	//���W�ƥμ���
	std::vector<int> ObjectIndex;

	//�Ĥ@�����y
	for(int i=0;i<src.rows;i++)
	{
		for(int j=0;j<src.cols;j++)
		{
			C[0] = result[i][j];
			if (C[0] <128)
				continue;
			/*�p�G C[0] > 128 �N������*/
			min = src.rows*src.cols;

			if(j-1 <0)
				C[1] = 0;
			else
				C[1] = result[i][j-1];
			if (i-1<0 || j-1 <0)
				C[2] = 0;
			else
				C[2] = result[i-1][j-1];
			if(i-1<0)
				C[3] = 0;
			else
				C[3] = result[i-1][j];
			if (i-1<0 || j+1 >=src.cols)
				C[4] = 0;
			else
				C[4] = result[i-1][j+1];


			if(C[1] ==0 && C[2] ==0 && C[3] ==0 && C[4] ==0)
			{
				C[0] = LableNumber;
				LableNumber++;
			}
			else
			{
				for(int k=1;k<=4;k++)
				{
					if(C[k]<min && C[k] != 0)
						min = C[k];
				}
				C[0] = min;
			}
			result[i][j] = C[0];
		}
	}


	//LableNumber != 1 �N���e������s�b
	//LableNumber == 1 �N��L�e������
	//�ϥΰʺA�s�WTable�O����
	if(LableNumber != 1)
	{
		Table = new bool *[LableNumber];
		assitT = new bool [LableNumber*LableNumber];
		memset(assitT,0,LableNumber*LableNumber);
		refTable = new int [LableNumber];
		checkTable = new bool [LableNumber];
		for(int i=0;i<LableNumber;i++,assitT+=LableNumber)
		{
			Table[i] = assitT;
			refTable[i] = 0;
			checkTable[i] = 0;
		}
	}

	//�ĤG�����y
	for(int i=0;i<src.rows;i++)
	{
		for(int j=0;j<src.cols;j++)
		{	
			S = result[i][j];
			if(S == 0)
				continue;

			if(i-1<0)
				W = 0;
			else
				W = result[i-1][j];
			if(i-1<0 || j-1<0)
				Q = 0;
			else
				Q = result[i-1][j-1];
			if(j-1<0)
				A = 0;
			else
				A = result[i][j-1];
			if (i-1<0 || j+1 >= src.cols)
				E = 0;
			else
				E = result[i-1][j+1];

			/*��Table��*/
			Table[S][S] = 1;
			if(W != 0)
			{
				Table[S][W] = 1;
				Table[W][S] = 1;
			}
			if(A!=0)
			{
				Table[S][A] = 1;
				Table[A][S] = 1;
			}
			if(Q!=0)
			{
				Table[S][Q] = 1;
				Table[Q][S] = 1;
			}
			if (E!=0)
			{
				Table[S][E] = 1;
				Table[E][S] = 1;
			}					
		}
	}
	//Equivalent Table
	//�]�����ҭȬO�q1�}�l�A�ҥHi,j�q1�}�l
	for(int i=1;i<LableNumber;i++)
	{
		for (int j=1;j<LableNumber;j++)
		{
			if(Table[i][j] ==1)
			{
				for(int ii = 1;ii<LableNumber;ii++)
					if(Table[j][ii] == 1)
					{
						Table[i][ii] = 1;
						Table[ii][i] = 1;
					}
			}
		}
	}

	//�إ�ref��Ӫ�
	for(int i=1;i<LableNumber;i++)
	{
		for (int j=1;j<LableNumber;j++)
		{
			if(Table[i][j] != 0)
			{
				refTable[i] = j;
				checkTable[j] = true;
				break;
			}
		}
	}


	//LableNumber != 1 �N���e������s�b
	for(int i=1;i<LableNumber;i++)
	{
		if(checkTable[i] == true)
		{
			ObjectIndex.push_back(i);
		}
	}

	//�Q�ι�Ӫ�hRefine�æP�ɱN���ҥ��W��
	int labelValue = 0,labelIndex = 0;
	for(int i=0;i<src.rows;i++)
	{
		for(int j=0;j<src.cols;j++)
		{
			temp = result[i][j];
			if(temp != 0)
			{
				labelValue = refTable[temp];
				for(int k=0;k<ObjectIndex.size();k++)
				{
					if(ObjectIndex[k] == labelValue)
					{
						result[i][j] = k+offset;
						break;
					}
				}
			}
		}
	}

	//[][] convert to Mat
	for(int i=0;i<src.rows;i++)
	{
		int temp = i*src.cols;
		for(int j=0;j<src.cols;j++)
		{
			dst.data[temp+j] = result[i][j];
		}
	}


	//LableNumber != 1 �N���e������s�b
	//�R��Table�O����
	if(LableNumber != 1)
	{
		delete []Table[0];
		delete []Table;
		delete []result[0];
		delete []result;
		delete []refTable;
		delete []checkTable;
	}

	return true;
}


