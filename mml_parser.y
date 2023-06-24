%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include ".auto/all_nodes.h"
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
//-- don't change *any* of these --- END!
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  //-- don't change *any* of these --- END!

  int                   i;	/* integer value */
  double                d;	/* double value */
  std::string          *s;	/* symbol name or string literal */
  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;

  
  mml::block_node       *block;
  std::vector<std::shared_ptr<cdk::basic_type>> *args;
};

%token <i> tINTEGER
%token <d> tDOUBLE
%token <s> tID tSTRING

%token tAND tOR tNE tLE tGE tEQ
%token tWHILE tIF tELSE tELIF tPRINT tPRINTLN tINPUT tBEGIN tEND tSIZEOF
%token tPUBLIC tPRIVATE tFOREIGN tFORWARD tARROW
%token tSTOP tNEXT tRETURN
%token tTYPE_STRING tTYPE_INT tTYPE_DOUBLE tTYPE_AUTO tTYPE_VOID tNULLPTR

%type <node> stmt program file dec file_var_dec return argdec iffalse
%type <expression> expr opt_init func_def func_call
%type <sequence> opt_decs file_var_decs instrs opt_instrs exprs opt_exprs argdecs opt_argdecs
%type <s> string
%type <type> basic_type data_type ret_type func_type pointer_type void_pointer_type
%type <args> opt_types type_list
%type <lvalue> lval
%type <block> block

%nonassoc tIFX
%nonassoc tELSE tELIF

%right '='
%left tOR
%left tAND
%nonassoc '~'
%left tEQ tNE
%left tGE tLE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%left '?'
%right tUNARY
%nonassoc '(' '['

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file : /* empty */                      { compiler->ast($$ = new cdk::sequence_node(LINE)); }
     | file_var_decs                    { compiler->ast($$ = $1); }
     | file_var_decs program            { compiler->ast($$ = new cdk::sequence_node(LINE, $2, $1)); }
     |               program            { compiler->ast($$ = new cdk::sequence_node(LINE, $1)); }
     ;

program   : tBEGIN opt_decs opt_instrs tEND       { $$ = new mml::program_node(LINE, nullptr, 
                                                       cdk::primitive_type::create(4, cdk::TYPE_INT), 
                                                       new mml::block_node(LINE, $2, $3)); }
          | tBEGIN          opt_instrs tEND       { $$ = new mml::program_node(LINE, nullptr, 
                                                       cdk::primitive_type::create(4, cdk::TYPE_INT), 
                                                       new mml::block_node(LINE, nullptr, $2)); }
	     ;

file_var_decs  : file_var_dec ';'                           { $$ = new cdk::sequence_node(LINE, $1); }
               | file_var_decs file_var_dec ';'             { $$ = new cdk::sequence_node(LINE, $2, $1); }
               ;

file_var_dec   : tPUBLIC data_type tID opt_init             { $$ = new mml::declaration_node(LINE, tPUBLIC, $2, *$3, $4); }
               | tPUBLIC tTYPE_AUTO tID '=' expr            { $$ = new mml::declaration_node(LINE, tPUBLIC, nullptr, *$3, $5); }
               | tPUBLIC            tID '=' expr            { $$ = new mml::declaration_node(LINE, tPUBLIC, nullptr, *$2, $4); }
               | tFORWARD data_type tID                     { $$ = new mml::declaration_node(LINE, tFORWARD, $2, *$3, nullptr); }
               | tFOREIGN func_type tID                     { $$ = new mml::declaration_node(LINE, tFOREIGN, $2, *$3, nullptr); }
               | dec                                        { $$ = $1; }
               ;

opt_decs  : dec ';'                          { $$ = new cdk::sequence_node(LINE, $1); }
          | opt_decs dec ';'                 { $$ = new cdk::sequence_node(LINE, $2, $1); }
          ;

dec  : data_type tID opt_init      { $$ = new mml::declaration_node(LINE, tPRIVATE, $1, *$2, $3); }
     | tTYPE_AUTO tID '=' expr     { $$ = new mml::declaration_node(LINE, tPRIVATE, nullptr, *$2, $4); }

opt_init  : /* empty */            { $$ = nullptr; }
          | '=' expr               { $$ = $2; }
          ;

