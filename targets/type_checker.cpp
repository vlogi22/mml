#include <string>
#include "targets/type_checker.h"
#include ".auto/all_nodes.h"  // automatically generated
#include <cdk/types/primitive_type.h>
#include "mml_parser.tab.h"

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

bool mml::type_checker::is_input_node(cdk::typed_node *const node) {
  return (!dynamic_cast<mml::input_node *>(node)) ? true : false;
}

bool mml::type_checker::is_stack_alloc_node(cdk::typed_node *const node) {
  return dynamic_cast<mml::stack_alloc_node *>(node) != nullptr;
}

bool mml::type_checker::is_null_node(cdk::typed_node *const node) {
  return dynamic_cast<mml::null_node *>(node) != nullptr;
}

//----------bad names ;-; + disgusting code: i'm too lazy to refactor this **** code-----:O

void mml::type_checker::check_BinaryIntOnlyExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  if (node->left()->is_typed(cdk::TYPE_UNSPEC)) { // checking INPUT
    if (!is_input_node(node->left())) {
      throw std::string("integer expression expected in binary operator (left)");
    }
  }

  if (node->left()->is_typed(cdk::TYPE_UNSPEC)) {
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (!node->left()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in binary operator (left)");
  }

  node->right()->accept(this, lvl + 2);
  if (node->right()->is_typed(cdk::TYPE_UNSPEC)) { // checking INPUT
    if (!is_input_node(node->right())) {
      throw std::string("integer expression expected in binary operator (right)");
    }
  }

  if (node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (!node->right()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in binary operator (right)");
  }

  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void mml::type_checker::check_UnaryIntOnlyExpression(cdk::unary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) { // checking INPUT
    if (!is_input_node(node->argument())) {
      throw std::string("integer expression expected in binary operator (left)");
    }
  }

  if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
    node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (!node->argument()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in unary operator.");
  }

  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void mml::type_checker::check_mulExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_UNSPEC)) {
    if (!is_input_node(node->left())) {
      throw std::string("integer expression expected in binary operator (left)");
    }
  } else if (node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    if (!is_input_node(node->right())) {
      throw std::string("integer expression expected in binary operator (right)");
    }
  }

  // Input(left)
  if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->left()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->type(node->right()->type());
  // Input(right)
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->right()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->type(node->left()->type());
  // Input(both)
  } else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  // Double
  } else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  // Integer
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else {
    throw std::string("wrong types in binary expression");
  }
}

void mml::type_checker::check_addExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (!node->left()->is_typed(cdk::TYPE_INT) && 
      !node->left()->is_typed(cdk::TYPE_DOUBLE) && 
      !node->left()->is_typed(cdk::TYPE_POINTER) && 
      !node->left()->is_typed(cdk::TYPE_UNSPEC))
    throw std::string("integer, double or pointer expression expected in ADD operator (left)");

  if (!node->right()->is_typed(cdk::TYPE_INT) && 
      !node->right()->is_typed(cdk::TYPE_DOUBLE) && 
      !node->right()->is_typed(cdk::TYPE_POINTER) && 
      !node->right()->is_typed(cdk::TYPE_UNSPEC))
    throw std::string("integer, double or pointer expression expected in ADD operator (right)");

  // Input(left)
  if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->left()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->type(node->right()->type());
  // Input(right)
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->right()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->type(node->left()->type());
  // Input(both)
  } else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  // Double
  } else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  // Pointer
  } else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->left()->type());
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    node->type(node->right()->type());
  // Integer
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else {
    throw std::string("invalid types in ADD operator");
  }
}

