#!/bin/bash

lens=(  1   2   3   5   7  10  15  22  33  47  68 100 150 220 330 470 680 1000)
for i in "${!lens[@]}"; do 
    len="${lens[$i]}"
    idx="$(printf "%02d" $i)"
    cat > format-2.$idx-int-space-pad$len.py <<EOF
import bench


def test(num):
    f = "{: ${len}d}"
    for i in range(num // 10):
        s = f.format(i)


bench.run(test)
EOF
    cat > format-3.$idx-int-unusual-pad$len.py <<EOF
import bench


def test(num):
    f = "{:!${len}d}"
    for i in range(num // 10):
        s = f.format(i)


bench.run(test)
EOF
done
