#!/bin/bash
set -e

cat << EOF > "${TMP}/ramdisk.py"
class RAMBlockDev:
    def __init__(self, block_size, num_blocks):
        self.block_size = block_size
        self.data = bytearray(block_size * num_blocks)

    def readblocks(self, block_num, buf):
        for i in range(len(buf)):
            buf[i] = self.data[block_num * self.block_size + i]

    def writeblocks(self, block_num, buf):
        for i in range(len(buf)):
            self.data[block_num * self.block_size + i] = buf[i]

    def ioctl(self, op, arg):
        if op == 4: # get number of blocks
            return len(self.data) // self.block_size
        if op == 5: # get block size
            return self.block_size

import os
def add_fs(path):
    bdev = RAMBlockDev(512, 50)
    os.VfsFat.mkfs(bdev)
    os.mount(bdev, path)

add_fs('/ramdisk/errno_fs/ENOENT')
add_fs('/ramdisk/errno_fs/EISDIR')
add_fs('/ramdisk/errno_fs/EEXIST')
add_fs('/ramdisk/errno_fs/ENODEV')

add_fs('/ramdisk')
os.chdir('/ramdisk')


EOF

$MPREMOTE run "${TMP}/ramdisk.py"

# Set up paths for potentially-confusing filenames
$MPREMOTE resume mkdir :errno_files
$MPREMOTE resume touch :errno_files/ENOENT
$MPREMOTE resume touch :errno_files/EISDIR
$MPREMOTE resume touch :errno_files/EEXIST
$MPREMOTE resume touch :errno_files/ENODEV

$MPREMOTE resume mkdir :errno_dirs
$MPREMOTE resume mkdir :errno_dirs/ENOENT
$MPREMOTE resume mkdir :errno_dirs/EISDIR
$MPREMOTE resume mkdir :errno_dirs/EEXIST
$MPREMOTE resume mkdir :errno_dirs/ENODEV

$MPREMOTE resume mkdir :errno_empty

set +e # everything after this point should error; the question is _what_ error

echo -----
$MPREMOTE resume rmdir :errno_fs/ENOENT
$MPREMOTE resume rmdir :errno_fs/EISDIR
$MPREMOTE resume rmdir :errno_fs/EEXIST
$MPREMOTE resume rmdir :errno_fs/ENODEV

echo -----
$MPREMOTE resume cat :errno_fs/ENOENT
$MPREMOTE resume cat :errno_fs/EISDIR
$MPREMOTE resume cat :errno_fs/EEXIST
$MPREMOTE resume cat :errno_fs/ENODEV

echo -----
$MPREMOTE resume mkdir :errno_fs/ENOENT
$MPREMOTE resume mkdir :errno_fs/EISDIR
$MPREMOTE resume mkdir :errno_fs/EEXIST
$MPREMOTE resume mkdir :errno_fs/ENODEV

echo -----
$MPREMOTE resume touch :errno_fs/ENOENT
$MPREMOTE resume touch :errno_fs/EISDIR
$MPREMOTE resume touch :errno_fs/EEXIST
$MPREMOTE resume touch :errno_fs/ENODEV

echo -----
$MPREMOTE resume rm :errno_fs/ENOENT
$MPREMOTE resume rm :errno_fs/EISDIR
$MPREMOTE resume rm :errno_fs/EEXIST
$MPREMOTE resume rm :errno_fs/ENODEV



echo -----
$MPREMOTE resume ls :errno_files/ENOENT
$MPREMOTE resume ls :errno_files/EISDIR
$MPREMOTE resume ls :errno_files/EEXIST
$MPREMOTE resume ls :errno_files/ENODEV

echo -----
$MPREMOTE resume mkdir :errno_files/ENOENT
$MPREMOTE resume mkdir :errno_files/EISDIR
$MPREMOTE resume mkdir :errno_files/EEXIST
$MPREMOTE resume mkdir :errno_files/ENODEV

echo -----
$MPREMOTE resume rmdir :errno_files/ENOENT
$MPREMOTE resume rmdir :errno_files/EISDIR
$MPREMOTE resume rmdir :errno_files/EEXIST
$MPREMOTE resume rmdir :errno_files/ENODEV



echo -----
$MPREMOTE resume cat :errno_dirs/ENOENT
$MPREMOTE resume cat :errno_dirs/EISDIR
$MPREMOTE resume cat :errno_dirs/EEXIST
$MPREMOTE resume cat :errno_dirs/ENODEV

echo -----
$MPREMOTE resume mkdir :errno_dirs/ENOENT
$MPREMOTE resume mkdir :errno_dirs/EISDIR
$MPREMOTE resume mkdir :errno_dirs/EEXIST
$MPREMOTE resume mkdir :errno_dirs/ENODEV

echo -----
$MPREMOTE resume touch :errno_dirs/ENOENT
$MPREMOTE resume touch :errno_dirs/EISDIR
$MPREMOTE resume touch :errno_dirs/EEXIST
$MPREMOTE resume touch :errno_dirs/ENODEV

echo -----
$MPREMOTE resume rm :errno_dirs/ENOENT
$MPREMOTE resume rm :errno_dirs/EISDIR
$MPREMOTE resume rm :errno_dirs/EEXIST
$MPREMOTE resume rm :errno_dirs/ENODEV


true
