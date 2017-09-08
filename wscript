# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

"""
Copyright (c) 2014-2017,  The University of Memphis,
                          Regents of the University of California,
                          Arizona Board of Regents.

This file is part of NLSR (Named-data Link State Routing).
See AUTHORS.md for complete list of NLSR authors and contributors.

NLSR is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

NLSR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
NLSR, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
"""

VERSION = "0.1"
APPNAME = "autondn"
# BUGREPORT = "https://redmine.named-data.net/projects/nlsr"
# URL = "https://named-data.net/doc/NLSR/"
# GIT_TAG_PREFIX = "NLSR-"

from waflib import Logs, Utils, Context
import os

def options(opt):
    opt.load(['compiler_cxx', 'gnu_dirs'])
    opt.load(['default-compiler-flags', 'coverage', 'sanitizers',
              'boost', 'doxygen', 'sphinx_build'],
            tooldir=['.waf-tools'])

    nlsropt = opt.add_option_group('NLSR Options')

    nlsropt.add_option('--with-tests', action='store_true', default=False, dest='with_tests',
                       help='''build unit tests''')


def configure(conf):
    conf.load(['compiler_cxx', 'gnu_dirs',
               'default-compiler-flags', 'boost'])

    if 'PKG_CONFIG_PATH' not in os.environ:
        os.environ['PKG_CONFIG_PATH'] = Utils.subst_vars('${LIBDIR}/pkgconfig', conf.env)

    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'],
                   uselib_store='NDN_CXX', mandatory=True)

    boost_libs = 'system chrono program_options iostreams thread regex filesystem'
    if conf.options.with_tests:
        conf.env['WITH_TESTS'] = 1
        conf.define('WITH_TESTS', 1);
        boost_libs += ' unit_test_framework'

    conf.check_boost(lib=boost_libs)

    conf.load('coverage')

    conf.write_config_header('config.hpp')

def build(bld):
    autondn_objects = bld(
        target='autondn-objects',
        name='autondn-objects',
        features='cxx',
        source=bld.path.ant_glob(['src/**/*.cpp'],
                                 excl=['src/main.cpp']),
        use='NDN_CXX BOOST',
        includes='. src',
        export_includes='. src',
        )

    autondn = bld(
        target='bin/autondn',
        features='cxx cxxprogram',
        source='src/main.cpp',
        use='autondn-objects',
        #lib=['wiringPi'],
        )

    if bld.env['WITH_TESTS']:
        bld.recurse('tests')

def dist(ctx):
    version(ctx)

def distcheck(ctx):
    version(ctx)
