#! /usr/bin/env python3

import sys, os, csv, subprocess, time, re

cdir = os.path.dirname(os.path.realpath(__file__))

FourqlPath = os.environ['FOURQL_PATH']
Iterations = 1000

QueryRe = re.compile(r'query\s*:\s*(?P<query>[^\s]+)\s*', re.IGNORECASE)


def main(args):

    output = None
    if len(args)==2:
        output = args[1]

    for root, dirs, filenames in os.walk(cdir):
        for f in filenames:
            if not f.endswith('.4ql'):
                continue

            ffull = os.path.join(root, f)
            query = get_query(ffull)

            average_time = run_benchmark(ffull, query)
            print('File: %s. Average execution time %.4f.' % (f, average_time))
            if output:
                write_results(output, f, average_time)


def run_benchmark(f, query):

    args = [FourqlPath, '-i']
    if query:
        args.extend(['-q', query])

    DEVNULL = open(os.devnull, 'w')
    start_time = time.time()
    for i in range(Iterations):
        subprocess.call(args, stdout=DEVNULL)

    total_time = time.time() - start_time
    return total_time / Iterations


def get_query(f):

    first_line = ''
    with open(f) as ff:
        first_line = ff.readline()

    m = QueryRe.search(first_line)
    if m:
        return m.group('query')

    return ''


def write_results(output, fname, average_time):
    with open(output, 'w') as outputf:
        outputf.write('%s,%.4f' % (fname, average_time))


if __name__ == '__main__':
    main(sys.argv)
