include("$(PORT_DIR)/variants/manifest.py")

include("$(MPY_DIR)/extmod/asyncio")

# freeze in EVERY micropython-lib package
import os, sys, glob

for arg in sys.argv:
    if arg.startswith("MPY_LIB_DIR="):
        MPY_LIB_DIR = arg.split("=", 1)[1]
        break
else:
    MPY_LIB_DIR = None

MPY_LIB_DIR = os.path.abspath(os.path.join("../..", MPY_LIB_DIR))

mpy_all_libs = sum(
    (
        glob.glob(os.path.join(MPY_LIB_DIR, "micropython/*/manifest.py")),
        glob.glob(os.path.join(MPY_LIB_DIR, "python-stdlib/*/manifest.py")),
        glob.glob(os.path.join(MPY_LIB_DIR, "python-ecosys/*/manifest.py")),
    ),
    start=[],
)
for path in mpy_all_libs:
    pkg_name = path.split(os.path.sep)[-2]
    # print(f"require({pkg_name!r})")
    require(pkg_name)
