#include <iostream>

class Base {
public:
    virtual void foo() {
        std::cout << "Base::foo()" << std::endl;
    }
};

class Derived1 : public virtual Base {
public:
    virtual void foo() {
        Base::foo();
        std::cout << "Derived1::foo()" << std::endl;
    }
};

class Derived2 : public virtual Base {
public:
    virtual void foo() {
        Base::foo();
        std::cout << "Derived2::foo()" << std::endl;
    }
};

class Final : public Derived1, public Derived2 {
public:
    using Base::foo;
    void foo() {
        Derived1::foo();
        Derived2::foo();
        std::cout << "Final::foo()" << std::endl;
    }
};

int main() {
    Final f;
    f.foo();
    return 0;
}