void mml::type_checker::check_subExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (!node->left()->is_typed(cdk::TYPE_INT) && 
      !node->left()->is_typed(cdk::TYPE_DOUBLE) && 
      !node->left()->is_typed(cdk::TYPE_POINTER) && 
      !node->left()->is_typed(cdk::TYPE_UNSPEC))
    throw std::string("integer, double or pointer expression expected in SUB operator (left)");

  if (!node->right()->is_typed(cdk::TYPE_INT) && 
      !node->right()->is_typed(cdk::TYPE_DOUBLE) && 
      !node->right()->is_typed(cdk::TYPE_POINTER) && 
      !node->right()->is_typed(cdk::TYPE_UNSPEC))
    throw std::string("integer, double or pointer expression expected in SUB operator (right)");

  // Input(left)
  if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->left()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->type(node->right()->type());
  // Input(right)
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->right()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->type(node->left()->type());
  // Input(both)
  } else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  // Double
  } else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  // Pointer
  } else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->left()->type());
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    node->type(node->right()->type());
  } else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    node->type(node->left()->type());
  // Integer
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else {
    throw std::string("invalid types in SUB operator");
  }
}

void mml::type_checker::check_generalLogicalBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  // Input(left)
  if (node->left()->is_typed(cdk::TYPE_UNSPEC) && 
      (node->right()->is_typed(cdk::TYPE_INT) || node->right()->is_typed(cdk::TYPE_DOUBLE))) {
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  // Input(right)
  } else if ((node->left()->is_typed(cdk::TYPE_INT) || node->left()->is_typed(cdk::TYPE_DOUBLE)) && 
      node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  // Input(both)
  } else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  // Pointer
  } else if ((node->left()->is_typed(cdk::TYPE_POINTER) && !node->right()->is_typed(cdk::TYPE_POINTER)) ||
      (!node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER))) {
    throw std::string("pointer can only be compared with other pointer.");
  }
  
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void mml::type_checker::check_comparationExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  
  node->left()->accept(this, lvl + 2);
  if (node->left()->is_typed(cdk::TYPE_UNSPEC)) { // checking INPUT
    if (!is_input_node(node->left())) {
      throw std::string("integer expression expected in binary operator (left)");
    }
  } 
  
  if (node->left()->is_typed(cdk::TYPE_UNSPEC)) {
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (!node->left()->is_typed(cdk::TYPE_INT) && !node->left()->is_typed(cdk::TYPE_DOUBLE)) {
    throw std::string("integer or double expression expected in binary logical expression (left)");
  }

  node->right()->accept(this, lvl + 2);
  if (node->right()->is_typed(cdk::TYPE_UNSPEC)) { // checking INPUT
    if (!is_input_node(node->right())) {
      throw std::string("integer expression expected in binary operator (right)");
    }
  }

  if (node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (!node->right()->is_typed(cdk::TYPE_INT) && !node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    throw std::string("integer or double expression expected in binary logical expression (left)");
  }

  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//------------------------------cdk node-------------------------------------

void mml::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl);
}

//--------------------------------EMPTYS-------------------------------------

void mml::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
  // EMPTY
}

void mml::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
  // EMPTY
}

void mml::type_checker::do_block_node(mml::block_node *const node, int lvl) {
  //EMPTY
}

void mml::type_checker::do_stop_node(mml::stop_node *const node, int lvl) {
  //Stop level type is already checked in parser
}

void mml::type_checker::do_next_node(mml::next_node *const node, int lvl) {
  //Next level type is already checked in parser
}

//-----------------------------------TYPES-----------------------------------

void mml::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void mml::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
}

void mml::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

//---------------------------------------------------------------------------

/* Only for integers or doubles */
void mml::type_checker::do_neg_node(cdk::neg_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) { // checking INPUT
    if (!is_input_node(node->argument())) {
      throw std::string("integer or double expressions expected");
    }
  }
  
  if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
    node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (!node->argument()->is_typed(cdk::TYPE_INT) && !node->argument()->is_typed(cdk::TYPE_DOUBLE)) {
    throw std::string("integer or double expressions expected");
  }

  node->type(node->argument()->type());
}

/* Only for integers or doubles */
void mml::type_checker::do_identity_node(mml::identity_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->argument()->accept(this, lvl + 2);

  if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
    if (!is_input_node(node->argument())) {
      throw std::string("integer expression expected in binary operator (left)");
    }
  }

  if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
    node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (node->argument()->is_typed(cdk::TYPE_INT)) {
    node->type(node->argument()->type());
  } else if (node->argument()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(node->argument()->type());
  } else {
    throw std::string("integer or double expressions expected");
  }
}

