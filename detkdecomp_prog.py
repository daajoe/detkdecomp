import re
import sys

from subprocess import Popen, PIPE

import os

import time

detkdecomp_re = {
    "time": ("float", re.compile(r"^Real time \(s\): (?P<val>[0-9]+(\.[0-9]+)?)$"), lambda x: x),
    "memerror": ("string", re.compile(r"^Maximum VSize (?P<val>exceeded): sending SIGTERM then SIGKILL"), lambda x: x),
    "parse_wall": ("float", re.compile(r"^Parsing input file done in (?P<val>[0-9]+(\.[0-9]+)) sec"), lambda x: x),
    "hgbuild_wall": (
        "float", re.compile(r"^Building\s*hypergraph\s*done\s*in\s*(?P<val>[0-9]+(\.[0-9]+)*)\s*sec."), lambda x: x),
    "solve_wall": (
        "float", re.compile(r"Building hypertree done in (?P<val>[0-9]+(\.[0-9]+)*) sec \(hypertree-width: [0-9]+\)."),
        lambda x: x),
    "verify_wall": (
        "float", re.compile(r"^Checking hypertree conditions done in (?P<val>[0-9]+(\.[0-9]+)*) sec."), lambda x: x),
    "num_variables": (
        "int", re.compile(
            r"^Parsing\s*input\s*file\s*done\s*in\s*[0-9]+(\.[0-9]+)*\s*sec\s*\(([0-9]+)\s*atoms,\s*(?P<val>[0-9]+)\s*variables\)."),
        lambda x: x),
    "num_hyperedges": (
        "int", re.compile(
            r"^Parsing\s*input\s*file\s*done\s*in\s*[0-9]+(\.[0-9]+)*\s*sec\s*\((?P<val>[0-9]+)\s*atoms,\s*([0-9]+)\s*variables\)."),
        lambda x: x),
    "objective": (
        "int", re.compile(r"Building hypertree done in [0-9]+(\.[0-9]+)* sec \(hypertree-width: (?P<val>[0-9]+)\)."),
        lambda x: int(x)),
    "cond1": ("string", re.compile(r"^Condition 1: (?P<val>satisfied)."), lambda x: x),
    "cond2": ("string", re.compile(r"^Condition 2: (?P<val>satisfied)."), lambda x: x),
    "cond3": ("string", re.compile(r"^Condition 3: (?P<val>satisfied)."), lambda x: x),
    "cond4": ("string", re.compile(r"^Condition 4: (?P<val>satisfied)."), lambda x: x),
    "unsat": (
        "boolean", re.compile(r"^Hypertree of width [0-9]+ (?P<val>not) found in [0-9]+(\.[0-9]+)* sec."),
        lambda x: True)
}


def result_parser(output):
    ret = {}
    for line in output.splitlines():
        for val, reg in detkdecomp_re.items():
            m = reg[1].match(line)
            if m:
                ret[val] = reg[2](m.group("val"))

    try:
        if ret['cond1'] == ret['cond2'] == ret['cond3'] == ret['cond4'] and ret['objective']:
            ret['solved'] = 1
    except KeyError:
        ret['solved'] = 0

    return ret


def det_progression(instance, i, cwd):
    ret = {
        'solver': 'detkdecomp_progression',
        'solved': 0,
        'objective': 'nan',
    }
    wall_start = time.time()
    inc = 1
    previous = i
    finished = False
    sat = False
    while True:
        cmd = '%s %s %s' % ('sources/detkdecomp', i, ''.join(instance))
        sys.stderr.write('COMMAND=%s\n' % cmd)

        p_solver = Popen(cmd, stdout=PIPE, stderr=PIPE, shell=True, close_fds=True, cwd=cwd)
        output, err = p_solver.communicate()
        sys.stderr.write('%s RETCODE %s\n' % ('*' * 40, '*' * 40))
        sys.stderr.write('ret=%s\n' % p_solver.returncode)
        sys.stderr.write('%s STDOUT %s\n' % ('*' * 40, '*' * 40))
        sys.stderr.write(output)
        sys.stderr.write('%s STDERR %s\n' % ('*' * 40, '*' * 40))
        sys.stderr.write(err)

        # TODO: parse return code
        run = result_parser(output)
        ret["run_%s" % inc] = run

        if 'unsat' in run:
            try:
                remove_tmp(instance)
            except OSError:
                pass
            finished = True
            break
        if 'objective' in run:
            sat = True

        i = run['objective']

        if i == 1:
            previous = i
            remove_tmp(instance)
            break
        else:
            previous = i

        inc += 1
        i -= 1

        sys.stderr.write('Continue with new objective %s\n' % i)
        try:
            remove_tmp(instance)
        except OSError as e:
            sys.stderr.write("Something went wrong while deleting the temp file. Exception was %s" % e)


    wall = time.time() - wall_start
    ret['wall'] = wall
    ret['objective'] = previous
    ret['num_variables'] = ret["run_%s" % inc]['num_variables']
    ret['num_hyperedges'] = ret["run_%s" % inc]['num_hyperedges']
    ret['solved'] = int(finished and sat)
    return ret


def remove_tmp(instance):
    # remove temp instance
    tmp = "%s.gml" % os.path.splitext(instance)[0]
    sys.stderr.write('TEMP INSTANCE was %s\n' % tmp)
    os.remove(tmp)
