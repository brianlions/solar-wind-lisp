#include <iostream>

class Foo
{
protected:
    int _v;
public:
    Foo(int v = 0): _v(v) {
    }
    virtual ~Foo() {
    }
    virtual void set_value(int v) {
        _v = v;
    }
    virtual int get_value() const {
        return _v;
    }
    virtual void output() const {
        std::cout << "Foo::output() " << _v << std::endl;
    }
};

class Bar: public Foo
{
public:
    Bar(int v = 0): Foo(v) {
    }
    virtual ~Bar() {
    }
    virtual void output() const {
        std::cout << "Bar::output() " << _v << std::endl;
    }
};

std::ostream& operator<<(std::ostream &out, const Foo &foo)
{
    out << "value:" << foo.get_value();
    return out;
}

class ManagedPtr
{
private:
    Foo * _ptr;
    bool _is_owner;

    void remove_existing() {
        if (_ptr && _is_owner) {
            std::cout << "deleting ptr at " << ((void *) _ptr) << std::endl;
            delete _ptr;
            _ptr = NULL;
            _is_owner = false;
        }
    }

public:
    ManagedPtr(ManagedPtr const &b) {
        _ptr = b._ptr;
        _is_owner = false;
    }
#if 0
    ManagedPtr(ManagedPtr &b) {
        _ptr = b._ptr;
        _is_owner = false;
    }
#endif
    ManagedPtr & operator=(ManagedPtr const &b) {
        if (this != &b) {
            remove_existing();
            _ptr = b._ptr;
            _is_owner = false;
        }
        return *this;
    }
#if 0
    ManagedPtr & operator=(ManagedPtr &b) {
        if (this != &b) {
            remove_existing();
            _ptr = b._ptr;
            _is_owner = false;
        }
        return *this;
    }
#endif
    // Ctor
    ManagedPtr(Foo * p = NULL) {
        _ptr = p;
        _is_owner = p ? true : false;
    }
    // Dtor
    ~ManagedPtr() {
        remove_existing();
    }
    void reset(Foo * p) {
        if (p && p != _ptr) {
            remove_existing();
            _ptr = p;
            _is_owner = true;
        }
    }
    const Foo * operator->() const {
        return _ptr;
    }
    Foo * operator->() {
        return _ptr;
    }
    const Foo& operator*() const {
        return *_ptr;
    }
    Foo& operator*() {
        return *_ptr;
    }
};

void hello(ManagedPtr p);
void world(ManagedPtr p);
void hello(ManagedPtr p)
{
    std::cout << "in hello(): " << *p << std::endl;
    world(p);
}
void world(ManagedPtr p)
{
    std::cout << "in world(): " << *p << std::endl;
}

int main() {
    Foo * f = new Foo(5);
    Bar * b = new Bar(10);
    std::cout
        << "f at " << ((void *) f) << std::endl
        << "b at " << ((void *) b) << std::endl;

    ManagedPtr m;

    m.reset(f);
    std::cout << "value of f: " << m->get_value() << std::endl;
    std::cout << "m: " << *m << std::endl;
    m->set_value(6);
    std::cout << "value of f: " << m->get_value() << std::endl;
    std::cout << "m: " << *m << std::endl;

    std::cout << "-----" << std::endl;

    m.reset(b);
    std::cout << "value of b: " << m->get_value() << std::endl;
    std::cout << "m: " << *m << std::endl;
    m->set_value(12);
    std::cout << "value of b: " << m->get_value() << std::endl;
    std::cout << "m: " << *m << std::endl;

    std::cout << "-----" << std::endl;

    hello(m);

    ManagedPtr n;
    n = m;
    hello(n);

    return 0;
}