/* Only for integers, doubles or pointers */
void mml::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  check_addExpression(node, lvl);
}

/* Only for integers, doubles or pointers */
void mml::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  check_subExpression(node, lvl);
}

/* Only for integers or doubles */
void mml::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  check_mulExpression(node, lvl);
}

/* Only for integers or doubles */
void mml::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  check_mulExpression(node, lvl);
}

/* Only for integers */
void mml::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  check_BinaryIntOnlyExpression(node, lvl);
}

/* Only for integers or doubles */
void mml::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  check_comparationExpression(node, lvl);
}

/* Only for integers or doubles */
void mml::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  check_comparationExpression(node, lvl);
}

/* Only for integers or doubles */
void mml::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  check_comparationExpression(node, lvl);
}

/* Only for integers or doubles */
void mml::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  check_comparationExpression(node, lvl);
}

/* Only for integers, doubles or pointers */
void mml::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  check_generalLogicalBinaryExpression(node, lvl);
}

/* Only for integers, doubles or pointers */
void mml::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  check_generalLogicalBinaryExpression(node, lvl);
}

/* Only for integers */
void mml::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  check_UnaryIntOnlyExpression(node, lvl); // in mml NOT operation is only for TYPE_INT
}

/* Only for integers */
void mml::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  check_BinaryIntOnlyExpression(node, lvl); // in mml AND operation is only for TYPE_INT
}

/* Only for integers */
void mml::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  check_BinaryIntOnlyExpression(node, lvl); // in mml OR operation is only for TYPE_INT
}

//---------------------------------------------------------------------------

void mml::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC;

  const std::string &id = node->name();
  std::shared_ptr<mml::symbol> symbol = _symtab.find(id);

  if (symbol != nullptr) {
    node->type(symbol->type());
  } else {
    throw "undeclared variable '" + id + "'";
  }
}

void mml::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC;

  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  } catch (const std::string &id) {
    throw "undeclared variable '" + id + "'";
  }
}

void mml::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->lvalue()->accept(this, lvl + 2);
  node->rvalue()->accept(this, lvl + 2);
  
  process_assignment(node->lvalue(), node->rvalue());
  node->type(node->lvalue()->type());
}

//---------------------------------------------------------------------------

void mml::type_checker::do_evaluation_node(mml::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);

  if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
    node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
}

/* Only for integers, doubles or strings */
void mml::type_checker::do_print_node(mml::print_node *const node, int lvl) {
  node->arguments()->accept(this, lvl + 2);
  cdk::expression_node *expr_node;
  
  for (size_t i = 0; i < node->arguments()->size(); i++) {
    expr_node = dynamic_cast<cdk::expression_node *>(node->arguments()->node(i));
    if (!expr_node) throw new std::string("print argument error.");
    
    if (expr_node->is_typed(cdk::TYPE_UNSPEC)) {
      expr_node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (expr_node->is_typed(cdk::TYPE_POINTER)) {
      throw new std::string("cannot print a pointer.");
    } else if (!expr_node->is_typed(cdk::TYPE_INT) && 
        !expr_node->is_typed(cdk::TYPE_DOUBLE) && 
        !expr_node->is_typed(cdk::TYPE_STRING)) {
      throw new std::string("unkown type to print.");
    }
  }
}

//--------------------------TYPE_UNSPEC--------------------------------------

void mml::type_checker::do_input_node(mml::input_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->type(cdk::primitive_type::create(0, cdk::TYPE_UNSPEC));
}

//---------------------------------------------------------------------------
// A condition can be any type
void mml::type_checker::do_while_node(mml::while_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4); //UNSPEC, ONLY INT?
}
// A condition can be any type
void mml::type_checker::do_if_node(mml::if_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (!node->condition()->is_typed(cdk::TYPE_INT)) //UNSPEC, ONLY INT?
    throw std::string("expected integer condition");
}
// A condition can be any type
void mml::type_checker::do_if_else_node(mml::if_else_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (!node->condition()->is_typed(cdk::TYPE_INT)) //UNSPEC, ONLY INT?
    throw std::string("expected integer condition");
}

