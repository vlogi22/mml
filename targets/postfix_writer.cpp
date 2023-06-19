#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include ".auto/all_nodes.h"  // all_nodes.h is automatically generated
#include "mml_parser.tab.h"

//---------------------------------------------------------------------------

void mml::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void mml::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}

void mml::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->argument()->accept(this, lvl + 2);
  _pf.INT(0);
  _pf.EQ();
}
void mml::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  std::string lbl = mklbl(++_lbl);
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JZ(lbl);
  node->right()->accept(this, lvl + 2);
  _pf.AND();
  _pf.ALIGN();
  _pf.LABEL(lbl);
}
void mml::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  std::string lbl = mklbl(++_lbl);
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JNZ(lbl);
  node->right()->accept(this, lvl + 2);
  _pf.OR();
  _pf.ALIGN();
  _pf.LABEL(lbl);
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//------------------------------TYPES----------------------------------------

void mml::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  if (_inFunctionBody) {
    _pf.INT(node->value()); // integer literal is on the stack: push an integer
  } else {
    _pf.SINT(node->value()); // integer literal is on the DATA segment
  }
}

void mml::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  if (_inFunctionBody) {
    _pf.DOUBLE(node->value()); // load number to the stack
  } else {
    _pf.SDOUBLE(node->value());    // double is on the DATA segment
  }
}

void mml::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  std::string lbl = mklbl(++_lbl);

  _pf.RODATA();
  _pf.ALIGN();
  _pf.LABEL(lbl);
  _pf.SSTRING(node->value());

  if (_inFunctionBody) {
    _pf.TEXT(_return_labels.back());
    _pf.ADDR(lbl);
  } else {
    _pf.DATA();
    _pf.SADDR(lbl);
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->argument()->accept(this, lvl + 2);
  _pf.NEG();
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    auto referenced = cdk::reference_type::cast(node->right()->type())->referenced();
    _pf.INT(referenced->size());
    _pf.MUL();
  }

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    auto referenced = cdk::reference_type::cast(node->left()->type())->referenced();
    _pf.INT(referenced->size());
    _pf.MUL();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DADD();
  } else {
    _pf.ADD();
  }
}

void mml::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    auto referenced = cdk::reference_type::cast(node->right()->type())->referenced();
    _pf.INT(referenced->size());
    _pf.MUL();
  }

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    auto referenced = cdk::reference_type::cast(node->left()->type())->referenced();
    _pf.INT(referenced->size());
    _pf.MUL();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DSUB();
  } else {
    _pf.SUB();
  }
}

void mml::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE)) _pf.DMUL();
  else _pf.MUL();
}

void mml::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE)) _pf.DDIV();
  else _pf.DIV();
}

void mml::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  _pf.MOD();
}

void mml::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->right()->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) _pf.I2D();
  
  if (node->left()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.LT();
}

void mml::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl + 2);
  if (node->right()->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) _pf.I2D();
  
  if (node->left()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.LE();
}

void mml::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->right()->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) _pf.I2D();
  
  if (node->left()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.GE();
}

void mml::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl + 2);
  if (node->right()->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) _pf.I2D();
  
  if (node->left()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.GT();
}

void mml::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl + 2);
  if (node->right()->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) _pf.I2D();
  
  if (node->left()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }
 
  _pf.NE();
}

void mml::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl + 2);
  if (node->right()->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) _pf.I2D();
  
  if (node->left()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }
  _pf.EQ();
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  const std::string &id = node->name();
  auto var = _symtab.find(id);
 
  if (var->is_foreign()) _foreign_label = var->name();
  else if (var->is_global()) _pf.ADDR(var->name());
  else _pf.LOCAL(var->offset());
}

void mml::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->lvalue()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE)) _pf.LDDOUBLE();
  else if (_foreign_label.empty()) _pf.LDINT();
}

void mml::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
   node->rvalue()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    if (node->rvalue()->is_typed(cdk::TYPE_INT)) _pf.I2D();
    _pf.DUP64();
  } else _pf.DUP32();

  node->lvalue()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE)) _pf.STDOUBLE();
  else _pf.STINT();
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_evaluation_node(mml::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->argument()->accept(this, lvl);
  _pf.TRASH(node->argument()->type()->size());
}

