#include <cstdlib>
#include <iostream>
#include "SharedPtr.h"
struct Base {
    virtual ~Base() = default;
};

struct Derived : Base {};
int main() { 
    SharedPtr<int> ptr = makeShared<int>(5);
    std::cout << *ptr << std::endl;
    std::cout << ptr.get() << std::endl;
    ptr.reset();
    std::cout << (ptr.get() == NULL) << std::endl;

    ptr.reset(new int(25));
    std::cout << *ptr << std::endl;
    std::cout << ptr.get() << std::endl;

    SharedPtr<int> ptr2(new int(125));
    std::cout << *ptr2 << std::endl;
    std::cout << ptr2.get() << std::endl;

    ptr.swap(ptr2);
    std::cout << "swapped ptrs" << std::endl;
    std::cout << *ptr << std::endl;
    std::cout << ptr.get() << std::endl;
    std::cout << *ptr2 << std::endl;
    std::cout << ptr2.get() << std::endl;

    SharedPtr<int> mkunique = makeShared<int>(5);
    std::cout << *mkunique << std::endl;

    std::cout << "casting" << std::endl;
    SharedPtr<Derived> ptr5 = makeShared<Derived>();
    SharedPtr<Base> ptr6 = std::move(ptr5);
    return EXIT_SUCCESS;
}
