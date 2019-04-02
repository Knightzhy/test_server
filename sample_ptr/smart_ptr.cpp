#include <iostream>

template<class T>
class SmartPtr{
private:
    T* _ptr;
    int* _count;
    void SubCount();
public:
    SmartPtr(T *ptr = NULL); // constructor
    SmartPtr(const SmartPtr& ptr); // destructor
    void operator=(const SmartPtr& ptr);
    T operator*();
    int GetCount();
    ~SmartPtr();
};

template<class T>
SmartPtr<T>::SmartPtr(T *ptr) : _ptr(ptr)
{
    if (_ptr) {
        _count = new int(1);
    } else {
        _count = new int(0);
    }
}

template<class T>
SmartPtr<T>::SmartPtr(const SmartPtr& ptr)
{
    if (this != &ptr) {
        this->_ptr = ptr._ptr;
        this->_count = ptr._count;
        (*this->_count)++;
    }
}

template<class T>
void SmartPtr<T>::operator=(const SmartPtr& ptr)
{
    if (this->_ptr == ptr._ptr) {
        return;
    }
    if (this->_ptr) {
        this->SubCount();
    }
    this->_count = ptr._count;
    this->_ptr = ptr._ptr;
    (*this->_count)++;
}

template<class T>
T SmartPtr<T>::operator*()
{
    return *this->_ptr;
}

template<class T>
SmartPtr<T>::~SmartPtr()
{
    this->SubCount();
}

template<class T>
void SmartPtr<T>::SubCount()
{
    (*this->_count)--;
    if (*this->_count == 0) {
        std::cout << "del ptr, " << *this->_ptr << std::endl;
        delete this->_ptr;
        delete this->_count;
    }
}

template<class T>
int SmartPtr<T>::GetCount()
{
    return *this->_count;
}

int main()
{
    int *i = new int(10);
    SmartPtr<int> a(i);
    std::cout << "a.count=" << a.GetCount() << std::endl;
    std::cout << "===" << std::endl;
    SmartPtr<int> b = a;
    std::cout << "a.count=" << a.GetCount() << std::endl;
    std::cout << "b.count=" << b.GetCount() << std::endl;
    std::cout << "===" << std::endl;
    SmartPtr<int> c(b);
    std::cout << "a.count=" << a.GetCount() << std::endl;
    std::cout << "b.count=" << b.GetCount() << std::endl;
    std::cout << "c.count=" << c.GetCount() << std::endl;
    std::cout << "===" << std::endl;

    int *j = new int(11);
    SmartPtr<int> d(j);
    std::cout << "a.count=" << a.GetCount() << std::endl;
    std::cout << "b.count=" << b.GetCount() << std::endl;
    std::cout << "c.count=" << c.GetCount() << std::endl;
    std::cout << "d.count=" << d.GetCount() << std::endl;
    std::cout << "===" << std::endl;

    b = d;
    std::cout << "a.count=" << a.GetCount() << std::endl;
    std::cout << "b.count=" << b.GetCount() << std::endl;
    std::cout << "c.count=" << c.GetCount() << std::endl;
    std::cout << "d.count=" << d.GetCount() << std::endl;
    std::cout << "===" << std::endl;

    a = b;
    std::cout << "a.count=" << a.GetCount() << std::endl;
    std::cout << "b.count=" << b.GetCount() << std::endl;
    std::cout << "c.count=" << c.GetCount() << std::endl;
    std::cout << "d.count=" << d.GetCount() << std::endl;
    std::cout << "===" << std::endl;

    SmartPtr<int> *e = new SmartPtr<int>(c);
    std::cout << "a.count=" << a.GetCount() << std::endl;
    std::cout << "b.count=" << b.GetCount() << std::endl;
    std::cout << "c.count=" << c.GetCount() << std::endl;
    std::cout << "d.count=" << d.GetCount() << std::endl;
    std::cout << "e.count=" << e->GetCount() << std::endl;
    std::cout << "===" << std::endl;
    delete e;
    return 0;
}
