#pragma once

#include <array>
#include <map>
#include <memory>
#include <string>
#include <functional>
#include <any>
#include <iostream>


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
    explicit Derived2(const std::string& message) : message_(message) {}
    void info() const override {
        std::cout << "Derived2: " << message_ << std::endl;
    }
private:
    std::string message_;
};

class Derived3 : public Base {
public:
    explicit Derived3(int x, int y) : x_(x), y_(y) {}
    void info() const override {
        std::cout << "Derived2: x = " << x_ << "; y = " << y_ << std::endl;
    }
private:
    int x_;
    int y_;
};

class Derived4 : public Base {
public:
    explicit Derived4(std::array<double, 6> const& q) : q_(q) {}
    void info() const override {
        std::cout << "Derived4: q_ = " << q_[0] << ", " << q_[1] << ", " << q_[2] << ", " << q_[3] << ", " << q_[4] << ", " << q_[5] << std::endl;
    }
private:
    std::array<double, 6> q_;
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
        constructors[typeName] = std::function<std::unique_ptr<TBase>(T...)>( [] (T&&... args) {
            return std::make_unique<TDerived>(std::forward<T>(args)...);
        });
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


