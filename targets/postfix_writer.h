#ifndef __MML_TARGETS_POSTFIX_WRITER_H__
#define __MML_TARGETS_POSTFIX_WRITER_H__

#include "targets/basic_ast_visitor.h"

#include <sstream>
#include <stack>
#include <set>
#include <iostream>
#include <cdk/emitters/basic_postfix_emitter.h>
#include <targets/frame_size_calculator.h>

namespace mml {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<mml::symbol> &_symtab;
    std::shared_ptr<mml::symbol> _function;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;

    
    bool _inFunctionBody = false;
    bool _inFunctionArgs = false;
    bool _returnSeen = false;
    int _offset = 0;
    std::vector<int> _whileCond, _whileEnd;

    cdk::typename_type _lvalueType;

    std::set<std::string> _foreign_functions;
    std::string _foreign_label;
    std::set<std::string> _to_BSS;
    std::vector<std::shared_ptr<mml::symbol>> _functions;
    std::vector<std::string> _return_labels;

    std::string _currentBodyRetLabel; // where to jump when a return occurs of an exclusive section ends
    std::string _currentBodyEnterLabel; // where to jump when a return occurs in the end section

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<mml::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

    void error(int lineno, std::string s) {
      std::cerr << "error: " << lineno << ": " << s << std::endl;
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // mml

#endif
