#include <iostream>
#include <fstream>

#include "stack.cpp"
#include "expr.cpp"
#include "read.cpp"

const char* inpath="input.txt";
const char* outpath="output.txt";

using namespace std;
				
int main()
{
	ifstream inf;
	inf.open(inpath,ios::in);
	Expression* e=readExp(inf);
	inf.close();
	
	ofstream outf;
	outf.open(outpath,ios::out);
	e->diff('x')->simplify()->print(outf);
	outf.close();
	
	return 0;
}
