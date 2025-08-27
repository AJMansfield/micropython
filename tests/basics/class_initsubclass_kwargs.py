try:

    class T:
        pass

    T().__class__.__name__
    getattr
except AttributeError:
    print("SKIP")  # skip failing on minimal ports
    raise SystemExit


class Base:
    @classmethod
    def __init_subclass__(cls, arg=None, **kwargs):
        print("Base.__init_subclass__({}, arg={!r}, **{!r})".format(cls.__name__, arg, kwargs))


class A(Base, arg="arg"):
    pass
