#pragma once
#include <string_view>

namespace hm {

#define HM_PLUGIN(ClassName)                                                                       \
    std::string_view name() const override {                                                       \
        return #ClassName;                                                                         \
    }

class App;

class Plugin {
  public:
    virtual ~Plugin() = default;
    virtual void build(App&) = 0;

    virtual bool ready(App&) {
        return true;
    }
    virtual void finish(App&) {}
    virtual void cleanup(App&) {}
    virtual std::string_view name() const = 0;

};

} // namespace hm
