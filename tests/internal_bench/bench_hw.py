import utime


ITERS = 200000


def run(f):
    t = utime.ticks_us()
    f(ITERS)
    t = utime.ticks_us() - t
    s, us = divmod(t, 1_000_000)
    print(f"{s}.{us:06}")
