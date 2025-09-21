import bench


def test(num):
    f = "{: >680d}"
    for i in range(num // 10):
        s = f.format(i)


bench.run(test)
