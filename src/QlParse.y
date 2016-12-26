%token_prefix TOK_

%include {

#include <cassert>
#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include "Lexer.h"
#include "UnsafeTerm.h"
#include "QlParse.h"
#include "QlParseState.h"
#include "QlParseUtils.h"

#define YYNOERRORRECOVERY 1 // turn off recovery after syntax error

using namespace fovris;

typedef std::string * Token;

void destruct_token(Token t){
        delete t;
}

std::unique_ptr<std::string>to_uniq(std::string *s){
   return std::unique_ptr<std::string>(s);
}

}


%extra_argument {QlParseState* ps}

//types
//--------------------------------------------------

%token_type {Token}

%type builtin_predicate {std::string*}
%type declaration_section {std::vector<RelationDef>*}
%type domain_section {DomainsMap*}
%type domains {DomainsMap*}
%type relation_section {std::vector<PRelationDef>*}
%type relations {std::vector<PRelationDef>*}
%type rule_section {std::vector<Rule>*}
%type rules {std::vector<Rule>*}
%type rule {Rule*}
%type rule_disjunct {RuleDisjunct*}
%type fact_section {std::vector<GroundLiteral>*}
%type facts {std::vector<GroundLiteral>*}
%type fact {GroundLiteral*}
%type relation {PRelationDef*}
%type rule_body {std::vector<RuleDisjunct>*}
%type types_list {std::vector<PValueType>*}
%type module {Module*}
%type module_body {ModuleBody*}
%type term {Term*}
%type domain {Domain*}
%type terms_list{std::vector<Term>*}
%type ground_term {Term*}
%type ground_terms_list{std::vector<Term>*}
%type ground_literal {GroundLiteral*}
%type literal {Literal*}
%type external_literal {RuleLiteral*}
%type rule_literal {RuleLiteral*}
%type data_type {TermType}
%type logic_value {TruthValue}
%type logic_list {std::vector<TruthValue>*}

//destructors
//--------------------------------------------------

%token_destructor{destruct_token($$);}

%destructor declaration_section {delete $$;}
%destructor domain_section {delete $$;}
%destructor domains {delete $$;}
%destructor relation_section {delete $$;}
%destructor relations {delete $$;}
%destructor rule_section {delete $$;}
%destructor rules {delete $$;}
%destructor rule {delete $$;}
%destructor fact_section {delete $$;}
%destructor facts {delete $$;}
%destructor fact {delete $$;}
%destructor relation {delete $$;}
%destructor rule_body {delete $$;}
%destructor types_list {delete $$;}
%destructor module {delete $$;}
%destructor module_body {delete $$;}
%destructor term {delete $$;}
%destructor domain {delete $$;}
%destructor terms_list{delete $$;}
%destructor ground_term {delete $$;}
%destructor ground_terms_list{delete $$;}
%destructor ground_literal {delete $$;}
%destructor literal {delete $$;}
%destructor external_literal {delete $$;}
%destructor rule_literal {delete $$;}
%destructor logic_list {delete $$;}


//error handling
//--------------------------------------------------

%syntax_error {

    if(ps->errState){
        return;
    }

#ifndef NDEBUG
    int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
    ps->errMsg += "Unexpected token " + std::string(yyTokenName[yymajor]);
    for (int i = 0,e=0; i < n; ++i) {
            int a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
            if (a < YYNSTATE + YYNRULE) {
                    if(e==0){
                        ps->errMsg += " Expected: ";
                    }else if(e>0){
                        ps->errMsg += " ,";
                    }

                    e++;
                    ps->errMsg +=  yyTokenName[i];
            }
    }
#endif
    ps->errState = true;
}

%parse_failure {
    ps->errState = true;
}

//rules
//--------------------------------------------------

program ::= blocks END.

blocks ::= blocks block.
blocks ::= block.
block ::= query.
block ::= module.

module  ::= MODULE ID(N) COLON module_body(B) MODULE_END DOT. {
        ps->modules.emplace_back(
            *N,
            std::move(B->relations),
            std::move(B->rules),
            std::move(B->facts)
        );
        delete B;
        destruct_token(N);
}

module ::= MODULE ID(N) COLON MODULE_END DOT. {
        ps->modules.emplace_back(
            *N
        );
        destruct_token(N);
}

module_body(B) ::= declaration_section(D) rule_section(R) fact_section(F).{
    B = new ModuleBody{std::move(*D),
        std::move(*R),
        std::move(*F)
    };

    delete R;
    delete F;
    delete D;
}

module_body(B) ::= declaration_section(D) rule_section(R).{
    B = new ModuleBody{std::move(*D),
        std::move(*R),
        {}
    };
    delete D;
    delete R;
}
module_body(B) ::= declaration_section(D) fact_section(F).{
    B = new ModuleBody{std::move(*D),
        {},
        std::move(*F)
    };
    delete D;
    delete F;
}

