#include <iostream>

template<class T>
class SmartPtr;

template<class T>
class Point{
    friend class SmartPtr<T>;
private:
    T* _ptr;
    int _count;
    Point(T*ptr) {
        _ptr = ptr;
        if (_ptr) {
            _count = 1;
        } else {
            _count = 0;
        }
    }
    ~Point() {
        delete _ptr;
        _ptr = NULL;
    }
};

template<class T>
class SmartPtr {
public:
    SmartPtr(T* ptr);
    SmartPtr(const SmartPtr& s);
    SmartPtr& operator=(const SmartPtr<T>& s);
    T* operator&();
    T& operator*();
    T* operator->();
    int GetCount(){ return _p->_count;}
    ~SmartPtr();
private:
    Point<T> *_p;
};

template<class T>
SmartPtr<T>::SmartPtr(T* ptr = NULL) : _p(new Point<T>(ptr))
{
}

template<class T>
SmartPtr<T>::SmartPtr(const SmartPtr<T>& s)
{
    _p = s._p;
    _p->_count++;
}

template<class T>
SmartPtr<T>& SmartPtr<T>::operator=(const SmartPtr<T>& s)
{
    if (this !=&s) {
        _p->_count--;
        if (_p->_count == 0) {
            delete _p;
        }
        _p = s._p;
        _p->_count++;
    }
    return *this;
}

template<class T>
T* SmartPtr<T>::operator&()
{
    return _p->_ptr;
}

template<class T>
T* SmartPtr<T>::operator->()
{
    return _p->_ptr;
}

template<class T>
T& SmartPtr<T>::operator*()
{
    return *(_p->_ptr);
}

template<class T>
SmartPtr<T>::~SmartPtr()
{
    _p->_count--;
    if (_p->_count == 0) {
        delete _p;
    }
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
