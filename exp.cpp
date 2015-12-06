class Expression;

class Env{
	std::map<std::string, std::stack<Expression*> > m;
	
public:	
	Env() {}
	Env(const Env& orig) {}
	Expression* fromEnv(std::string name);
	void add(std::string name, Expression* expr);
	void remove(std::string name);		
};




class Expression{

public:
	Expression() {}
	virtual ~Expression() {/*std::cout<<"exp"<<std::endl;*/}
	
	virtual void print(std::ostream& os)=0;
	virtual Expression* eval(Env& en)=0;
	Expression* eval();
	
	virtual int getValue(){ throw "exp: no value"; }
	
	virtual Expression* func_makeCall(Expression* arg_evaluated){ return NULL; }
	virtual void func_setEnv(Env&) {throw "exp: func_setEnv: not a function";}
	virtual bool isFunction(){return false;}
	
};

class Val : public Expression{
	int num;
public:
	Val(int n) :  num(n) {}
	void print(std::ostream& os);
	int getValue(){ return num; }
	Expression* eval(Env& en);
};

class Var : public Expression{
	std::string name;
public:
	Var(std::string name) :  name(name) {}
	
	void print(std::ostream& os);
	Expression* eval(Env& en);
};

class Add : public Expression{
	Expression* left;
	Expression* right;
public:
	Add(Expression* l, Expression* r) : left(l), right(r){}
	~Add() {delete left; delete right;}
	
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
		: cond1(c1), cond2(c2), e_then(th), e_else(el){}
	~Iff() {delete cond1; delete cond2; delete e_then; delete e_else;}
	void print(std::ostream& os);
	Expression* eval(Env& en);
};

class Let : public Expression{
	std::string name;
	Expression* value;
	Expression* body;
public:
	Let(std::string name, Expression* v, Expression* b)
		: name(name), value(v), body(b){}
	~Let() {delete value; delete body; /*std::cout<<"let"<<std::endl;*/}
	void print(std::ostream& os);
	Expression* eval(Env& en);
};



class Function : public Expression{
	std::string par;
	Expression* body;
	Env en;
public:
	Function(std::string p, Expression* b)
		: par(p), body(b){}	//environment will be put by eval
	~Function() {delete body;}
	void print(std::ostream& os);
	Expression* eval(Env& en);
	Expression* func_makeCall( Expression* arg_evaluated);
	void func_setEnv(Env&);
	bool isFunction() { return true; }

};



class Call : public Expression{
	Expression* fun;
	Expression* arg;
public:
	Call(Expression* f, Expression* a)
		: fun(f), arg(a){}
	~Call() {delete fun; delete arg;}
	void print(std::ostream& os);
	Expression* eval(Env& en);
};

class Set : public Expression{
	std::string name;
	Expression* value;

public:
	Set (std::string n, Expression* v)
		: name(n), value(v) {}
	~Set() {delete value;}
	void print(std::ostream& os);
	Expression* eval(Env& en);
};


class Block : public Expression{
	std::list<Expression*> li;
	Env en_block;

public:
	Block ( std::list<Expression*> l )
		: li(l) {}	//environment will be put by eval
	~Block();
	void print(std::ostream& os);
	Expression* eval(Env& en);
};


//--------------------------

Block::~Block(){
	std::list<Expression*>::iterator it;
	for ( it=this->li.begin(); it!=this->li.end(); ++it )
		delete *it;
}



Expression* Env::fromEnv(std::string name){
	std::map<std::string, std::stack<Expression*> >::iterator it;
	it=m.find(name);
	if (it!=m.end() && !(it->second.empty()) )
		return it->second.top();
	else
		return NULL;
}

void Env::add(std::string name, Expression* expr){
	
	std::map<std::string, std::stack<Expression*> >::iterator it;
	it=m.find(name);
	
	if (it==m.end()){
		std::stack<Expression*> stk;
		stk.push(expr);
		m.insert( std::pair<std::string, std::stack<Expression*> >(name, stk) );
	}else{
		it->second.push(expr);
	}
	return;
}


