#ifndef __MML_AST_PROGRAM_H__
#define __MML_AST_PROGRAM_H__

#include <string>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include "ast/block_node.h"
#include "ast/function_definition_node.h"

namespace mml {

  class program_node: public mml::function_definition_node {

    public:
      program_node(int lineno, cdk::sequence_node *arguments, 
          std::shared_ptr<cdk::basic_type> funType, mml::block_node *block) :
          mml::function_definition_node(lineno, arguments, funType, block) {
      }

    public:
      void accept(basic_ast_visitor *sp, int level) {
        sp->do_program_node(this, level);
      }

  };

} // mml

#endif