void mml::postfix_writer::do_print_node(mml::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  for (size_t ix = 0; ix < node->arguments()->size(); ix++) {
    auto child = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ix));
    child->accept(this, lvl);
    if (child->is_typed(cdk::TYPE_INT)) {
      _foreign_functions.insert("printi");
      _pf.CALL("printi");
      _pf.TRASH(4);
    } else if (child->is_typed(cdk::TYPE_DOUBLE)) {
      _foreign_functions.insert("printd");
      _pf.CALL("printd");
      _pf.TRASH(8);
    } else if (child->is_typed(cdk::TYPE_STRING)) {
      _foreign_functions.insert("prints");
      _pf.CALL("prints");
      _pf.TRASH(4);
    } else {
      std::cerr << "cannot write expression of unknown type" << std::endl;
    }
  }
  
  if (node->newline()) {
    _foreign_functions.insert("println");
    _pf.CALL("println");
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_input_node(mml::input_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (_lvalueType == cdk::TYPE_DOUBLE) {
    _foreign_functions.insert("readd");
    _pf.CALL("readd");
    _pf.LDFVAL64();
  } else if (_lvalueType == cdk::TYPE_INT) {
    _foreign_functions.insert("readi");
    _pf.CALL("readi");
    _pf.LDFVAL32();
  } else {
    std::cerr << "FATAL: " << node->lineno() << ": cannot read type" << std::endl;
    return;
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_while_node(mml::while_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  _whileCond.push_back(++_lbl);
  _whileEnd.push_back(++_lbl);

  _symtab.push(); // new context

  _pf.ALIGN();
  _pf.LABEL(mklbl(_whileCond.back()));
  node->condition()->accept(this, lvl + 2);
  _pf.JZ(mklbl(_whileEnd.back()));

  node->block()->accept(this, lvl + 2);
  _pf.JMP(mklbl(_whileCond.back()));
  _pf.ALIGN();
  _pf.LABEL(mklbl(_whileEnd.back()));

  _symtab.pop();
  
  _whileEnd.pop_back();
  _whileCond.pop_back();
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_if_node(mml::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_if_else_node(mml::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}


void mml::postfix_writer::do_block_node(mml::block_node *const node, int lvl) {
  _symtab.push(); // for block-local vars
  if (node->declarations()) node->declarations()->accept(this, lvl + 2);
  if (node->instructions()) node->instructions()->accept(this, lvl + 2);
  _symtab.pop();
}

void mml::postfix_writer::do_index_node(mml::index_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->base()->accept(this, lvl);
  node->index()->accept(this, lvl);
  _pf.INT(node->type()->size());
  _pf.MUL();
  _pf.ADD();
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_null_node(mml::null_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  if (_inFunctionBody) {
    _pf.INT(0);
  } else {
    _pf.SINT(0);
  }
}

void mml::postfix_writer::do_stop_node(mml::stop_node *const node, int lvl) {
  size_t n = node->level();
  if (_whileCond.size() > n-1) {
    _pf.JMP(mklbl(_whileEnd[_whileEnd.size() - n]));
  }  else
    error(node->lineno(), "'stop' outside 'while'");
}

void mml::postfix_writer::do_next_node(mml::next_node *const node, int lvl) {
  size_t n = node->level();
  if (_whileCond.size() > n-1) {
    _pf.JMP(mklbl(_whileCond[_whileCond.size() - n]));
  } else
    error(node->lineno(), "'next' outside 'while'");
}

void mml::postfix_writer::do_address_of_node(mml::address_of_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->lvalue()->accept(this, lvl + 2);
}

void mml::postfix_writer::do_stack_alloc_node(mml::stack_alloc_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->argument()->accept(this, lvl + 2);  // vector lenght inserted
  _pf.INT(cdk::reference_type::cast(node->type())->referenced()->size());
  _pf.MUL();
  _pf.ALLOC();
  _pf.SP();
}

void mml::postfix_writer::do_identity_node(mml::identity_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_function_call_node(mml::function_call_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  std::vector<std::shared_ptr<cdk::basic_type>> in_types;

  if (node->identifier()) { // non-recursive
    in_types = cdk::functional_type::cast(node->identifier()->type())->input()->components();
  } else { // recursive
    auto curr_fun = _functions.back();
    in_types = cdk::functional_type::cast(curr_fun->type())->input()->components();
  }

  size_t args_size = 0;
  if (node->arguments()) {
    for (int ix = node->arguments()->size() - 1; ix >= 0; --ix) {
      auto arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ix));
      arg->accept(this, lvl + 2);
      if (arg->is_typed(cdk::TYPE_INT) && in_types.at(ix)->name() == cdk::TYPE_DOUBLE) {
        _pf.I2D();
        args_size += 4; // needed?
      }
      args_size += arg->type()->size();
    }
  }

  if (node->identifier()) { // non-recursive
    _foreign_label.clear();
    node->identifier()->accept(this, lvl + 2);

    if (!_foreign_label.empty()) _pf.CALL(_foreign_label);
    else _pf.BRANCH();

  } else _pf.CALL(_return_labels.back()); // recursive case

  if (args_size != 0) _pf.TRASH(args_size);

  if (node->is_typed(cdk::TYPE_INT)) {
    if (!_foreign_label.empty()) {
      _pf.LDFVAL32();
    } else {
      _pf.LDFVAL64();
      _pf.D2I();
    }
  } else if (node->is_typed(cdk::TYPE_POINTER) || node->is_typed(cdk::TYPE_STRING) || node->is_typed(cdk::TYPE_FUNCTIONAL)) {
    _pf.LDFVAL32();
  } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.LDFVAL64();
  }

  _foreign_label.clear();
}

void mml::postfix_writer::do_function_definition_node(mml::function_definition_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  std::shared_ptr<mml::symbol> function = new_symbol();

  if (function) {
    _functions.push_back(function);
    reset_new_symbol();
  }

  _offset = 8;
  _symtab.push(); // scope of args

  if(node->arguments()) {
    _inFunctionArgs = true;
    for (size_t ix = 0; ix < node-> arguments()->size(); ix++){
      cdk::basic_node *argument = node->arguments()->node(ix);
      if (!argument) break; // this means an empty sequence of arguments
      argument->accept(this, 0);
    }
    _inFunctionArgs = false;
  }

  std::string label = mklbl(++_lbl);
  _return_labels.push_back(label);
  _pf.TEXT(label);
  _pf.ALIGN();
  if (function->qualifier() == tPUBLIC) _pf.GLOBAL(function->name(), _pf.FUNC());
  _pf.LABEL(label);

  frame_size_calculator lsc(_compiler, _symtab, function);

  // all inserted symbols during this visit are useless (hack!)
  _symtab.push();
  node->accept(&lsc, lvl);
  _symtab.pop();
  
  _pf.ENTER(lsc.localsize());

  _offset = 0;

  bool reset = _inFunctionBody;
  _inFunctionBody = true;
  
  if (node->block()) node->block()->accept(this, lvl + 4);
  _inFunctionBody = reset;

  _symtab.pop(); // scope of args

  if (!_returnSeen) {
    _pf.LEAVE();
    _pf.RET();
  }

  _return_labels.pop_back();
  if (function) _functions.pop_back();
  
  if (_inFunctionBody) {
    _pf.TEXT(_return_labels.back()); // change to label?
    _pf.ALIGN();
    _pf.ADDR(label);
  } else { // needed?
    _pf.DATA();
    _pf.ALIGN();
    _pf.SADDR(label);
  }

  _currentBodyEnterLabel = label;
}

void mml::postfix_writer::do_program_node(mml::program_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  std::shared_ptr<mml::symbol> function;

  for (std::string decl_name : _to_BSS) {
    auto function = _symtab.find(decl_name, 0);
    if (function->is_foreign()) _foreign_functions.insert(decl_name);
    else {
      _pf.BSS();
      _pf.ALIGN();
      _pf.LABEL(decl_name);
      _pf.SALLOC(function->type()->size());
    }
  }

  function = new_symbol();
  _functions.push_back(function);
  reset_new_symbol();

  _return_labels.push_back("_main");

  _symtab.push();

  _pf.TEXT("_main");
  _pf.ALIGN();
  _pf.GLOBAL("_main", _pf.FUNC());
  _pf.LABEL("_main");
  
  frame_size_calculator lsc(_compiler, _symtab, function);

  // frame_size_calculator
  _symtab.push();
  node->accept(&lsc, lvl);
  _symtab.pop();
  // frame_size_calculator

  _pf.ENTER(lsc.localsize());

  bool prevReturnSeen = _returnSeen;
  _returnSeen = false;

  _inFunctionBody = true;
  if (node->block()) node->block()->accept(this, lvl + 4);
  _inFunctionBody = false;

  _symtab.pop(); // scope of arguments
  
  _return_labels.pop_back();
  if (!_returnSeen) {
    _pf.INT(0);
    _pf.STFVAL32();
  }

  _pf.LEAVE();
  _pf.RET();

  _functions.pop_back();
  for (std::string s: _foreign_functions) {
    _pf.EXTERN(s);
  }

  _foreign_functions.clear();
  _returnSeen = prevReturnSeen;
}

void mml::postfix_writer::do_return_node(mml::return_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  _returnSeen = true;
  
  auto curr_fun = _functions.back();
  auto output = cdk::functional_type::cast(curr_fun->type())->output(0);
  if (output->name() != cdk::TYPE_VOID) {
    node->retval()->accept(this, lvl + 2);
    if (output->name() == cdk::TYPE_INT) {
      if (!curr_fun->is_main()) {
        _pf.I2D();
        _pf.STFVAL64();
      } else {
        _pf.STFVAL32();
      }
    }else if (output->name() == cdk::TYPE_STRING || output->name() == cdk::TYPE_POINTER 
        || output->name() == cdk::TYPE_FUNCTIONAL) {
      _pf.STFVAL32();
    } else if (output->name() == cdk::TYPE_DOUBLE) {
      if (node->retval()->is_typed(cdk::TYPE_INT)) _pf.I2D();
      _pf.STFVAL64();
    } else {
      std::cerr << node->lineno() << ": should not happen: unknown return type" << std::endl;
    }
  }
  _pf.LEAVE();
  _pf.RET();
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_declaration_node(mml::declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  const std::string &id = node->identifier();
  int offset = 0, typesize = node->type()->size();

  if (_inFunctionArgs) {
    offset = _offset;
    _offset += typesize;
  } else if (_inFunctionBody) {
    _offset -= typesize;
    offset = _offset;
  } else {
    offset = 0;
  }

  std::shared_ptr<mml::symbol> symbol = new_symbol();
  if (symbol) {
    symbol->set_offset(offset);
    reset_new_symbol();
  }

  if (!_inFunctionArgs && !_inFunctionBody) {
    _to_BSS.insert(symbol->name());
  }

  if (node->initializer()) {
    _to_BSS.erase(symbol->name());
    if (_inFunctionBody) {
      node->initializer()->accept(this, lvl);
      if (symbol->is_typed(cdk::TYPE_INT) || symbol->is_typed(cdk::TYPE_STRING) || 
          symbol->is_typed(cdk::TYPE_POINTER) || symbol->is_typed(cdk::TYPE_FUNCTIONAL)) {
        _pf.LOCAL(symbol->offset());
        _pf.STINT();
      } else if(symbol->is_typed(cdk::TYPE_DOUBLE)) {
        if (node->initializer()->is_typed(cdk::TYPE_INT)) _pf.I2D();
        _pf.LOCAL(symbol->offset());
        _pf.STDOUBLE();
      } else {
        std::cerr << "cannot initialize" << std::endl;
        return;
      }
    } else {
      if (symbol->is_typed(cdk::TYPE_INT) || symbol->is_typed(cdk::TYPE_DOUBLE) || symbol->is_typed(cdk::TYPE_POINTER)) {
        _pf.DATA();
        _pf.ALIGN();
        _pf.LABEL(symbol->name());
        
        if (symbol->is_typed(cdk::TYPE_INT) || symbol->is_typed(cdk::TYPE_POINTER)) {
          node->initializer()->accept(this, lvl);
        } else if (symbol->is_typed(cdk::TYPE_DOUBLE)) {
          if (node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
            node->initializer()->accept(this, lvl);
          } else if (node->initializer()->is_typed(cdk::TYPE_INT)) {
            cdk::integer_node *i2d = dynamic_cast<cdk::integer_node*>(node->initializer());
            cdk::double_node db(i2d->lineno(), i2d->value());
            db.accept(this, lvl);
          } else {
            std::cerr << node->lineno() << ": '" << id << "' has bad initializer for real value"  << std::endl;
          }
        }
      } else if (symbol->is_typed(cdk::TYPE_STRING)) {
        _pf.DATA();
        _pf.ALIGN();
        _pf.LABEL(symbol->name());
        node->initializer()->accept(this, lvl);
      } else if (symbol->is_typed(cdk::TYPE_FUNCTIONAL)) {
        _functions.push_back(symbol);
        reset_new_symbol();
        node->initializer()->accept(this, lvl);
        _pf.DATA();
        if (_functions.back()->qualifier() == tPUBLIC) {
          _pf.GLOBAL(_functions.back()->name(), _pf.OBJ()); // obj?
        }
        _pf.ALIGN();
        _pf.LABEL(symbol->name());
        std::string label = _currentBodyEnterLabel;
        _currentBodyEnterLabel.clear(); // needed?
        _pf.SADDR(label);
      } else {
        std::cerr << node->lineno() << ": '" << id << "' has unexpected initializer" << std::endl;
      }
    }
  }
}
//---------------------------------------------------------------------------

void mml::postfix_writer::do_sizeof_node(mml::sizeof_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (_inFunctionBody) {
    _pf.INT(node->expression()->type()->size());
  } else {
    _pf.SINT(node->expression()->type()->size());
  }
}
