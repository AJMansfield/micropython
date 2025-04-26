#!/bin/bash
set -e

# Special ErrorFS so the test can induce arbitrary filesystem errors.
cat << EOF > "${TMP}/fs.py"
import os, vfs, errno

class ErrorFS:
    def mount(self, *a, **k):
        pass
    def umount(self, *a, **k):
        pass
    def chdir(self, *a, **k):
        pass
    def open(self, *a, **k):
        raise self.error

fs = ErrorFS()
vfs.mount(fs, '/fs')
os.chdir('/fs')
EOF

$MPREMOTE run "${TMP}/fs.py"
stderr=$(mktemp --tmpdir="${TMP}")

echo -----
$MPREMOTE resume exec "fs.error = Exception('error message')"
(
  $MPREMOTE resume cat :Exception.py || echo "expect error"
) 2> >(head -n1 >&2) # discard traceback specifics but keep main error message

for errno in ENOENT EISDIR EEXIST ENODEV EINVAL EPERM EOPNOTSUPP ; do
echo -----
$MPREMOTE resume exec "fs.error = OSError(errno.$errno, '${errno,,} message')"
$MPREMOTE resume cat :${errno,,}.py || echo "expect error"
$MPREMOTE resume exec "fs.error = OSError(errno.$errno)"
$MPREMOTE resume cat :${errno,,}.py || echo "expect error"
done

echo -----
# [Errno 30] EROFS is a standard errno not implemented by micropython
$MPREMOTE resume exec "fs.error = OSError(30, 'erofs message')"
$MPREMOTE resume cat :erofs.py || echo "expect error"
$MPREMOTE resume exec "fs.error = OSError(30)"
$MPREMOTE resume cat :erofs.py || echo "expect error"

echo -----
$MPREMOTE resume exec "fs.error = OSError(f'[Errno {errno.EISDIR}] ENOTDIR')"
$MPREMOTE resume cat :ehamlet.py || echo "expect error"

echo -----
$MPREMOTE resume exec "vfs.umount('/fs')"
