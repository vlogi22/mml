#ifndef __MML_AST_FUNCTION_DEFINITION_H__
#define __MML_AST_FUNCTION_DEFINITION_H__

#include <string>
#include <cdk/types/functional_type.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include "ast/block_node.h"
#include "ast/declaration_node.h"

namespace mml {
  //!
  //! Class for describing function definitions.
  //! <pre>
  //! declaration: ...
  //!            {
  //!              $$ = new mml::function_definition(LINE, ...);
  //!            }
  //! </pre>
  //!
  class function_definition_node: public cdk::expression_node {
    cdk::sequence_node *_arguments;
    mml::block_node *_block;

  public:
    /**
     * Constructor for a function definition with arguments.
     */
    function_definition_node(int lineno, cdk::sequence_node *arguments, 
        std::shared_ptr<cdk::basic_type> funType, mml::block_node *block) :
        cdk::expression_node(lineno), _arguments(arguments), _block(block) {

      auto vec = new std::vector<std::shared_ptr<cdk::basic_type>>();

      if (arguments) {
        for (size_t i = 0; i < arguments->size(); i++) {
          std::shared_ptr<cdk::basic_type> t = (dynamic_cast<mml::declaration_node *>(arguments->node(i)))->type();
          vec->push_back(t);
        }
      }
  
      type(cdk::functional_type::create(*vec, funType));
    }

  public:
    cdk::sequence_node* arguments() {
      return _arguments;
    }

    cdk::typed_node* argument(size_t ax) {
      return dynamic_cast<cdk::typed_node*>(_arguments->node(ax));
    }

    mml::block_node* block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_definition_node(this, level);
    }

  };

} // mml

#endif
