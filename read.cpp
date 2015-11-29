enum TokenType{
	UNKN,	//������������
	
	BRA,	// (
	KET,	// )
	EQ,	// =
	INUM,	// ����� �����
	ID,	// �������������

	//�������� �����
	VAL,
	VAR,
	ADD,
	IFF,
	THENN,
	ELSEE,
	LET,
	IN,
	FUNC,
	CALL,

	
	FEND	//����� �����
};

class Token{
	Token();
public:
	TokenType tType;
	Token(TokenType t)
		: tType(t) {}
	//void print(){ std::cout<<tType<<std::endl; }
	virtual int getNum(){return 0;}
	virtual std::string getString() {return std::string("");}	
};



class TokenNum : public Token{
	TokenNum();
	int n;
public:	
	TokenNum(int n) : Token(INUM) {
		this->n=n;
	}
	int getNum(){
		return n;
	}
};


class TokenId : public Token{
	TokenId();
	std::string s;
public:	
	TokenId(std::string s) : Token(ID), s(s) {} 
	std::string getString(){
		return s;
	}
};



Token* getToken(std::istream& inf){
	unsigned char c;
	bool prevMinus=0;
	
	do{
		c=inf.get();
		if(inf.eof())
			return new Token(FEND);
		}while(isspace(c));	//���������� ������� � ����.
	
	if(c=='-'){	//������ � ������� ������ ������ �����
		c=inf.get();
		if(!isdigit(c)){
			inf.putback(c);
			return new Token(UNKN);
		}
		prevMinus=1;
	}
	

	if (isdigit(c)){
		inf.putback(c);

		if(prevMinus)
			inf.putback('-');
		int n;
		inf>>n;
		return new TokenNum(n);
	}
	

	//������������� ������� �� ����, ���� � _
	//� ���������� �� � �����
	if(isalpha(c) || c=='_'){
		
		std::string s;
		
		while (isalpha(c) || c=='_' || isdigit(c) ) {
			s+=c;
			c=inf.get();
		};
		
		if (! (isalpha(c) || c=='_' || isdigit(c) ) )
			inf.putback(c);
		
		
		if (s=="val") return new Token(VAL);
		if (s=="var") return new Token(VAR);
		if (s=="add") return new Token(ADD);
		if (s=="if") return new Token(IFF);
		if (s=="then") return new Token(THENN);
		if (s=="else") return new Token(ELSEE);
		if (s=="let") return new Token(LET);
		if (s=="in") return new Token(IN);
		if (s=="function") return new Token(FUNC);
		if (s=="call") return new Token(CALL);
			
		return new TokenId(s);
	}
	
	TokenType t=UNKN;
	
	if (c=='(' ) t=BRA;
	if (c==')' ) t=KET;
	if (c=='=' ) t=EQ;

	return new Token(t);
}



std::queue<Token*> getPhrase(std::istream& inf){
	std::queue<Token*> phrase;
	Token* t;
	do{
		t=getToken(inf);
		phrase.push(t);
		}
		while (t->tType != FEND);
	return phrase;
}
	

Expression* makeExpression(std::queue<Token*>& phrase){
	
	Token* t = phrase.front();
	phrase.pop();
	
	TokenType tt=t->tType;
	if (tt==FEND){
		throw "read: unexpected end";
	}
	if (tt!=BRA){
		throw "read: ( expected";
	}
	
	t=phrase.front();
	phrase.pop();
	tt=t->tType;
	
	Expression* e_ret;
	switch(tt){
		case FEND:
			throw "read: unexpected end";
			
		case UNKN:
			throw "read: unknown symbol";


		
		case VAL:
			t=phrase.front();
			phrase.pop();
			if (t->tType!=INUM){		
				throw "read: \"val\" must be followed by integer";
			}
			e_ret = new Val( t->getNum() );
			break;
		
		
		case VAR:
			t=phrase.front();
			phrase.pop();
			if (t->tType!=ID){
				throw "read: \"var\" must be followed by an identifier";
			}
			e_ret =  new Var(t->getString() );
			break;
		
		case ADD:{
			Expression* e1=makeExpression(phrase);
			Expression* e2=makeExpression(phrase);
	
			e_ret =  new Add(e1,e2);
			break;
		}
		case IFF:{
			Expression* cond1=makeExpression(phrase);
			Expression* cond2=makeExpression(phrase);
			
			t=phrase.front();
			if(t->tType==THENN)
				phrase.pop();
			else {
				//AFAIC there must be "then" and "else" notwithstanding the example
			}
			Expression* e_then=makeExpression(phrase);
			
			t=phrase.front();
			if(t->tType==ELSEE)
				phrase.pop();
			else {
				//AFAIC there must be "then" and "else" notwithstanding the example
			}
			Expression* e_else=makeExpression(phrase);
	
			e_ret =  new Iff(cond1, cond2, e_then, e_else);
			break;
		}
		case LET:{
			t=phrase.front();
			phrase.pop();
			if (t->tType!=ID){
				throw "read: \"let\" must be followed by an identifier";
			}
	
			std::string e_id=t->getString();
			
			t=phrase.front();
			phrase.pop();
			if (t->tType!=EQ){
				throw "read: \"let ...\" must be followed by =";
			}
	
			Expression* e_value=makeExpression(phrase);
			
			t=phrase.front();
			phrase.pop();
			if (t->tType!=IN){
				throw "read: \"let ... =\" must be followed by \"in\"";
			}
			
			Expression* e_body=makeExpression(phrase);
	
			e_ret =  new Let(e_id, e_value, e_body);
			break;
		}
		case FUNC:{
			t=phrase.front();
			phrase.pop();
			if (t->tType!=ID){
				throw "read: \"function\" must be followed by an identifier";
			}
			
	
			std::string e_id=t->getString();
			
			Expression* e_body=makeExpression(phrase);
			
			e_ret =  new Function(e_id, e_body);
	
			break;
		}
		
		case CALL:{
			
			Expression* e_func=makeExpression(phrase);
			Expression* e_arg=makeExpression(phrase);
	
			e_ret =  new Call(e_func, e_arg);
			break;
		}
		
		default:
			throw "read: wrong beginning after ( token";
	}
	
	t=phrase.front();
	phrase.pop();
	if (t->tType==KET){
		return e_ret;
	}
	throw "read: ) expected";
			
}