//---------------------------------------------------------------------------

void mml::type_checker::do_index_node(mml::index_node *const node, int lvl) {
  ASSERT_UNSPEC;

  std::shared_ptr<cdk::reference_type> base_type;

  if (node->base()) {
    node->base()->accept(this, lvl + 2);

    if (!node->base()->is_typed(cdk::TYPE_POINTER)) 
      throw std::string("pointer expression expected in index left-value");
    base_type = cdk::reference_type::cast(node->base()->type());

    if (node->index()) {
      node->index()->accept(this, lvl + 2);

      if (node->index()->is_typed(cdk::TYPE_UNSPEC)) { 
        if (!is_input_node(node->index())) // checking INPUT
          throw std::string("integer expression expected in index");

        node->index()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
      } else if (!node->index()->is_typed(cdk::TYPE_INT))
        throw std::string("integer expression expected in left-value index");
    } else {
      throw std::string("unkown index.");
    }

    node->type(base_type->referenced());
  } else {
    throw std::string("unkown index base.");
  }
}

void mml::type_checker::do_sizeof_node(mml::sizeof_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->expression()->accept(this, lvl + 2);
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void mml::type_checker::do_null_node(mml::null_node *const node, int lvl) {
  node->type(cdk::reference_type::create(4, nullptr));
}

void mml::type_checker::do_address_of_node(mml::address_of_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->lvalue()->accept(this, lvl + 2);
  node->type(cdk::reference_type::create(4, node->lvalue()->type()));
}

void mml::type_checker::do_stack_alloc_node(mml::stack_alloc_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) { // checking INPUT
    if (!is_input_node(node->argument())) 
      throw std::string("integer expression expected in stack alloc.");
    
    node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } 
  else if (!node->argument()->is_typed(cdk::TYPE_INT)) 
    throw std::string("integer expression expected in stack alloc.");

  node->type(cdk::reference_type::create(4, cdk::primitive_type::create(1, cdk::TYPE_VOID)));
}

//---------------------------------------------------------------------------

void mml::type_checker::do_function_call_node(mml::function_call_node *const node, int lvl) {
  ASSERT_UNSPEC;

  std::shared_ptr<cdk::functional_type> funType;
  std::shared_ptr<mml::symbol> symbol;
  
  if (!node->identifier()) { // recursive!
    symbol = _symtab.find("sugoi", 1);
    if (symbol == nullptr) throw std::string("recursive call outside function");
    if (!symbol->is_typed(cdk::TYPE_FUNCTIONAL)) throw std::string("symbol is not a function.");
    if (symbol->is_main()) throw std::string("recursive call in main");
    funType = cdk::functional_type::cast(symbol->type());
  } else {
    node->identifier()->accept(this, lvl + 2);
    if (!node->identifier()->is_typed(cdk::TYPE_FUNCTIONAL)) throw std::string("identifier not a function.");
    funType = cdk::functional_type::cast(node->identifier()->type());
  }

  node->type(funType->output(0));

  if (node->arguments()->size() == funType->input_length()) { // #arguments
    node->arguments()->accept(this, lvl + 4);
    for (size_t arg = 0; arg < node->arguments()->size(); arg++) {
      if (node->argument(arg)->type()->name() == funType->input(arg)->name()) continue;
      if (node->argument(arg)->is_typed(cdk::TYPE_INT) && (funType->input(arg)->name() == cdk::TYPE_DOUBLE)) continue;
      throw std::string("type mismatch in function call for argument" + std::to_string(arg + 1) + ".");
    }
  } else {
    throw std::string("wrong number of arguments. " + 
        std::to_string(node->arguments()->size()) + " were given; " + 
        std::to_string(funType->input_length()) + "expected.");
  }
}

