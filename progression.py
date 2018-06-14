#!/usr/bin/env python
#
# Copyright 2017

#
# runsolver_wrapper.py is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
# runsolver_wrapper.py is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.  You should have received a
# copy of the GNU General Public License along with
# runsolver_wrapper.py.  If not, see <http://www.gnu.org/licenses/>.
import argparse
import inspect
import json
import os
import signal
import sys
from subprocess import Popen, PIPE

import psutil

from detkdecomp_prog import det_progression


def handler(signum, frame):
    sys.stderr.write('SIGNAL received %s\n' % signum)
    current_process = psutil.Process()
    children = current_process.children(recursive=True)
    for child in children:
        sys.stderr.write('Child pid is %s\n' % (child.pid))
        sys.stderr.write('Killing child\n')
        os.kill(child.pid, 15)


signal.signal(signal.SIGTERM, handler)
signal.signal(signal.SIGINT, handler)


class dotdict(dict):
    """dot.notation access to dictionary attributes"""
    __getattr__ = dict.get
    __setattr__ = dict.__setitem__
    __delattr__ = dict.__delitem__


def is_valid_file(parser, arg):
    if not arg:
        parser.error('Missing file.')
    if not os.path.exists(arg):
        parser.error('The file "%s" does not exist!' % arg)


def parse_args():
    parser = argparse.ArgumentParser(description='%(prog)s -f instance')
    # parser.formatter_class._max_help_position = 120

    root_group = parser.add_mutually_exclusive_group()
    root_group.add_argument('-f', '--file', dest='instance', action='store', type=lambda x: os.path.realpath(x),
                            help='instance')
    parser.add_argument('-t', '--tmp-file', dest='tmp', action='store', type=lambda x: os.path.realpath(x),
                        help='ignored')
    parser.add_argument('-k', dest='k', action='store', type=lambda x: int(x), default=50,
                        help='Upper bound. [default=50]')
    parser.add_argument('--runid', dest='run', action='store', type=int,
                        help='ignored')
    args = parser.parse_args()
    is_valid_file(parser, args.instance)

    return args


def main(args):
    sys.stderr.write('%s %s\n' % (os.path.realpath(__file__), " ".join(sys.argv[1:])))
    sys.stderr.write('\n')

    dir_path = os.path.dirname(os.path.realpath(__file__))
    instance = args.instance

    i = args.k
    ret = det_progression(instance, i, dir_path)
    sys.stdout.write(json.dumps(ret, sort_keys=True))
    sys.stdout.write('\n')
    sys.stdout.flush()
    exit(0)


if __name__ == "__main__":
    args = parse_args()
    main(args)