module_body(B) ::= declaration_section(D).{
    B = new ModuleBody{std::move(*D),
        {},{}
    };
    delete D;
}

declaration_section(S) ::= domain_section(D) relation_section(R).{
    auto resolveRet = resolveDomains(*D,*R);
    delete D;
    delete R;
    S = resolveRet.relations.release();
    if(!resolveRet.errMsg.empty()){
        ps->errState = true;
        ps->errMsg = resolveRet.errMsg;
    }
}

declaration_section(S) ::= relation_section(R).{
    auto resolveRet = resolveDomains({},*R);
    delete R;
    S = resolveRet.relations.release();
}

domain_section(S) ::= DOMAINS domains(V).{
    S=V;
}

domains(V) ::= domains(VV) domain(D).{
    VV->insert(std::move(*D));
    V = VV;
    delete D;
}
domains(V) ::= domain(D). {
    V = new DomainsMap();
    V->insert(std::move(*D));
    delete D;
}
domain(D) ::= data_type(T) ID(N) DOT. {
    D = new Domain(*N,T);
    destruct_token(N);
}

data_type(T)::= STRING_TYPE. {T=TermType::String;}
data_type(T)::= ID_TYPE. {T=TermType::Id;}
data_type(T)::= INTEGER_TYPE. {T=TermType::Integer;}
data_type(T)::= REAL_TYPE. {T=TermType::Real;}
data_type(T)::= DATE_TYPE. {T=TermType::Date;}
data_type(T)::= DATETIME_TYPE. {T=TermType::DateTime;}
data_type(T)::= LOGIC_TYPE. {T=TermType::Logic;}

relation_section(LHS) ::= RELATIONS relations(RS).{
    LHS = RS;
}

relations(RS) ::= relations(ORS) relation(R).{
    ORS->push_back(std::move(*R));
    RS = ORS;
    delete R;
}
relations(RS) ::= relation(R).{
    RS = new std::vector<PRelationDef>{std::move(*R)};
    delete R;
}
relation(D) ::= ID(P) LRB types_list(L) RRB DOT.{
    D = new PRelationDef{*P,std::move(*L)};
    destruct_token(P);
    delete L;
}

types_list(L) ::= types_list(P) COMMA ID(T).{
    P->push_back(PValueType(*T));
    L = P;
    destruct_token(T);
}

types_list(L) ::= types_list(P) COMMA data_type(T).{
    P->push_back(PValueType(T));
    L = P;
}

types_list(L) ::= data_type(T).{
    L = new std::vector<PValueType>();
    L->push_back(PValueType(T));
}

types_list(L) ::= ID(T).{
    L = new std::vector<PValueType>;
    L->push_back(PValueType(*T));
    destruct_token(T);
}

rule_section(RS) ::= RULES rules(R).{
    RS = R;
}

rules(RR) ::= rules(R) rule(RL).{
    R->push_back(std::move(*RL));
    RR = R;
    delete RL;
}

rules(RR) ::= rule(R).{
    RR = new std::vector<Rule>{std::move(*R)};
    delete R;
}

rule(R) ::= literal(H) IMPL rule_body(B) DOT.{
    R = new Rule{std::move(*H),std::move(*B)};
    delete H;
    delete B;
}

rule_body(RB) ::= rule_disjunct(RD).{
    RB = new std::vector<RuleDisjunct>{std::move(*RD)};
    delete RD;
}

rule_body(RB) ::= rule_body(B) OR rule_disjunct(RD).{
    RB = B;
    RB->push_back(std::move(*RD));
    delete RD;
}

rule_disjunct(RB) ::= rule_disjunct(B) COMMA rule_literal(L).{
    RB = B;
    RB->getBody().push_back(std::move(*L));
    delete L;
}

rule_disjunct(B) ::= rule_literal(L). {
    B = new RuleDisjunct({std::move(*L)});
    delete L;
}

rule_literal(R)::= external_literal(F).{
    R = F;
}

rule_literal(L)::=  term(A) builtin_predicate(P) term(B). {
    L = new RuleLiteral(std::move(*P),std::vector<Term>{std::move(*A),std::move(*B)});
    delete A;
    delete B;
    delete P;
}

rule_literal(L)::=  term(A) NEG EQ term(B). {
    L = new RuleLiteral("!=",std::vector<Term>{std::move(*A),std::move(*B)});
    delete A;
    delete B;
}


builtin_predicate(P)::= GT|LT|GE|LE|EQ(X).{ P = X; }

rule_literal(R)::= literal(F).{
    R = new RuleLiteral(std::move(*F));
    delete F;
}

literal(L)::=  ID(P) LRB terms_list(T) RRB. {
    L = new Literal(std::move(*P),std::move(*T));
    delete T;
    destruct_token(P);
}

literal(L)::=  NEG ID(P) LRB terms_list(T) RRB. {
    L = new Literal(false,std::move(*P),std::move(*T));
    delete T;
    destruct_token(P);
}

