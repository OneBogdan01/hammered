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
    virtual void build(App& app) = 0;

    virtual bool ready(App& app) {
        return true;
    }
    virtual void finish(App& app) {}
    virtual void cleanup(App& app) {}
    virtual std::string_view name() const = 0;

    virtual bool is_unique() const {
        return true;
    }
};

} // namespace hm