void mml::type_checker::do_function_definition_node(mml::function_definition_node *const node, int lvl) {
  auto function = mml::make_symbol(node->type(), "sugoi", tPRIVATE);

  if (_symtab.find_local(function->name())) 
    _symtab.replace(function->name(), function);
  else if (!_symtab.insert(function->name(), function)) 
    throw std::string("error inserting function aux");

  _parent->set_new_symbol(function);
}

void mml::type_checker::do_program_node(mml::program_node *const node, int lvl) {
  auto main = mml::make_symbol(cdk::functional_type::create(cdk::primitive_type::create(4, cdk::TYPE_INT)), "kawai", tPRIVATE);
  auto main_aux = mml::make_symbol(cdk::functional_type::create(cdk::primitive_type::create(4, cdk::TYPE_INT)), "sugoi", tPRIVATE);
  main->set_main(true);
  main_aux->set_main(true);

  if (_symtab.find_local(main_aux->name())) 
    _symtab.replace(main_aux->name(), main_aux);
  else if (!_symtab.insert(main_aux->name(), main_aux)) 
    throw std::string("error inserting main aux");

  _parent->set_new_symbol(main);
}

void mml::type_checker::do_return_node(mml::return_node *const node, int lvl) {

  auto symbol = _symtab.find("sugoi", 1);

  if (!symbol) {
    symbol = _symtab.find("kawai", 0);
    
    if (!symbol) {
      throw std::string("return statement outside program block");
    } else {
      if (!node->retval())
         throw std::string("wrong type for program return expression (integer expected).");

      node->retval()->accept(this, lvl + 2);
      if (!node->retval()->is_typed(cdk::TYPE_INT))
        throw std::string("wrong type for program return expression (integer expected).");
    }
  } else if (node->retval()) {
    std::shared_ptr<cdk::functional_type> return_type = cdk::functional_type::cast(symbol->type());

    if (return_type->output() && return_type->output(0)->name() == cdk::TYPE_VOID)
      throw std::string("void function cannot return values.");
    
    node->retval()->accept(this, lvl + 2);
    if (return_type->output() && return_type->output(0)->name() == cdk::TYPE_INT) {
      if (!(node->retval()->is_typed(cdk::TYPE_INT)))
        throw std::string("wrong return type (integer expected).");
    } else if (return_type->output() && return_type->output(0)->name() == cdk::TYPE_DOUBLE) {
      if (!(node->retval()->is_typed(cdk::TYPE_INT)) && !(node->retval()->is_typed(cdk::TYPE_DOUBLE)))
        throw std::string("Wrong return type (integer or double expected).");
    } else if (return_type->output() && return_type->output(0)->name() == cdk::TYPE_STRING) {
      if (!(node->retval()->is_typed(cdk::TYPE_STRING)))
        throw std::string("Wrong return type (string expected).");
    } else if (return_type->output() && return_type->output(0)->name() == cdk::TYPE_POINTER) {
      process_same_pointer_types(return_type->output(0), node->retval());
    } else if (return_type->output() && return_type->output(0)->name() == cdk::TYPE_FUNCTIONAL) {
      if (!(return_type->output(0)->name() == cdk::TYPE_FUNCTIONAL))
        throw std::string("wrong return type (functional expected).");
      same_function_types(cdk::functional_type::cast(return_type->output(0)), cdk::functional_type::cast(node->retval()->type()));
    } else {
      throw std::string("Unknown type for return expression.");
    }
  }
}

