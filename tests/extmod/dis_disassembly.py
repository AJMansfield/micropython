try:
    (lambda: 0).__code__
except AttributeError:
    print("SKIP")
    raise SystemExit


import dis


def f():
    return a


bc = dis.Bytecode(f)
print(bc.dis())


dis.dis(f)
