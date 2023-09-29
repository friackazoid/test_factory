// By http://www.nirfriedman.com/2018/04/29/unforgettable-factory/

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>

#include <vector>

#include <cxxabi.h>

std::string demangle(char const* name) {
    int status = -4;
    std::unique_ptr<char, void (*)(void *)> res{
        abi::__cxa_demangle(name, NULL, NULL, &status), free};
    return (status == 0) ? res.get() : name;
}

template <class Base, class... Args>
class RequestFactory {
public:
  template <class ... T>
  static std::unique_ptr<Base> make(std::string const& s, T&&... args) {
      return data().at(s)(std::forward<T>(args)...);
  }

  template <class T>
  struct Registrar : Base {
    friend T;

    static bool registerT() {
      const auto name = demangle(typeid(T).name());
      RequestFactory::data()[name] =
          [](Args... args) -> std::unique_ptr<Base> {
        return std::make_unique<T>(std::forward<Args>(args)...);
      };
      return true;
    }
    static bool registered;

  private:
    Registrar() : Base(Key{}) { (void)registered; };
  };

  friend Base;

private:
    class Key {
        Key(){};
        template <class T> friend struct Registrar;
    };

    using FuncType = std::unique_ptr<Base> (*)(Args...);
    RequestFactory() = default;

    static auto &data() {
        static std::unordered_map<std::string, FuncType> s;
        return s;
    }
};

template <class Base, class... Args>
template <class T>
bool RequestFactory<Base, Args...>::Registrar<T>::registered =
    RequestFactory<Base, Args...>::Registrar<T>::registerT();

struct RequestBase : RequestFactory<RequestBase, int> {
  RequestBase(Key) {}
  virtual void makeNoise() = 0;
  virtual ~RequestBase() = default;
};

class stop : public RequestBase::Registrar<stop> {
public:
  
  stop(int x) : m_x(x) {}

  void makeNoise() override { std::cout << "stop: " << m_x << "\n"; }

public:
  int m_x;
};

class move_ptp : public RequestBase::Registrar<move_ptp> {
public:
   move_ptp(int x) : m_x(x) {}
  //move_ptp(int x, int y) : m_x(x), m_y(y) {}
  //move_ptp(RequestHeader x, std::array<double, 6> q_array) : m_x(x), q_array_(q_array) {}

  void makeNoise() override { std::cout << "move_ptp: " << m_x << " " << m_y << "\n"; }

public:
  int m_x;
  int m_y;
  //std::array<double, 6> q_array_;
};

int main()
{
    std::vector<std::string> vec{"stop", "move_ptp"};

    auto serv = RequestBase::make("stop", 1);

    auto serv_ptp = RequestBase::make<int>("move_ptp", 1);
    serv_ptp->makeNoise();

    //auto serv2 = RequestBase::make("blah", 1);
}
