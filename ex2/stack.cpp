/* Тут шаблон стека. */

template <typename T>
class Stack{
	struct Node{
		Node* prev;
		T data;
		Node(Node* prev, T data)
			: prev(prev), data(data) {}		
	};
	Node* top;	//вершина (последняя по времени запись)
public:
	Stack();
	~Stack();
	void push(T);
	T pop();
	T get();	//посмотреть, что в вершине
	bool isEmpty();
};

template <typename T>
Stack<T>::Stack(){
	top = 0;	//или как?
}

template <typename T>
bool Stack<T>::isEmpty(){
	return((this->top)==0);
	}

template <typename T>
void Stack<T>::push(T data){
	Node* nw= new Node(this->top,data);
	this->top=nw;
}

template <typename T>
T Stack<T>::pop(){
	if ( this->isEmpty() )
		throw;
	T olddata=top->data;	//сохраняем то, что вернём
	Node* oldtop=top;	//эту запись удалим
	this->top=top->prev;
	delete oldtop;
	return olddata;
}

template <typename T>
T Stack<T>::get(){
	if ( this->isEmpty() )
		throw;
	return this->top->data;
}


template <typename T>
Stack<T>::~Stack(){
	while(!this->isEmpty())
		this->pop();	
}	
/*int testStack(){
	Stack<int> s;
	s.push(17);
	s.push(19);
	s.push(27);
	s.push(11);
	cout<<s.pop();
	cout<<s.pop();
	cout<<s.pop();
	cout<<s.pop();
	cout<<" privet";
	
	return 0;
	}*/
