from os.path import join, isfile, abspath

Import("env")

WORKDIR = abspath("./")
libdir = join(WORKDIR, ".pio/libdeps/nano33ble/Arduino_LSM9DS1")
patchflag = join(libdir, ".patched")
patchfile = join(WORKDIR, "patches/1-lsm9ds1-80hz-mag.patch")

if not isfile(patchflag):
    originalfile = join(libdir, "src/LSM9DS1.cpp")
    assert isfile(originalfile) and isfile(patchfile)
    env.Execute("patch %s %s" % (originalfile, patchfile))

    def _touch(path):
            with open(path, "w") as fp:
                fp.write("")
    
    env.Execute(lambda *args, **kwargs: _touch(patchflag))