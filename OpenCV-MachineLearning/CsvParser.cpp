
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "CsvParser.h"

using namespace std;

CsvParser::CsvParser(void)
{
	matrix.clear();
}
CsvParser::~CsvParser(void)
{
}

typedef vector< vector<string> >  SMatrix ;

void CsvParser::readMatrix(string fileName)
{
	string line;
	ifstream myfile (fileName);
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			if(line == "")continue;
			vector<string> rowLine = splitRow(line, ',');
			matrix.push_back(rowLine);
		}
		myfile.close();

	}
	else
		cout << "Unable to open file"; 
}

void CsvParser::writeMatrix(string fileName)
{
	ofstream myfile (fileName);
	if (myfile.is_open())
	{
		for(int i = 0 ; i < matrix.size() ; i++)
		{
			for(int j = 0 ; j < matrix[i].size() ; j++)
			{
				if(j)myfile<<",";
				myfile << matrix[i][j];
			}
			myfile << endl;
		}
	}
	else 
		cout << "Unable to open file to write";
}

vector<string> CsvParser::splitRow(string line, char delimiter)
{
	vector<string>ret;
	string cur = "";
	bool quote = false;
	for(int i=0 ; i<line.size() ; i++)
		if(quote==false && line[i]==delimiter)
		{
			ret.push_back(cur);
			cur = "";
		}
		else if(line[i]=='"')
			quote ^= 1;
		else
			cur += line[i];
		ret.push_back(cur);
		return ret;
}
double CsvParser::stringToNum(string str)
{
	stringstream ss;
	ss<<str;
	double ret;
	ss>>ret;
	return ret;
}
string CsvParser::numToString(double num)
{
	stringstream ss;
	ss<<num;
	string ret;
	ss>>ret;
	return ret;
}
void CsvParser::selectCol(vector<int>ColIndex)
{
	SMatrix newMat;
	int n = matrix.size();
	int m = ColIndex.size();
	newMat.resize(n);
	for(int i = 0 ; i< n ; i++)
	{
		newMat[i].resize(m);
		for(int j = 0 ; j<ColIndex.size() ; j++)
		{
			newMat[i][j] = matrix[i][ColIndex[j]];
		}
	}
	matrix = newMat;
}

int CsvParser::getRows()
{
	return matrix.size();
}
int CsvParser::getCols()
{
	if(matrix.size()>0)
		return matrix[0].size();
	return 0;
}
void CsvParser::replace(int colIndex , string pat,  string rep)
{
	for(int i = 0 ; i< getRows() ; i++)
		if(matrix[i][colIndex].find(pat)!=string::npos)
			matrix[i][colIndex]=rep;
}
