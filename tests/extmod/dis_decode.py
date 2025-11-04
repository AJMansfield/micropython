def f(x, y):
    a = x + y * 6
    b = x - y * 8
    return a * b


try:
    f.__code__.co_code
except AttributeError:
    print("SKIP")
    raise SystemExit

try:
    import dis
    import _dis
except ImportError:
    print("SKIP")
    raise SystemExit

mv = memoryview(f.__code__.co_code)
while mv:
    op = _dis._decode(mv)
    print(op, dis.opname[op.opcode])
    print
    mv = mv[op.size :]
