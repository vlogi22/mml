#include <string>
#include <iostream>
#include "targets/type_checker.h"
#include "targets/xml_writer.h"
#include ".auto/all_nodes.h"  // automatically generated

#include "mml_parser.tab.h"

static std::string qualifier_name(int qualifier) {
  if (qualifier == tPUBLIC) return "public";
  if (qualifier == tFORWARD) return "forward";
  if (qualifier == tFOREIGN) return "foreign";
  if (qualifier == tPRIVATE) return "private";
  else return "unknown qualifier";
}

static std::string type_name(std::shared_ptr<cdk::basic_type> dType, bool is_funType) {
  if (is_funType) {
    std::string type_name = "functional_type -input: ";

    size_t in_len = cdk::functional_type::cast(dType)->input_length();
    size_t out_len = cdk::functional_type::cast(dType)->output_length();

    for (size_t i = 0; i < in_len; i++) {
      type_name += cdk::to_string(cdk::functional_type::cast(dType)->input(i)) + ", ";
    }

    type_name += "-output: ";

    for (size_t j = 0; j < out_len; j++) {
      type_name += cdk::to_string(cdk::functional_type::cast(dType)->output(j));
    }

    return type_name;
  } else {
    return cdk::to_string(dType);
  }
}

//---------------------------------------------------------------------------

