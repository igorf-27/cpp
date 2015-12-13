using namespace std;

/*class Expression;
class Num;
class Var;
class Add;
class Mul;
class Sub;
class Div;*/


class Expression{
public:
	virtual void print(ostream& o)=0;
	virtual Expression* diff(char c)=0;

	virtual bool operator==(double x){ return false; }
	virtual bool operator==(char c){ return false; }
	virtual bool operator==(Expression* e){ return false; }
	virtual Expression* simplify()=0;
};

void printExp(Expression* l, Expression* r, char sg, ostream& o);

class Number : public Expression{
	double n;
public:
	Number(double n) : n(n) {}
	void print(ostream& o) {o << n;}
	Expression* diff(char c){
		return new Number(0);
	}
	bool operator==(double x){
		return x==this->n;
	}
	bool operator==(Expression* e){
		return *e==this->n;
	}
	Expression* simplify(){
		return this;
	}
};


class Variable : public Expression{
	char v;
public:
	Variable(char v) : v(v) {}
	void print(ostream& o) {o << v;}
	Expression* diff(char c){
		if (c==this->v)
			return new Number(1);
		else return new Number(0);
	}
	bool operator==(char c){
		return c==this->v;
	}
	bool operator==(Expression* e){
		return *e==this->v;
	}
	Expression* simplify(){
		return this;
	}
};

//class Mul;

class Add : public Expression{
	Expression* l;
	Expression* r;
public:
	Add(Expression* l, Expression* r) : l(l), r(r) {}
	~Add(){
		delete this->l;
		delete this->r;
	}
	void print(ostream& o){
		printExp(l, r, '+', o);
	}
	Expression* diff(char c){
		return new Add(l->diff(c), r->diff(c));
	}
	Expression* simplify();

};


class Sub : public Expression{
	Expression* l;
	Expression* r;
public:
	Sub(Expression* l, Expression* r) : l(l), r(r) {}
	~Sub(){
		delete this->l;
		delete this->r;
	}
	void print(ostream& o){
		printExp(l, r, '-', o);
	}
	Expression* diff(char c){
		return new Sub(l->diff(c), r->diff(c));
	}
	Expression* simplify();

};

class Mul : public Expression{
	Expression* l;
	Expression* r;
public:
	Mul(Expression* l, Expression* r) : l(l), r(r) {}
	~Mul(){
		delete this->l;
		delete this->r;
	}
	void print(ostream& o){
		printExp(l, r, '*', o);
	}
	Expression* diff(char c) {
		return new Add(
			new Mul(l->diff(c), r),
			new Mul(l,r->diff(c)) );
	}
	Expression* simplify();

};

class Div : public Expression{
	Expression* l;
	Expression* r;
public:
	Div(Expression* l, Expression* r) : l(l), r(r) {}
	~Div(){
		delete this->l;
		delete this->r;
	}
	void print(ostream& o){
		printExp(l, r, '/', o);
	}
	Expression* diff(char c){
		/*return new Div(
			new Sub(
				new Mul(l->diff(), r),
				new Mul(l, r->diff()) ),
			new Mul(r,r));*/
	
		return new Sub(
			new Div(l->diff(c), r),
			new Div(
				new Mul(l, r->diff(c)),
				new Mul(r,r)
				)
			);
	}
	Expression* simplify();

};

Expression* Add::simplify(){
	l=l->simplify();
	r=r->simplify();
	if (*l==(double)0)
		return r;
	if (*r==(double)0)
		return l;
	if (*l==r)
		return new Mul(new Number(2), l);
	return this;
}

Expression* Mul::simplify(){
	l=l->simplify();
	r=r->simplify();
	if (*l==(double)0 || *r==(double)0)
		return new Number(0);
	if (*l==(double)1)
		return r;
	if (*r==(double)1)
		return l;
	return this;
}

Expression* Sub::simplify(){

	l=l->simplify();
	r=r->simplify();
	if (*l==(double)0)
		return new Mul(new Number(-1),r);
	if (*r==(double)0)
		return l;
	return this;
}

Expression* Div::simplify(){

	l=l->simplify();
	r=r->simplify();
	if (*l==(double)0)
		return new Number(0);
	if (*r==(double)1)
		return l;
	if (*l==r)
		return new Number(1);
	return this;

}


void printExp(Expression* l, Expression* r, char sg, ostream& o){
	o<<'(';
	l->print(o);
	o << sg;
	r->print(o);
	o<<')';
}
