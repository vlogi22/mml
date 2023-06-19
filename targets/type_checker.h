#ifndef __MML_TARGETS_TYPE_CHECKER_H__
#define __MML_TARGETS_TYPE_CHECKER_H__

#include "targets/basic_ast_visitor.h"
#include <cdk/types/reference_type.h>
#include <cdk/types/functional_type.h>

namespace mml {

  /**
   * Print nodes as XML elements to the output stream.
   */
  class type_checker: public  basic_ast_visitor {
    cdk::symbol_table<mml::symbol> &_symtab;
    basic_ast_visitor *_parent = nullptr;

  public:
    type_checker(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<mml::symbol> &symtab, 
        std::shared_ptr<mml::symbol> function, basic_ast_visitor *parent) :
        basic_ast_visitor(compiler), _symtab(symtab), _parent(parent) {
    }

  public:
    ~type_checker() {
      os().flush();
    }

  protected:
    template<typename T>
    void process_literal(cdk::literal_node<T> *const node, int lvl) {}

  protected:
    bool is_input_node(cdk::typed_node *const node);
    bool is_stack_alloc_node(cdk::typed_node *const node);
    bool is_null_node(cdk::typed_node *const node);
    void check_UnaryIntOnlyExpression(cdk::unary_operation_node *const node, int lvl);
    void check_BinaryIntOnlyExpression(cdk::binary_operation_node *const node, int lvl);
    void check_mulExpression(cdk::binary_operation_node *const node, int lvl);
    void check_addExpression(cdk::binary_operation_node *const node, int lvl);
    void check_subExpression(cdk::binary_operation_node *const node, int lvl);
    void check_generalLogicalBinaryExpression(cdk::binary_operation_node *const node, int lvl);
    void check_comparationExpression(cdk::binary_operation_node *const node, int lvl);
  
  protected:
    void process_assignment(cdk::typed_node *const lnode, cdk::typed_node *const rnode);
    void process_same_pointer_types(std::shared_ptr<cdk::basic_type> ltype, cdk::typed_node *const rnode);
    void same_pointer_types(std::shared_ptr<cdk::basic_type> lnode, std::shared_ptr<cdk::basic_type> rnode);
    void same_function_types(std::shared_ptr<cdk::functional_type> func_type1, std::shared_ptr<cdk::functional_type> func_type2);

  public:
    // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
    // do not edit these lines: end

  };

} // mml

//---------------------------------------------------------------------------
//     HELPER MACRO FOR TYPE CHECKING
//---------------------------------------------------------------------------

#define CHECK_TYPES(compiler, symtab, function, node) { \
  try { \
    mml::type_checker checker(compiler, symtab, function, this); \
    (node)->accept(&checker, 0); \
  } \
  catch (const std::string &problem) { \
    std::cerr << (node)->lineno() << ": " << problem << std::endl; \
    return; \
  } \
}

#define ASSERT_SAFE_EXPRESSIONS CHECK_TYPES(_compiler, _symtab, _function, node)

#endif
