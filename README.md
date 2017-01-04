# FOVRiS (Four-Valued Reasoning System)
--------

FOVRiS is a reasoning engine for [4QL](http://4ql.org) rule-based query language. It's based on
the version of the language denoted as 4QL<sup>*</sup>, which was presented in
the article „Partiality and Inconsistency in Agents’ Belief Bases”.

4QL is a logic programming language derived from Datalog. However, unlike Datalog,
it uses four valued logic to deal with inconsistencies and lack of information.

## Usage

FOVRiS ships with a simple command line interpreter and an extendable
C++11 library.

### Interpreter

The interpreter can read 4QL scripts and evaluate them to create an in-memory Knowledge
Base. It uses [linenoise-ng](https://github.com/arangodb/linenoise-ng) to provide autocompletion and history. 
After evaluation the database can be queried using a simple syntax:

`<module>.<relation>(<term>,...)`

To see instructions for the interpreter check the manpage ( `man man/fovris.1` ) or
print help `./fovris -h`.

### C++ library

The C++ library API is currently undocumented. See `examples/code`, the cli module or check
the headers in the `include/` directory.

## Syntax

The syntax of a 4QL program doesn't differ much from the ones written for Datalog. The
prevalent elements are still rules and facts. However, there are a few additions to
support module-based architecture and four-valued logic.

### Declaring a module

Every 4QL program consists of one or more modules. A module can contain:

- Domain aliases
- Relation definitions
- Rules
- Facts

Below you can see the well-known Datalog program that can compute ancestor relation, but translated to 4QL. 
Not much has changed except from explicitly stated term types in relations.
```
module p:
    domains:
        literal person.
    relations:
        parent(person,person).
        ancestor(person,person).
    rules:
        ancestor(X,Y):- ancestor(X,Z),parent(Z,Y).
        ancestor(X,Y):- parent(X,Y).
    facts:
        parent(alice,bob).
        parent(alice,bill).
        parent(bob,carol).
        parent(carol,dennis).
        parent(carol,david).
end.
```

After evaluating the `p` module, one can check whom is the Alice ancestor of by executing the following query:

```
p.ancestor(alice,X)
```

### Literal negation

Every literal can be negated. Be it in the body or in the head of a rule.

```
!a(X,Y) :- !b(X), c(Y).
```

Negated literals will evaluate to false logical value, whereas normal literals
will evaluate to true. Facts that are both true and false are considered to be *inconsistent*.

### Builtin operators

FOVRiS supports basic comparison operators `> < >= <= != =`, which are basically binary function
predicates. They can be defined instead of literals in the body of a rule like so:

```
a(X,Y) :- b(X), c(Y), X<Y.
```

Notice:

Every variable in a operator (or any other function predicate) must be also present in a safe
 literal within the body of a rule. Safe literals define queries that yields finite set of facts.

## External literals

Relations in previously evaluated modules can be queried. The name of the external literal
starts with the name of the name module that contains the relation:

```
module_name.relation_name(X,Y)
```

External literals can also be constrained to return facts with specific logical values.
The literal:

```
m.r(X,Y) in {true,false,incons}
```

will accept facts that are true, false or inconsistent.
Querying for unknown facts is unsafe. Every variable in such literal must also be present
in a safe literal within the body of a rule. For example:

```
m.r(X,Y) in {unknown,incons}, a(X), b(Y)
```

### Constant term types

FOVRiS supports a few primitives to define constant terms:

- integer
- real
- datetime
- string
- literal

*integer* and *real* have standard self-explanatory formats. 
*string* can be any text surrounded by quotes " ".
*literal* is a text identifier. It must begin with a letter or underscore that can be followed by letters, numbers or underscores.

*datetime* adheres to the naive ISO8601 format. For example:
```
rel(2016-12-31 12:30).
```

## Building

Currently, only building on Linux systems in supported. It could be possible to compile on Mac OS X, but it has not been tested. Compilation on Windows would require a few tweaks to the CMakeLists.

To build FOVRiS, you will need C++11 compliant compiler and CMake build tool >=2.8. Simply run the following command:

```bash
make CMAKE_BUILD_TYPE=Release
```
If you don't have proper compiler on the PATH, you can set it explicitly with:
```bash
CXX=g++-5 make
```

If you have Ninja build system installed, you 
can speed up the building process by running:
```bash
make BUILD_TYPE=Ninja
```

## Installation

Just run:

```bash
sudo make install
```

To install in a non-default directory add the env variable:

```bash
make install DESTDIR=/my/path
```

## Running tests

After building you can test if everything is alright by running:
```bash
make test
```
