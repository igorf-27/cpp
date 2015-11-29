#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <map>
#include <stack>

#include "exp.cpp"
#include "read.cpp"


int main(){
	std::ifstream inf;
	inf.open("input.txt", std::ios::in);
	
	std::queue<Token*> phrase = getPhrase(inf);
	Expression* e = makeExpression(phrase);

	//e->print(std::cout);
	//std::cout<<std::endl<<std::endl;
	
	//Env en;
	
	std::ofstream of;
	of.open("output.txt", std::ios::out);
	
	try{
		Expression* e_new = e->eval(/*en*/);
		e_new->print(of);
	} catch (const char*) {
		of<<"ERROR";
	}
		
	return 0;
}

