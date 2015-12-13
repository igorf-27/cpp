/* ��� ������ �����. */

template <typename T>
class Stack{
	struct Node{
		Node* prev;
		T data;
		Node(Node* prev, T data)
			: prev(prev), data(data) {}		
	};
	Node* top;	//������� (��������� �� ������� ������)
public:
	Stack();
	~Stack();
	void push(T);
	T pop();
	T get();	//����������, ��� � �������
	bool isEmpty();
};

template <typename T>
Stack<T>::Stack(){
	top = 0;	//��� ���?
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
	T olddata=top->data;	//��������� ��, ��� �����
	Node* oldtop=top;	//��� ������ ������
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
