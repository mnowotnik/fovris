#! /usr/bin/env python3

import sys, os, csv, subprocess, re

cdir = os.path.dirname(os.path.realpath(__file__))

FourqlPath = os.environ['FOURQL_PATH']

_ALGORITHMS = ['SN','N']


def main(args):
    for alg in _ALGORITHMS:
        single_pass(alg)

def single_pass(alg):
    count = 0
    passed = 0
    for root, dirs, filenames in os.walk(cdir):
        for f in filenames:
            if not f.endswith('.4ql'):
                continue

            f = os.path.join(root, f)

            result_csv = os.path.splitext(f)[0] + '.csv'
            result_csv = os.path.join(root, result_csv)

            assert (os.path.isfile(result_csv))

            if check_result(f, result_csv, alg):
                passed += 1
            count += 1

    print("{0}/{1} tests passed.".format(passed, count))


def check_result(in_f, csv_f, alg):
    correct_result = get_correct_result_rows(csv_f)
    query = get_query(in_f)
    return check_result_for_alg(query, correct_result, in_f, alg)


def get_correct_result_rows(csv_path):
    with open(csv_path) as csv_file:
        reader = csv.reader(csv_file)
        correct_result = (tuple(row) for row in reader if row)
        correct_result = set(correct_result)
        return correct_result


def check_result_for_alg(query, correct_result, in_f, alg):

    fql_out = subprocess.check_output([FourqlPath, "-f", "csv", "-i", in_f,
                                       "-a", alg])
    fql_out = fql_out.decode("utf-8")
    fql_out = re.sub(r'#.*\n', '', fql_out)
    fql_out = [row.strip() for row in fql_out.split('\n') if row.strip()]

    reader_fql = csv.reader(fql_out)

    fovris_result = tuple(tuple(row) for row in reader_fql)
    fovris_result = set(fovris_result)

    diff_wrong = fovris_result - correct_result
    diff_missing = correct_result - fovris_result

    if diff_wrong or diff_missing:
        print('---------')
        print("File: %s" % in_f.rsplit('/', 1)[-1])
        print("Query: %s" % query)
        print("Algorithm: %s" % alg)

    if diff_missing:
        print("Missing facts:")
        for f in diff_missing:
            print('  ', end='')
            print_terms(f)

    if diff_wrong:
        print("Wrong facts:")
        for f in diff_wrong:
            print('  ', end='')
            print_terms(f)

    if diff_wrong or diff_missing:
        return False

    return True


def get_query(script_4ql):
    query = None
    with open(script_4ql) as input_file:
        for line in input_file:
            m = re.search(r'([^\s]+)\s*\?', line)
            if m:
                query = m.group(1)
                break

    assert (query)

    return query


def print_terms(terms):
    for i, term in enumerate(terms):
        if i > 0:
            print(',', end='')
        print(term, end='')
    print()


if __name__ == '__main__':
    main(sys.argv)
