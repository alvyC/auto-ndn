# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

APPNAME = "auto-ndn"

from waflib import Logs, Utils, Context
import os

def options(opt):
    opt.load(['compiler_cxx', 'gnu_dirs'])
    opt.load(['default-compiler-flags', 'coverage', 'sanitizers',
              'boost'],
            tooldir=['.waf-tools'])

    autondnopt=opt.add_option_group('Autondn Options')
    autondnopt.add_option('--with-tests', action='store_true', default=False, dest='with_tests',
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
        cxxflags = '-DBOOST_LOG_DYN_LINK'
        )

    autondn = bld(
        target='bin/autondn',
        features='cxx cxxprogram',
        source='src/main.cpp',
        use='autondn-objects',
        cxxflags = '-DBOOST_LOG_DYN_LINK'
        )

    if bld.env['WITH_TESTS']:
        bld.recurse('tests')