void Env::remove(std::string name){
	std::map<std::string, std::stack<Expression*> >::iterator it;
	it=m.find(name);
	if (it==m.end() || (it->second.empty()) ){
		throw "it undefines undefined variable!";
		//return;
	}
	it->second.pop();
	if (it->second.empty())
		m.erase(name);
}


//--------eval---------------

Expression* Expression::eval(){
	Env en;
	return this->eval(en);
}

Expression* Val::eval(Env& en){
	return this;
}

Expression* Var::eval(Env& en){
	Expression* e=en.fromEnv(this->name);
	if (e!=NULL)
		return e;
	throw "undefined";
}

Expression* Add::eval(Env& en){
	Expression* e = new Val(left->eval(en)->getValue() + right->eval(en)->getValue() );
	return e;
}


Expression* Iff::eval(Env& en){
	if ( cond1->eval(en)->getValue() > cond2->eval(en)->getValue() )
		return e_then->eval(en);
	else
		return e_else->eval(en);
}




Expression* Let::eval(Env& en){
	
	Expression* value_evaluated = value->eval(en);

	en.add(name, value_evaluated);
	if (value_evaluated->isFunction())	//если это была функци€
		value_evaluated->func_setEnv(en);	//то оно мен€ет еЄ, прицепл€€ ей текущий environment
		/*при этом у функции в environment'е
		станет определена переменна€, соответствующа€ этой самой функции
		(т. ч. рекурси€ будет работать), -
		потому что указатель на неЄ в еЄ environment'е
		указывает на то же самое, на что и здешний value_evaluated -
		т. е. мы сначала добавили переменную функции в наш environment,
		а потом прицепили этой функции наш environment*/
		
	Expression* e_ret=body->eval(en);
	
	en.remove(name);
	return e_ret;
}


Expression* Function::eval(Env& en){

	return this;
}

Expression* Call::eval(Env& en){
	Expression* fun_evaluated = fun->eval(en);
	if (!fun_evaluated->isFunction() ){
		throw "not a function";
	}
	
	Expression* arg_evaluated = arg->eval(en);
	Expression* e_ret = fun_evaluated->func_makeCall(arg_evaluated/*, en*/);
	
	return e_ret;
}


void Function::func_setEnv(Env& enToSet){
	this->en = enToSet;
}

Expression* Function::func_makeCall( Expression* arg_evaluated){

	
	this->en.add(this->par, arg_evaluated);
	Expression* e_ret = this->body->eval(this->en);
	this->en.remove(this->par);
	
	return e_ret;
}


Expression* Set::eval(Env& en){

	en.add(this->name, this->value->eval() );
	return this;
}

Expression* Block::eval(Env& en){
	this->en_block = en;
	
	std::list<Expression*>::iterator it;
	Expression* next=NULL;
	for ( it=this->li.begin(); it!=this->li.end(); ++it ){
		next=*it;
		next=next->eval(en_block);
	}
	if (next==NULL)
		next=this;
	return next;	//en_block dies
}

//-------print------------

void Val::print(std::ostream& os){
	os<<"(val "<<num<<")";
}

void Var::print(std::ostream& os){
	os << "(var " << name << ")";
}

void Add::print(std::ostream& os){
	os<<"(add ";
	left->print(os);
	os<<" ";
	right->print(os);
	os<<")";
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

void Let::print(std::ostream& os){

	os << "(let " << name << " = ";
	value->print(os);
	os<<" in ";
	body->print(os);
	os<<")";
}
void Function::print(std::ostream& os){

	os<<"(function " << par << " ";
	body->print(os);
	os<<")";
}
void Call::print(std::ostream& os){
	os<<"(call ";
	fun->print(os);
	os<<" ";
	arg->print(os);	
	os<<")";
}

void Set::print(std::ostream& os){
	os<<"(set "<<name<<' ';
	value->print(os);
	os<<")";
}

void Block::print(std::ostream& os){
	os<<"(block{ ";
	std::list<Expression*>::iterator it;
	for (it=this->li.begin(); it!=this->li.end(); it++){
		(*it)->print(os);
		os<<" ";
	}
	os<<"})";
}

