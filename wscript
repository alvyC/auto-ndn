# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
import os

def options(opt):
    opt.load(['compiler_cxx', 'gnu_dirs'])
    opt.load(['default-compiler-flags', 'boost'], tooldir=['.waf-tools'])

def configure(conf):
    conf.load(['compiler_cxx', 'gnu_dirs',
               'default-compiler-flags', 'boost'])

    if not os.environ.has_key('PKG_CONFIG_PATH'):
        os.environ['PKG_CONFIG_PATH'] = ':'.join([
            '/usr/lib/pkgconfig',
            '/usr/local/lib/pkgconfig',
            '/opt/local/lib/pkgconfig'])
    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'],
                   uselib_store='NDN_CXX', mandatory=True)
    boost_libs = 'system chrono program_options iostreams thread regex filesystem'

    conf.check_boost(lib=boost_libs)

    #conf.env.append_value('INCLUDES', ['src'])

def build(bld):
    #bld.program(
        #features = 'cxx',
    #    target = 'communication',
    #    source = 'src/Calibration/motion.cpp',
        #use = 'NDN_CXX BOOST',
    #    includes = 'src/Calibration/pca9685/',
    #    export_includes='src/Calibration/pca9685/',
    #    )

#    bld.program(
#        features = 'cxx',
#        target = 'communication',
#        source = 'src/communication/communication.cpp',
#        use = 'NDN_CXX BOOST',
#        #includes = 'src/calibration',
#        #export_includes='src/calibration',
#    )

    comm_objects = bld(
        target='comm-objects',
        name='comm-objects',
        features='cxx',
        source=bld.path.ant_glob(['src/communication/*.cpp']),
        use='NDN_CXX BOOST',
        includes = 'src/calibration',
        export_includes='src/calibration',
        )

    calib_objects = bld(
        target='calib-objects',
        name='calib-objects',
        features='cxx',
        source=bld.path.ant_glob(['src/calibration/*.cpp']),
        )

    control_objects = bld(
        target='control-objects',
        name='control-objects',
        features='cxx',
        source=bld.path.ant_glob(['src/control/*.cpp']),
        use='NDN_CXX BOOST',
        includes=bld.path.ant_glob(['src/*']),
        export_includes=bld.path.ant_glob(['src/*']),
        )

