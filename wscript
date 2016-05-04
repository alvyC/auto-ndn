# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

from waflib import Utils

import os

def options(opt):
    opt.load(['compiler_cxx', 'gnu_dirs'])
    opt.load(['default-compiler-flags', 'boost'], tooldir=['.waf-tools'])

def configure(conf):
    conf.load(['compiler_cxx', 'gnu_dirs',
               'default-compiler-flags', 'boost'])

    if 'PKG_CONFIG_PATH' not in os.environ:
        os.environ['PKG_CONFIG_PATH'] = Utils.subst_vars('${LIBDIR}/pkgconfig', conf.env)

    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'],
                   uselib_store='NDN_CXX', mandatory=True)
    boost_libs = 'system chrono program_options iostreams thread regex filesystem'

    conf.check_boost(lib=boost_libs)

    conf.env.append_value('INCLUDES', ['src/calibration', 'src/communication', 'src/control'])

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
        lib=['wiringPi']
        )
