enum ExpType{_EXP, _VAL, _VAR, _ADD, _IFF, _LET, _FUNC, _CALL};

class Expression;

class Env{
	std::map<std::string, std::stack<Expression*> >* m;
public:	
	Env();
	Expression* fromEnv(std::string name);
	void add(std::string name, Expression* expr);
	void remove(std::string name);		
};


class Expression{
	ExpType eType;
public:
	Expression(ExpType t) : eType(t) {}
	virtual void print(std::ostream& os)=0;
	virtual Expression* eval(Env& en)=0;
	Expression* eval();
	
	virtual int getValue(){ throw "no value"; }
	
	ExpType getType(){ return eType; }
	
	virtual std::string func_getArgId(){ return std::string(""); }
	virtual Expression* func_getBody(){ return NULL; }
};

class Val : public Expression{
	int num;
public:
	Val(int n) : Expression(_VAL), num(n) {}
	void print(std::ostream& os);
	int getValue(){ return num; }
	Expression* eval(Env& en);
};

class Var : public Expression{
	std::string name;
public:
	Var(std::string name) : Expression(_VAR), name(name) {}
	
	void print(std::ostream& os);
	Expression* eval(Env& en);
};

class Add : public Expression{
	Expression* left;
	Expression* right;
public:
	Add(Expression* l, Expression* r) : Expression(_ADD), left(l), right(r){}
	void print(std::ostream& os);
	Expression* eval(Env& en);
};

class Iff : public Expression{
	Expression* cond1;
	Expression* cond2;
	Expression* e_then;
	Expression* e_else;
public:
	Iff(Expression* c1, Expression* c2, Expression* th, Expression* el)
		: Expression(_IFF), cond1(c1), cond2(c2), e_then(th), e_else(el){}
	void print(std::ostream& os);
	Expression* eval(Env& en);
};

class Let : public Expression{
	std::string name;
	Expression* value;
	Expression* body;
public:
	Let(std::string name, Expression* v, Expression* b)
		: Expression(_LET), name(name), value(v), body(b){}
	void print(std::ostream& os);
	Expression* eval(Env& en);
};



class Function : public Expression{
	std::string par;
	Expression* body;
public:
	Function(std::string p, Expression* b)
		: Expression(_FUNC), par(p), body(b){}
	void print(std::ostream& os);
	Expression* eval(Env& en);
	std::string func_getArgId(){ return par; }
	Expression* func_getBody(){ return body; }
};



class Call : public Expression{
	Expression* fun;
	Expression* arg;
public:
	Call(Expression* f, Expression* a)
		: Expression(_CALL), fun(f), arg(a){}
	void print(std::ostream& os);
	Expression* eval(Env& en);
};





Env::Env(){
	this->m = new std::map<std::string, std::stack<Expression*> >;
}

Expression* Env::fromEnv(std::string name){
	std::map<std::string, std::stack<Expression*> >::iterator it;
	it=m->find(name);
	if (it!=m->end() && !(it->second.empty()) )
		return it->second.top();
	else
		return NULL;
}

void Env::add(std::string name, Expression* expr){
	
	std::map<std::string, std::stack<Expression*> >::iterator it;
	it=m->find(name);
	
	if (it==m->end()){
		std::stack<Expression*> stk;
		stk.push(expr);
		m->insert( std::pair<std::string, std::stack<Expression*> >(name, stk) );
	}else{
		it->second.push(expr);
	}
	return;
}


void Env::remove(std::string name){
	std::map<std::string, std::stack<Expression*> >::iterator it;
	it=m->find(name);
	if (it==m->end() || (it->second.empty()) ){
		//throw "it undefines undefined variable!";
		return;
	}
	it->second.pop();
	if (it->second.empty())
		m->erase(name);
}



Expression* Expression::eval(){
	Env en;
	return this->eval(en);
}

void Val::print(std::ostream& os){
	os<<"(val "<<num<<")";
}

Expression* Val::eval(Env& en){
	return this;
}

void Var::print(std::ostream& os){
	os << "(var " << name << ")";
}

Expression* Var::eval(Env& en){
	Expression* e=en.fromEnv(this->name);
	if (e!=NULL)
		return e;
	throw "undefined";
}

void Add::print(std::ostream& os){
	os<<"(add ";
	left->print(os);
	os<<" ";
	right->print(os);
	os<<")";
}

Expression* Add::eval(Env& en){
	Expression* e = new Val(left->eval(en)->getValue() + right->eval(en)->getValue() );
	return e;
}

void Iff::print(std::ostream& os){
	os<<"(if ";
	cond1->print(os);
	os<<" ";
	cond2->print(os);
	os<<" ";
	e_then->print(os);
	os<<" ";
	e_else->print(os);	
	os<<")";	
}
Expression* Iff::eval(Env& en){
	if ( cond1->eval(en)->getValue() > cond2->eval(en)->getValue() )
		return e_then->eval(en);
	else
		return e_else->eval(en);
}

void Let::print(std::ostream& os){

	os << "(let " << name << " = ";// << value << " in " << body << ")";
	value->print(os);
	os<<" in ";
	body->print(os);
	os<<")";
}
Expression* Let::eval(Env& en){
	en.add(name, value->eval(en));
	Expression* e_ret = body->eval(en);
	en.remove(name);
	return e_ret;
}

void Function::print(std::ostream& os){

	os<<"(function " << par << " ";// << body << ")";
	body->print(os);
	os<<")";
}
Expression* Function::eval(Env& en){
	return this;
}

void Call::print(std::ostream& os){
	os<<"(call ";
	fun->print(os);
	os<<" ";
	arg->print(os);	
	os<<")";
}
Expression* Call::eval(Env& en){
	Expression* valfun=fun->eval(en);
	if (valfun->getType()!=_FUNC){
		throw "not a function";
	}
	std::string argId = ( valfun->func_getArgId() );
	en.add(argId , arg->eval(en) );
	Expression* e_ret=valfun->func_getBody()->eval(en);
	en.remove( argId );
	return e_ret;
}




/*int main(){
	Expression* n = new Val(17);
	
	return 0;
}*/

//typedef std::map<Id*, Expression*> env;

/*struct env{
	std::map<std::string, Expression*>* m;
	std::string fromEnv(std::string k){
		std::map<std::string, Expression*>::iterator it;
		it=m->find(k);
		if (it!=m->end() )
			return (*it).first;
		else
			return NULL;
	}
};*/