void mml::type_checker::do_declaration_node(mml::declaration_node *const node, int lvl) {

  if (!node->type()) { //AUTO
    if (!node->initializer()) {
      throw std::string("auto type must have an initializer.");
    }
    node->initializer()->accept(this, lvl + 2);

    if (node->initializer()->is_typed(cdk::TYPE_UNSPEC)) {
      node->initializer()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (!node->initializer()->is_typed(cdk::TYPE_INT) && !node->initializer()->is_typed(cdk::TYPE_DOUBLE) &&
        !node->initializer()->is_typed(cdk::TYPE_STRING) && !node->initializer()->is_typed(cdk::TYPE_POINTER) &&
        !node->initializer()->is_typed(cdk::TYPE_FUNCTIONAL)) {
      throw std::string("unknown type for auto initializer.");
    }

    node->type(node->initializer()->type());
  } else { 
    if (node->initializer()) { //INT, DOUBLE, STRING, POINTER, UNSPEC(INPUT), FUNCTIONAL
      node->initializer()->accept(this, lvl + 2);
      process_assignment(node, node->initializer());
    }
  }

  const std::string &id = node->identifier();
  auto symbol = mml::make_symbol(node->type(), id, node->qualifier());
  auto prev_symbol = _symtab.find_local(symbol->name());

  if (prev_symbol && prev_symbol->qualifier() == tFORWARD) {
    if (prev_symbol->type()->name() == cdk::TYPE_POINTER && symbol->type()->name() == cdk::TYPE_POINTER) {
      same_pointer_types(prev_symbol->type(), symbol->type()); //potential wrong cast
      _symtab.replace(symbol->name(), symbol);
    } else if (prev_symbol->type()->name() == cdk::TYPE_FUNCTIONAL && symbol->type()->name() == cdk::TYPE_FUNCTIONAL) {
      same_function_types(cdk::functional_type::cast(prev_symbol->type()), cdk::functional_type::cast(symbol->type()));
      _symtab.replace(symbol->name(), symbol);
    } else if (prev_symbol->type()->name() == symbol->type()->name()) _symtab.replace(symbol->name(), symbol);
    else throw id + " redeclared";
  } else _symtab.insert(id, symbol);

  _parent->set_new_symbol(symbol);

  if (node->qualifier() == tFOREIGN) {
    symbol->set_foreign(true);
  }
}

//---------------------------------------------------------------------------

void mml::type_checker::process_assignment(cdk::typed_node *const lnode, cdk::typed_node *const rnode) {
  // left node cannot be UNSPEC
  if (lnode->is_typed(cdk::TYPE_INT)) {
    if (rnode->is_typed(cdk::TYPE_UNSPEC) && is_input_node(rnode)) {
      rnode->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (!rnode->is_typed(cdk::TYPE_INT))
      throw std::string("wrong type for initializer (integer expected).");
  } else if (lnode->is_typed(cdk::TYPE_DOUBLE)) {
    if (rnode->is_typed(cdk::TYPE_UNSPEC) && is_input_node(rnode)) {
      rnode->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else if (!rnode->is_typed(cdk::TYPE_INT) && !rnode->is_typed(cdk::TYPE_DOUBLE))
      throw std::string("wrong type for initializer (integer or double expected).");
  } else if (lnode->is_typed(cdk::TYPE_STRING)) {
    if (!rnode->is_typed(cdk::TYPE_STRING))
      throw std::string("wrong type for initializer (string expected).");
  } else if (lnode->is_typed(cdk::TYPE_POINTER)) {
    if (!rnode->is_typed(cdk::TYPE_POINTER))
      throw std::string("wrong type for initializer (pointer expected).");
    process_same_pointer_types(lnode->type(), rnode);
  } else if (lnode->is_typed(cdk::TYPE_FUNCTIONAL)) {
    if (!rnode->is_typed(cdk::TYPE_FUNCTIONAL))
      throw std::string("wrong type for initializer (functional expected).");
    same_function_types(cdk::functional_type::cast(lnode->type()), cdk::functional_type::cast(rnode->type()));
    lnode->type(rnode->type());
  } else {
    throw std::string("unknown type for initializer.");
  }
}

void mml::type_checker::same_pointer_types(std::shared_ptr<cdk::basic_type> ltype, std::shared_ptr<cdk::basic_type> rtype) {
  std::shared_ptr<cdk::basic_type> lref_type = ltype;
  std::shared_ptr<cdk::basic_type> rref_type = rtype;

  while (lref_type->name() == cdk::TYPE_POINTER && rref_type->name() == cdk::TYPE_POINTER && rref_type != nullptr) {
    lref_type = cdk::reference_type::cast(lref_type)->referenced();
    //if (!lref_type) throw "null pointer in asssigment (left)."; // will never happen because lval

    rref_type = cdk::reference_type::cast(rref_type)->referenced();
    
    if (!rref_type) return ; // rtype allowed to be nullptr(null_node)
  }

  if (lref_type->name() == cdk::TYPE_FUNCTIONAL && rref_type->name() == cdk::TYPE_FUNCTIONAL) {
    same_function_types(cdk::functional_type::cast(lref_type), cdk::functional_type::cast(rref_type));
  } else if (!(lref_type->name() == cdk::TYPE_VOID || rref_type->name() == cdk::TYPE_VOID) && // void pointer
      lref_type->name() != rref_type->name()) {
    throw std::string("pointer types not matching.");
  }
}

void mml::type_checker::process_same_pointer_types(std::shared_ptr<cdk::basic_type> ltype, cdk::typed_node *const rnode) {
  std::shared_ptr<cdk::basic_type> lref_type = ltype;
  std::shared_ptr<cdk::basic_type> rref_type = cdk::reference_type::cast(rnode->type());

  while (lref_type->name() == cdk::TYPE_POINTER && rref_type->name() == cdk::TYPE_POINTER) {
    lref_type = cdk::reference_type::cast(lref_type)->referenced();
    if (!lref_type) throw "null pointer in asssigment (left)."; // will never happen because lval

    rref_type = cdk::reference_type::cast(rref_type)->referenced();
    if (!rref_type) { // rtype allowed to be nullptr(null_node)
      if (is_null_node(rnode)) return ;
      throw std::string("unkown pointer with nullptr.");
    }
  }
  
  if (lref_type->name() == cdk::TYPE_FUNCTIONAL && rref_type->name() == cdk::TYPE_FUNCTIONAL) {
    same_function_types(cdk::functional_type::cast(lref_type), cdk::functional_type::cast(rref_type));
  } else if (rref_type->name() == cdk::TYPE_VOID) { // checking stack_alloc
    if (is_stack_alloc_node(rnode))
      rnode->type(cdk::reference_type::create(4, lref_type));
  } else if (!(lref_type->name() == cdk::TYPE_VOID || rref_type->name() == cdk::TYPE_VOID) && // void pointer
      lref_type->name() != rref_type->name()) {
    throw std::string("pointer types not matching.");
  }
}

void mml::type_checker::same_function_types(std::shared_ptr<cdk::functional_type> func_type1, 
    std::shared_ptr<cdk::functional_type> func_type2) {

  if (func_type1->input_length() != func_type2->input_length()) {
    throw std::string("function input length not matching.");
  } else if (func_type1->output_length() != func_type2->output_length()) {
    throw std::string("function output length not matching.");
  }

  size_t input_length = func_type2->input_length();
  for (size_t i = 0; i < input_length; i++) {
    if (func_type1->input(i) != func_type2->input(i) && 
        !((func_type1->input(i)->name() == cdk::TYPE_INT || func_type1->input(i)->name() == cdk::TYPE_DOUBLE) && 
            (func_type2->input(i)->name() == cdk::TYPE_INT || func_type2->input(i)->name() == cdk::TYPE_DOUBLE))){
      throw std::string("function output length not matching.");
    }
  }

  size_t output_length = func_type2->output_length();
  for (size_t i = 0; i < output_length; i++) {
    if (func_type1->output(i) != func_type2->output(i) && 
        !((func_type1->output(i)->name() == cdk::TYPE_INT || func_type1->output(i)->name() == cdk::TYPE_DOUBLE) && 
            (func_type2->output(i)->name() == cdk::TYPE_INT || func_type2->output(i)->name() == cdk::TYPE_DOUBLE))){
      throw std::string("function output length not matching.");
    }
  }
}

