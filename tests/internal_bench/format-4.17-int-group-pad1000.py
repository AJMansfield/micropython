import bench


def test(num):
    f = "{:0>1000,d}"
    for i in range(num // 10):
        s = f.format(i)


bench.run(test)
