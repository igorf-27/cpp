using namespace std;


//��� ���� ����, ������� ��� ����� ���������
enum tokenType {
	UNKN,	//������������
	LB,	//	(
	RB,	//	)
	NUM,
	VAR,
	PLUS,
	MINUS,
	MUL,
	DIV,
	FEND	//����� �����
};

//��� ����� ��� ����, ����� ����� ���������� �� � ����
class Token{
	Token();
public:
	tokenType tType;	//��� �����
	Token(tokenType t)
		: tType(t) {}
	virtual double getNum(){}
	virtual char getVarSymb(){}	
};

//����� ����� �������� ������������ ���������,
//� ��� ����� ����������� ��������
class TokenNum : public Token{
	TokenNum();
	double n;	//����������� ��������
public:	
	TokenNum(double n) : Token(NUM) {
		this->n=n;
	}
	double getNum(){
		return n;
	}
};

class TokenVar : public Token{
	TokenVar();
	char c;	//������ ����������
public:	
	TokenVar(char c) : Token(VAR) {
		this->c=c;
	}
	char getVarSymb(){
		return c;
	}
};

//������ ��������� �����
Token* getToken(ifstream& inf){
	char c;
	do{
		c=inf.get();
		if(inf.eof())	//���� ���� ��������
			return new Token(FEND);
		}while(isspace(c));	//���� ������, �� ������ ������	

	tokenType t=UNKN;
	if (isdigit(c)){	//���� �����
		inf.putback(c);	//�� ������� � �����
		double n;
		inf>>n;	//� ����� ��� ���� ������ �����
		return new TokenNum(n);
	}
	if(isalpha(c))
		return new TokenVar(c);	//���������� - ���� ���������� ������
	if (c=='(' ) t=LB;
	if (c==')' ) t=RB;
	if (c=='+' ) t=PLUS;
	if (c=='-' ) t=MINUS;
	if (c=='*' ) t=MUL;
	if (c=='/' ) t=DIV;
	return new Token(t);	//���� ������ �� �����������, �� ������� UNKN
}

//���������� ��������� �������� ��� 0, ���� ��� �� ��������
int prior(Token* t){
	if (t->tType==PLUS||t->tType==MINUS)
		return 1;
	if (t->tType==MUL||t->tType==DIV)
		return 2;	
	return 0;
}

/* ������ ���������� � ����� ���� ����������, ��� ������ ���� ������:
 * ������� - "���������",
 * ����� "���������" - "����� ��� ���� ��������",
 * ����� "����� ��������" - "���������".
 * �������� ������ ���� �������������� ��������.
 */
enum Expectation{EXPR, OP_OR_END, OPER};

/* ������ ����, � ������� ��������:
 * �����,
 * ��� ����������,
 * ��� ��������, � ����� ����� �� ������� �� ���
 * - ������� ���� �������� � ����� �� �����,
 * - ����� ������.
 */
 
void makeString(ifstream& inf, Stack<Token*>& sSt){
	static Expectation expec=EXPR;
	
	Stack<Token*> sOp;	//�������������� ���� ��� ������ �������� � (
	Token* t;
	while((t=getToken(inf))->tType!=FEND){	//������, ���� �� ��������� ����� �����
		if(t->tType==UNKN){
			cout<<"Unknown symbol";
			throw;
		}
		if(expec==EXPR){
			switch (t->tType){
				case LB:	//����� ������ �� ��������������� ����
					sOp.push(t);
					expec=EXPR;
					break;				
				case NUM:
				case VAR:
					sSt.push(t);	//����� ��� ���������� � �������� "������"
					expec=OP_OR_END;
					break;
				//case RB:
				default:
					cout<<"Expression expected";
					throw;
			}
		}else{	//���� ����� �������� ��� �����
			switch (t->tType){
				case PLUS:
				case MINUS:
				case MUL:
				case DIV:				
					while(!sOp.isEmpty()	//����������� ����� �������� ������� ���������� � �������� "������"
						&& prior(sOp.get() ) >= prior(t) ){		//(prior ������ 0, ���� ��� �� ��������	)
							sSt.push(sOp.pop());
					}
					sOp.push(t);	//� ���� ���� �������� ��������� �� ��������������� �����, ���� �� ���������� ������ ��������
					expec=EXPR;
					break;
				//moved hither
				case RB:	// )
					while(!sOp.isEmpty()	//������� ��� � �������� ������, ���� �� ������ �� (
						&&(sOp.get()->tType)!=LB )
							sSt.push(sOp.pop());
					if(sOp.isEmpty() ){
						cout<<") has no (";
						throw;
					}
					sOp.pop();	//���������� (
					expec=OP_OR_END;
					break;

				default:
					cout<<"Operation sign expected";
					throw;
			}

		}
	}
	if(expec!=OP_OR_END){
		cout<<"Unexpected end";
		throw;
	}
	while(!sOp.isEmpty()){	//������� ���, ��� ��������
		Token* t=sOp.pop();
		if(t->tType==LB){	//����������� ������ �������������� ����� ������ �����������
			cout<<"( has no )";
			throw;
		}
		sSt.push(t);
	}
}
 

//������ Expression �� "������", ������� ������� makeString

Expression* makeExpr(Stack<Token*>& s){
	if(s.isEmpty())
		throw;
	Expression* e;
	Token* t=s.pop();
	tokenType ty=t->tType;
	if(ty==PLUS||ty==MINUS||ty==MUL||ty==DIV){
		Expression* r=makeExpr(s);
		Expression* l=makeExpr(s);
		if(ty==PLUS)
			return new Add(l,r);
		if(ty==MINUS)
			return new Sub(l,r);
		if(ty==MUL)
			return new Mul(l,r);
		if(ty==DIV)
			return new Div(l,r);
	}
	if(ty==NUM)
		return new Number(t->getNum());
	if(ty==VAR)
		return new Variable(t->getVarSymb());		
	throw;
}


Expression* readExp(ifstream& inf){
	Stack<Token*> s;
	makeString(inf, s);
	Expression* e=makeExpr(s);
	return e;
}
