fovris(1) --- 4QL query language interpreter
===========================================================

## SYNOPSIS

`fovris` [<options>] [<file> ...]

## DESCRIPTION

`fovris`
is a limited 4QL shell. It can be used interactively (default) or noninteractively. Noninteractive usage is active when the `--query` option is passed. The results are then printed in the format specified by the `--format` option.

## FILES

`fovris` expects valid 4QL files.
Basic 4QL file structure:

    module <name>:
        domains:
            <domain_type> <alias>.
            ...
        relations:
            <predicate>(<alias|domain_type>,...).
            ...
        rules:
            <predicate>(<term>,...).
            ...
        facts:
            <predicate>(<term>,...).
            ...
        end.

The domain_type can be one of:

    - literal
    - string
    - datetime
    - date
    - integer
    - real

## OPTIONS

  * `-f`, `--format` <format>:
    Specify the query result format, where the <format> value is one of the following: json, csv, 4ql(default).

  * `-q`, `--query` <query>:
    Execute query and print the results to 
    the standard output. The `--query` option can 
    be specified multiple times. The <query> string
    must be in the following format: 
            module.predicate(term1,term2,...)

  * `-i`, `--import` <path>:
    Import a 4ql program from the file at the <path>.
    This option can be specified multiple times.
    The import order is preserved.

## FOURQL COMMANDS

List of commands available at the prompt:

  * `?`       :
  Print `help`.

  * `help`    :
  Print list of commands.

  * `import` <path>:
  Import 4ql program from the given file.

  * `clear` :
  Delete all modules.

  * <query>:
  Evaluate <query>.