instrs    : stmt                   { $$ = new cdk::sequence_node(LINE, $1); }
          | instrs stmt            { $$ = new cdk::sequence_node(LINE, $2, $1); }
          ;

opt_instrs     : /* empty */       { $$ = new cdk::sequence_node(LINE); }
               | instrs            { $$ = $1; }
               ;

return    : tRETURN                { $$ = new mml::return_node(LINE, nullptr); }
          | tRETURN expr           { $$ = new mml::return_node(LINE, $2); }
          ;

/* VARIABLE TYPES */ 

func_type : ret_type '<' opt_types '>'       { $$ = cdk::functional_type::create(*$3, $1); }
          ;

basic_type     : tTYPE_INT                   { $$ = cdk::primitive_type::create(4, cdk::TYPE_INT); }
               | tTYPE_DOUBLE                { $$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }
               | tTYPE_STRING                { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
               | func_type                   { $$ = $1; }
               ;

data_type : basic_type                       { $$ = $1; }
          | pointer_type                     { $$ = $1; }
          | void_pointer_type                { $$ = $1; }
          ;

ret_type  : data_type                        { $$ = $1; }
          | tTYPE_VOID                       { $$ = cdk::primitive_type::create(0, cdk::TYPE_VOID); }
          ;

pointer_type   : '[' basic_type ']'               { $$ = cdk::reference_type::create(4, $2); }
               | '[' pointer_type ']'             { $$ = cdk::reference_type::create(4, $2); }
               ;

void_pointer_type   : '[' tTYPE_VOID ']'          { $$ = cdk::reference_type::create(4, cdk::primitive_type::create(1, cdk::TYPE_VOID)); }
                    | '[' void_pointer_type ']'   { $$ = $2; }
                    ;

opt_types : /* empty */                           { $$ = new std::vector<std::shared_ptr<cdk::basic_type>>(); }
          | type_list                             { $$ = $1; }
          ;

/* i.e: int, double, int... */
type_list : data_type                             { $$ = new std::vector<std::shared_ptr<cdk::basic_type>>(); $$->push_back($1); }
          | type_list ',' data_type               { $$ = $1; $$->push_back($3); }
          ;

argdec    : data_type tID                    { $$ = new mml::declaration_node(LINE, tPRIVATE, $1, *$2, nullptr); }
          ;
 
argdecs   : argdec                           { $$ = new cdk::sequence_node(LINE, $1); }
          | argdecs ',' argdec               { $$ = new cdk::sequence_node(LINE, $3, $1); }
          ;

opt_argdecs    : /* empty */                 { $$ = NULL; }
               | argdecs                     { $$ = $1; }
               ;

/* i.e:   (int i, int o) -> int { ... } */
func_def  : '(' opt_argdecs ')' tARROW  
               ret_type block                { $$ = new mml::function_definition_node(LINE, $2, $5, $6); }
          ;

func_call : expr '(' opt_exprs ')'           { $$ = new mml::function_call_node(LINE, $1, $3); }
          | '@' '(' opt_exprs ')'            { $$ = new mml::function_call_node(LINE, nullptr, $3); }

lval : tID                                   { $$ = new cdk::variable_node(LINE, $1); }
     | expr '[' expr ']'                     { $$ = new mml::index_node(LINE, $1, $3); }
     ;

string    : tSTRING                          { $$ = $1; }
          | string tSTRING                   { $$ = $1; $$->append(*$2); delete $2; }
          ;

block     : '{' opt_decs opt_instrs '}'           { $$ = new mml::block_node(LINE, $2, $3); }
          | '{'          opt_instrs '}'           { $$ = new mml::block_node(LINE, nullptr, $2); }
          ;

iffalse   : tELSE stmt                            { $$ = $2; }
          | tELIF '(' expr ')' stmt %prec tIFX    { $$ = new mml::if_node(LINE, $3, $5); }
          | tELIF '(' expr ')' stmt iffalse       { $$ = new mml::if_else_node(LINE, $3, $5, $6); }
          ;

stmt : expr ';'                                   { $$ = new mml::evaluation_node(LINE, $1); }
 	| exprs tPRINT                               { $$ = new mml::print_node(LINE, $1, false); }
     | exprs tPRINTLN                             { $$ = new mml::print_node(LINE, $1, true); }
     | tWHILE '(' expr ')' stmt                   { $$ = new mml::while_node(LINE, $3, $5); }
     | tIF '(' expr ')' stmt %prec tIFX           { $$ = new mml::if_node(LINE, $3, $5); }
     | tIF '(' expr ')' stmt iffalse              { $$ = new mml::if_else_node(LINE, $3, $5, $6); }
     | tSTOP ';'                                  { $$ = new mml::stop_node(LINE); }
     | tSTOP tINTEGER ';'                         { $$ = new mml::stop_node(LINE, $2); }
     | tNEXT ';'                                  { $$ = new mml::next_node(LINE); }
     | tNEXT tINTEGER ';'                         { $$ = new mml::next_node(LINE, $2); }
     | return ';'                                 { $$ = $1; }
     | block                                      { $$ = $1; }
     ;

opt_exprs : /* NULL */                  { $$ = new cdk::sequence_node(LINE); }
          | exprs                       { $$ = $1; }
          ;

exprs     : expr                        { $$ = new cdk::sequence_node(LINE, $1); }
          | exprs ',' expr              { $$ = new cdk::sequence_node(LINE, $3, $1); }
          ;

expr : /* PRIMITIVE EXPRESSION */   
       tINTEGER                         { $$ = new cdk::integer_node(LINE, $1); }
     | tDOUBLE                          { $$ = new cdk::double_node(LINE, $1); }
     | string                           { $$ = new cdk::string_node(LINE, $1); }
     | tNULLPTR                         { $$ = new mml::null_node(LINE); }
     /* PRIMARY EXPRESSION */
     | '(' expr ')'                     { $$ = $2; }
     | '[' expr ']'                     { $$ = new mml::stack_alloc_node(LINE, $2); }
     /* UNARY EXPRESSION */
     | '-' expr %prec tUNARY            { $$ = new cdk::neg_node(LINE, $2); }
     | '+' expr %prec tUNARY            { $$ = new mml::identity_node(LINE, $2); }
     /* ARITHMETIC EXPRESSIONS */
     | expr '+' expr	               { $$ = new cdk::add_node(LINE, $1, $3); }
     | expr '-' expr	               { $$ = new cdk::sub_node(LINE, $1, $3); }
     | expr '*' expr	               { $$ = new cdk::mul_node(LINE, $1, $3); }
     | expr '/' expr	               { $$ = new cdk::div_node(LINE, $1, $3); }
     | expr '%' expr	               { $$ = new cdk::mod_node(LINE, $1, $3); }
     /* LOGICAL EXPRESSIONS */
     | expr '<' expr	               { $$ = new cdk::lt_node(LINE, $1, $3); }
     | expr '>' expr	               { $$ = new cdk::gt_node(LINE, $1, $3); }
     | expr tGE expr	               { $$ = new cdk::ge_node(LINE, $1, $3); }
     | expr tLE expr                    { $$ = new cdk::le_node(LINE, $1, $3); }
     | expr tNE expr	               { $$ = new cdk::ne_node(LINE, $1, $3); }
     | expr tEQ expr	               { $$ = new cdk::eq_node(LINE, $1, $3); }
     /* LOGICAL EXPRESSIONS */
     | expr tAND  expr                  { $$ = new cdk::and_node(LINE, $1, $3); }
     | expr tOR   expr                  { $$ = new cdk::or_node (LINE, $1, $3); }
     | '~' expr                         { $$ = new cdk::not_node(LINE, $2); }
     /* ASSIGNMENTS */
     | lval '=' expr                    { $$ = new cdk::assignment_node(LINE, $1, $3); }
     /* LEFT VALUES */
     | lval                             { $$ = new cdk::rvalue_node(LINE, $1); }
     /* OTHER EXPRESSION */
     | tINPUT                           { $$ = new mml::input_node(LINE); }
     | tSIZEOF '(' expr ')'             { $$ = new mml::sizeof_node(LINE, $3); }
     | lval '?'                         { $$ = new mml::address_of_node(LINE, $1); }
     | func_def                         { $$ = $1; }
     | func_call                        { $$ = $1; }
     ;
%%
