#ifndef __MML_TARGETS_SYMBOL_H__
#define __MML_TARGETS_SYMBOL_H__

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace mml {

  class symbol {
    std::shared_ptr<cdk::basic_type> _type;
    std::string _name;
    int _offset; // 0 (zero) means global variable
    int _qualifier; // qualifiers: public, forward, "private" (i.e., none)
    bool _is_foreign;
    bool _is_main;

  public:
    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, int qualifier = 0) :
      _type(type), _name(name), _offset(0), _qualifier(qualifier), _is_foreign(false) {
    }

    virtual ~symbol() {
      // EMPTY
    }

    int qualifier() const {
      return _qualifier;
    }
    void set_type(std::shared_ptr<cdk::basic_type> t) {
      _type = t;
    }
    std::shared_ptr<cdk::basic_type> type() const {
      return _type;
    }
    bool is_typed(cdk::typename_type name) const {
      return _type->name() == name;
    }
    const std::string& name() const {
      return _name;
    }
    int offset() const {
      return _offset;
    }
    void set_offset(int offset) {
      _offset = offset;
    }

    bool is_global() const {
      return _offset == 0;
    }

    bool is_foreign() {
      return _is_foreign;
    }

    void set_foreign(bool v) {
      _is_foreign = v;
    }

    bool is_main() {
      return _is_main;
    }

    void set_main(bool v) {
      _is_main = v;
    }
  };

  inline auto make_symbol(std::shared_ptr<cdk::basic_type> type, 
    	const std::string &name, int qualifier) {
    return std::make_shared<symbol>(type, name, qualifier);
  }

} // mml

#endif
