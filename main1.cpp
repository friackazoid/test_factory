#include <map>
#include <memory>
#include <string>
#include <functional>
#include <any>
#include <string>
#include <iostream>


// Define a base class
class MyBase {
public:
    virtual void info() const =0; };

// Define derived classes with different constructors
class Derived1 : public MyBase {
public:
    explicit Derived1(int value) : value_(value) {}
    void info() const override {
        std::cout << "Derived1: " << value_ << std::endl;
    }
private:
    int value_;
};

class Derived2 : public MyBase {
public:
    explicit Derived2(const std::string& message) : message_(message) {}
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



int main () {
    std::map<std::string, AnyCallable<std::unique_ptr<MyBase>>> map;

    map["Derived1"] = std::function<std::unique_ptr<MyBase>(int)> ( [](int value) { return std::make_unique<Derived1>(value); });

    auto derived = map["Derived1"](1);

    derived->info();
    //std::cout << derived->info() << std::endl;
}