void mml::xml_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}
void mml::xml_writer::do_data_node(cdk::data_node * const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}
void mml::xml_writer::do_double_node(cdk::double_node * const node, int lvl) {
  process_literal(node, lvl);
}
void mml::xml_writer::do_not_node(cdk::not_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}
void mml::xml_writer::do_and_node(cdk::and_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void mml::xml_writer::do_or_node(cdk::or_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void mml::xml_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<sequence_node size='" << node->size() << "'>" << std::endl;
  for (size_t i = 0; i < node->size(); i++) {
    cdk::basic_node *n = node->node(i);
    if (n == NULL) break;
    n->accept(this, lvl + 2);
  }
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void mml::xml_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  process_literal(node, lvl);
}

void mml::xml_writer::do_string_node(cdk::string_node * const node, int lvl) {
  process_literal(node, lvl);
}

//---------------------------------------------------------------------------

void mml::xml_writer::do_unary_operation(cdk::unary_operation_node * const node, int lvl) {
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void mml::xml_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void mml::xml_writer::do_binary_operation(cdk::binary_operation_node * const node, int lvl) {
  openTag(node, lvl);
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void mml::xml_writer::do_add_node(cdk::add_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void mml::xml_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void mml::xml_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void mml::xml_writer::do_div_node(cdk::div_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void mml::xml_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void mml::xml_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void mml::xml_writer::do_le_node(cdk::le_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void mml::xml_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void mml::xml_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void mml::xml_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void mml::xml_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void mml::xml_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() << ">" << node->name() << "</" << node->label() << ">" << std::endl;
}

void mml::xml_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void mml::xml_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 2);
  node->rvalue()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void mml::xml_writer::do_evaluation_node(mml::evaluation_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void mml::xml_writer::do_print_node(mml::print_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  std::string nl = node->newline() ? "true" : "false";
  os() << std::string(lvl, ' ') << "<" << node->label() << " newline='" << nl << "'>" << std::endl;
  node->arguments()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void mml::xml_writer::do_block_node(mml::block_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("declarations", lvl + 4);
  if (node->declarations()) node->declarations()->accept(this, lvl + 8);
  closeTag("declarations", lvl + 4);
  openTag("instructions", lvl + 4);
  if (node->instructions()) node->instructions()->accept(this, lvl + 8);
  closeTag("instructions", lvl + 4);
  closeTag(node, lvl);
}

void mml::xml_writer::do_index_node(mml::index_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("base", lvl);
  node->base()->accept(this, lvl + 2);
  closeTag("base", lvl);
  openTag("index", lvl);
  node->index()->accept(this, lvl + 2);
  closeTag("index", lvl);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void mml::xml_writer::do_input_node(mml::input_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void mml::xml_writer::do_while_node(mml::while_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

void mml::xml_writer::do_null_node(mml::null_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

void mml::xml_writer::do_stop_node(mml::stop_node *const node, int lvl) {
  openTag(node, lvl);
  os() << node->level() << std::endl;
  closeTag(node, lvl);
}

void mml::xml_writer::do_next_node(mml::next_node *const node, int lvl) {
  openTag(node, lvl);
  os() << node->level() << std::endl;
  closeTag(node, lvl);
}

void mml::xml_writer::do_address_of_node(mml::address_of_node *const node, int lvl) {
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void mml::xml_writer::do_stack_alloc_node(mml::stack_alloc_node *const node, int lvl) {
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void mml::xml_writer::do_identity_node(mml::identity_node *const node, int lvl) {
  do_unary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void mml::xml_writer::do_if_node(mml::if_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl);
  openTag("then", lvl);
  node->block()->accept(this, lvl + 4);
  closeTag("then", lvl);
  closeTag(node, lvl);
}

void mml::xml_writer::do_if_else_node(mml::if_else_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl);
  openTag("then", lvl);
  node->thenblock()->accept(this, lvl + 4);
  closeTag("then", lvl);
  openTag("else", lvl);
  node->elseblock()->accept(this, lvl + 4);
  closeTag("else", lvl);
  closeTag(node, lvl);
}


//---------------------------------------------------------------------------

void mml::xml_writer::do_function_call_node(mml::function_call_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() << ">" << std::endl;
  openTag("identifier", lvl + 4);
  if (node->identifier()) node->identifier()->accept(this, lvl + 8);
  closeTag("identifier", lvl + 4);
  openTag("arguments", lvl + 4);
  if (node->arguments()) node->arguments()->accept(this, lvl + 8);
  closeTag("arguments", lvl + 4);
  closeTag(node, lvl);
}

void mml::xml_writer::do_function_definition_node(mml::function_definition_node *const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  os() << std::string(lvl, ' ') << "<" << node->label() << " type='" << type_name(node->type(), node->is_typed(cdk::TYPE_FUNCTIONAL)) << "'>" << std::endl;
  openTag("arguments", lvl + 4);
  if (node->arguments()) {
    node->arguments()->accept(this, lvl + 8);
  }
  closeTag("arguments", lvl + 4);
  node->block()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

void mml::xml_writer::do_program_node(mml::program_node *const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  os() << std::string(lvl, ' ') << "<" << node->label() << " type='" << type_name(node->type(), node->is_typed(cdk::TYPE_FUNCTIONAL)) << "'>" << std::endl;
  openTag("arguments", lvl + 4);
  if (node->arguments()) {
    node->arguments()->accept(this, lvl + 8);
  }
  closeTag("arguments", lvl + 4);
  node->block()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

void mml::xml_writer::do_return_node(mml::return_node *const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  if (node->retval()) node->retval()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void mml::xml_writer::do_declaration_node(mml::declaration_node *const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  if (node->type()) {
    os() << std::string(lvl, ' ') << "<" << node->label() << " id='" << node->identifier() << "' qualifier='"
    << qualifier_name(node->qualifier()) << "' type='" << type_name(node->type(), node->is_typed(cdk::TYPE_FUNCTIONAL)) << "'>" << std::endl;
  } else {
    os() << std::string(lvl, ' ') << "<" << node->label() << " id='" << node->identifier() << "' qualifier='"
    << qualifier_name(node->qualifier()) << "'>" << std::endl;
  }

  openTag("initializer", lvl + 4);
  if (node->initializer()) {
    node->initializer()->accept(this, lvl + 8);
  }
  closeTag("initializer", lvl + 4);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void mml::xml_writer::do_sizeof_node(mml::sizeof_node *const node, int lvl) {
  openTag(node, lvl);
  node->expression()->accept(this, lvl + 2);
  closeTag(node, lvl);
}