external_literal(F)::=  ID(M) DOT ID(P) LRB terms_list(L) RRB. {
    F = new RuleLiteral(std::move(*M),std::move(*P),std::move(*L));
    delete L;
    destruct_token(P);
    destruct_token(M);
}

external_literal(F)::=  NEG ID(M) DOT ID(P) LRB terms_list(L) RRB. {
    F = new RuleLiteral(false,std::move(*M),std::move(*P),std::move(*L));
    delete L;
    destruct_token(P);
    destruct_token(M);
}

external_literal(F)::=  ID(M) DOT ID(P) LRB terms_list(L) RRB EQ logic_value(V). {
    F = new RuleLiteral(std::move(*M),std::move(*P),std::move(*L),{V});
    delete L;
    destruct_token(P);
    destruct_token(M);
}

external_literal(F)::=  ID(M) DOT ID(P) LRB terms_list(L) RRB NEG EQ logic_value(V). {
    F = new RuleLiteral(false,std::move(*M),std::move(*P),std::move(*L),{V});
    delete L;
    destruct_token(P);
    destruct_token(M);
}

external_literal(F)::=  ID(M) DOT ID(P) LRB terms_list(L) RRB IN LCB logic_list(LL) RCB. {
    F = new RuleLiteral(std::move(*M),std::move(*P),std::move(*L),*LL);
    delete L;
    delete LL;
    destruct_token(P);
    destruct_token(M);
}

external_literal(F)::=  NEG ID(M) DOT ID(P) LRB terms_list(L) RRB IN LCB logic_list(LL) RCB.{
    F = new RuleLiteral(false,std::move(*M),std::move(*P),std::move(*L),*LL);
    delete L;
    delete LL;
    destruct_token(P);
    destruct_token(M);
}

terms_list(L) ::= terms_list(P) COMMA term(T).{
    P->push_back(std::move(*T));
    delete T;
    L = P;
}

terms_list(L) ::= term(T).{
    L = new std::vector<Term>();
    L->push_back(std::move(*T));
    delete T;
}

term(T) ::= VAR(X).     { T = new UnsafeTerm(to_uniq(X),TermType::Var);}
term(T) ::= ground_term(X).      { T = X;}

logic_list(L) ::= logic_value(V).{
    L = new std::vector<TruthValue>;
    L->push_back(V);
}
logic_list(RL) ::= logic_list(L) COMMA logic_value(V).{
    L->push_back(V);
    RL = L;
}

logic_value(V) ::= TRUE. { V=TruthValue::True;}
logic_value(V) ::= FALSE. { V=TruthValue::False;}
logic_value(V) ::= INCONS. { V=TruthValue::Incons;}
logic_value(V) ::= UNKNOWN. { V=TruthValue::Unknown;}

fact_section(S) ::= FACTS facts(F).{
    S=F;
}
facts(R) ::= facts(FS) fact(F).{
    FS->push_back(std::move(*F));
    R = FS;
    delete F;
}

facts(R) ::= fact(F).{
    R = new std::vector<GroundLiteral>{std::move(*F)};
    delete F;
}

fact(F) ::= ground_literal(G) DOT. { F=G; }

ground_literal(L)::=  ID(P) LRB ground_terms_list(T) RRB. {
    L = new GroundLiteral(std::move(*P),std::move(*T));
    delete T;
    destruct_token(P);
}

ground_literal(L)::=  NEG ID(P) LRB ground_terms_list(T) RRB. {
    L = new GroundLiteral(false,std::move(*P),std::move(*T));
    delete T;
    destruct_token(P);
}

ground_terms_list(L) ::= ground_terms_list(P) COMMA ground_term(T).{
    P->push_back(std::move(*T));
    delete T;
    L = P;
}

ground_terms_list(L) ::= ground_term(T).{
    L = new std::vector<Term>();
    L->push_back(std::move(*T));
    delete T;
}

ground_term(T) ::= ID(X).      { T = new Term(to_uniq(X),TermType::Id);}
ground_term(T) ::= STRING(X).  { T = new Term(to_uniq(X),TermType::String);}
ground_term(T) ::= DATE(X).    { T = new Term(Date{*X}); destruct_token(X);}
ground_term(T) ::= DATETIME(X).{ T = new Term(DateTime{*X}); destruct_token(X);}
ground_term(T) ::= INTEGER(X). { T = new Term(std::stoi(*X)); destruct_token(X);}
ground_term(T) ::= logic_value(X). { T = new Term(X);}
ground_term(T) ::= REAL(X).    { T = new Term(std::stod(*X)); destruct_token(X);}

// Query
query ::= ID(M) DOT ID(R) LRB terms_list(T) RRB QUESTION.{
      ps->queries.emplace_back(std::move(*M),std::move(*R),std::move(*T));
      delete M;
      delete R;
      delete T;
}
