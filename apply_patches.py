from os.path import join, isfile, abspath

Import("env")

WORKDIR = abspath("./")
LIBDIR = join(WORKDIR, ".pio/libdeps/nano33ble")
PATCHDIR = join(WORKDIR, "patches")
PATCHFLAG = join(PATCHDIR, ".patched")

def patch(file, patch):
    originalfile = join(LIBDIR, file)
    patchfile = join(PATCHDIR, patch)
    assert isfile(originalfile) and isfile(patchfile)
    env.Execute("patch %s %s" % (originalfile, patchfile))

if not isfile(PATCHFLAG):
    patch("Arduino_LSM9DS1/src/LSM9DS1.cpp", "1-lsm9ds1-80hz-mag.patch")
    
    def _touch(path):
            with open(path, "w") as fp:
                fp.write("")
    
    env.Execute(lambda *args, **kwargs: _touch(PATCHFLAG))