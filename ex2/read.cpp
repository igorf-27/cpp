using namespace std;


//это типы слов, которые оно может разобрать
enum tokenType {
	UNKN,	//неопознанное
	LB,	//	(
	RB,	//	)
	NUM,
	VAR,
	PLUS,
	MINUS,
	MUL,
	DIV,
	FEND	//конец файла
};

//это класс для слов, чтобы потом складывать их в стек
class Token{
	Token();
public:
	tokenType tType;	//тип слова
	Token(tokenType t)
		: tType(t) {}
	virtual double getNum(){}
	virtual char getVarSymb(){}	
};

//числа будут читаться стандартными функциями,
//и тут будет прочитанное значение
class TokenNum : public Token{
	TokenNum();
	double n;	//прочитанное значение
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
	char c;	//символ переменной
public:	
	TokenVar(char c) : Token(VAR) {
		this->c=c;
	}
	char getVarSymb(){
		return c;
	}
};

//читает следующее слово
Token* getToken(ifstream& inf){
	char c;
	do{
		c=inf.get();
		if(inf.eof())	//если файл кончился
			return new Token(FEND);
		}while(isspace(c));	//если пробел, то читаем дальше	

	tokenType t=UNKN;
	if (isdigit(c)){	//если цифра
		inf.putback(c);	//то убираем её назад
		double n;
		inf>>n;	//и пусть они сами читают число
		return new TokenNum(n);
	}
	if(isalpha(c))
		return new TokenVar(c);	//переменная - один алфавитный символ
	if (c=='(' ) t=LB;
	if (c==')' ) t=RB;
	if (c=='+' ) t=PLUS;
	if (c=='-' ) t=MINUS;
	if (c=='*' ) t=MUL;
	if (c=='/' ) t=DIV;
	return new Token(t);	//если ничего не разобралось, то вернётся UNKN
}

//возвращает приоритет операции или 0, если это не операция
int prior(Token* t){
	if (t->tType==PLUS||t->tType==MINUS)
		return 1;
	if (t->tType==MUL||t->tType==DIV)
		return 2;	
	return 0;
}

/* Заведём переменную и будем туда записывать, что должно идти дальше:
 * сначала - "выражение",
 * после "выражения" - "конец или знак операции",
 * после "знака операции" - "выражение".
 * Парность скобок типа контролируется отдельно.
 */
enum Expectation{EXPR, OP_OR_END, OPER};

/* Делает стек, в котором записано:
 * число,
 * или переменная,
 * или операция, и тогда после от вершины ко дну
 * - сначала один аргумент в такой же форме,
 * - потом другой.
 */
 
void makeString(ifstream& inf, Stack<Token*>& sSt){
	static Expectation expec=EXPR;
	
	Stack<Token*> sOp;	//дополнительный стек для знаков операций и (
	Token* t;
	while((t=getToken(inf))->tType!=FEND){	//читаем, пока не достигнем конца файла
		if(t->tType==UNKN){
			cout<<"Unknown symbol";
			throw;
		}
		if(expec==EXPR){
			switch (t->tType){
				case LB:	//левую скобку во вспомогательный стек
					sOp.push(t);
					expec=EXPR;
					break;				
				case NUM:
				case VAR:
					sSt.push(t);	//число или переменную в выходную "строку"
					expec=OP_OR_END;
					break;
				//case RB:
				default:
					cout<<"Expression expected";
					throw;
			}
		}else{	//если ждали операцию или конец
			switch (t->tType){
				case PLUS:
				case MINUS:
				case MUL:
				case DIV:				
					while(!sOp.isEmpty()	//выталкиваем знаки операций высшего приоритета в выходную "строку"
						&& prior(sOp.get() ) >= prior(t) ){		//(prior вернет 0, если это не операция	)
							sSt.push(sOp.pop());
					}
					sOp.push(t);	//а знак этой операции сохраняем во вспомогательном стеке, пока не обработаем второй аргумент
					expec=EXPR;
					break;
				//moved hither
				case RB:	// )
					while(!sOp.isEmpty()	//достаем все в выходную строку, пока не дойдем до (
						&&(sOp.get()->tType)!=LB )
							sSt.push(sOp.pop());
					if(sOp.isEmpty() ){
						cout<<") has no (";
						throw;
					}
					sOp.pop();	//выкидываем (
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
	while(!sOp.isEmpty()){	//достаем все, что осталось
		Token* t=sOp.pop();
		if(t->tType==LB){	//открывающие скобки обрабатывались после чтения закрывающих
			cout<<"( has no )";
			throw;
		}
		sSt.push(t);
	}
}
 

//делаем Expression из "строки", которую сделала makeString

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
