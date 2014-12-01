#include <iostream>
#include <memory>
class Foo {
private:
    int _f;
public:
    Foo(int f = 0): _f(f) {
    }
    virtual ~Foo() {
        std::cout << "dtor of foo" << std::endl;
    }
    int get_f() const {
        return _f;
    }
    virtual void output() const {
        std::cout << "in Foo: " << _f << std::endl;
    }
};

class Bar: public Foo {
private:
    int _b;
public:
    Bar(int f = 0, int b = 0): Foo(f), _b(b) {
    }
    virtual ~Bar() {
        std::cout << "dtor of bar" << std::endl;
    }
    int get_b() const {
        return _b;
    }
    virtual void output() const {
        std::cout << "in Bar: " << get_f() << "," << get_b() << std::endl;
    }
};

int main() {
    std::shared_ptr<Foo> f(new Foo(1));
    std::shared_ptr<Foo> b(new Bar(10,11));
    f->output();
    b->output();
    return 0;
}
