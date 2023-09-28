#include <iostream>
#include <string>

#include "factory.hpp"

#if 0

#include <map>
#include <memory>
#include <string>
#include <functional>
#include <any>


// Define a base class
class Base {
public:
    virtual void info() const = 0;
};

// Define derived classes with different constructors
class Derived1 : public Base {
public:
    explicit Derived1(int value) : value_(value) {}
    void info() const override {
        std::cout << "Derived1: " << value_ << std::endl;
    }
private:
    int value_;
};

class Derived2 : public Base {
public:
    Derived2(const std::string& message) : message_(message) {}
    void info() const override {
        std::cout << "Derived2: " << message_ << std::endl;
    }
private:
    std::string message_;
};


template <class TRet>
struct AnyCallable
{
    AnyCallable (){} 

    template <class F>
    AnyCallable(F&& fun) : m_any(std::function<TRet>(std::forward<F>(fun))) {}

    template<typename ... Args>
    AnyCallable(std::function<TRet(Args...)> fun) : m_any(fun) {}

    template <class... Args>
    TRet operator()(Args&&... args) const
    {
        return std::invoke(std::any_cast<std::function<TRet(Args...)>>(m_any), std::forward<Args>(args)...); 
    }

    std::any m_any;
};

template <class TBase>
class RequestFactory {
public:
    template <class TDerived, class... T>
    static void registerType(const std::string& typeName) {
        constructors[typeName] = [] (T&&... args) {
            return std::make_unique<TDerived>(std::forward<T>(args)...);
        };
    }

    template <class... T>
    static std::unique_ptr<TBase> create(const std::string& typeName, T&&... args) {
        auto it = constructors.find(typeName);
        if (it != constructors.end()) {
            return it->second(std::forward<T>(args)...);
        }
        throw std::runtime_error("Unknown type name");
    }

private:
    static std::map<std::string, AnyCallable<std::unique_ptr<TBase>>> constructors;
};

template <class TBase>
std::map<std::string, AnyCallable<std::unique_ptr<TBase>>> RequestFactory<TBase>::constructors;

#endif


int main() {
    // Register derived types with the factory
    RequestFactory<Base>::registerType<Derived1, int>("Derived1");
    RequestFactory<Base>::registerType<Derived2, std::string>("Derived2");
    RequestFactory<Base>::registerType<Derived3, int, int>("Derived3");
    RequestFactory<Base>::registerType<Derived4, std::array<double, 6>>("Derived4");

    //auto obj1 = RequestFactory<Base>::constructors["Derived1"](1);
    // Create objects using the factory
    auto obj1 = RequestFactory<Base>::create("Derived1", 1);
    auto obj2 = RequestFactory<Base>::create("Derived2", std::string("Hello world"));
    auto obj3 = RequestFactory<Base>::create("Derived3", 1, 2);
    auto obj4 = RequestFactory<Base>::create("Derived4", std::array<double, 6>{1,2,3,4,5,6});

    // Call methods on the created objects
    obj1->info();
    obj2->info();
    obj3->info();
    obj4->info();

    return 0;
}

