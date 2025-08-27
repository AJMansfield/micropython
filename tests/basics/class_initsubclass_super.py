try:

    class T:
        pass

    T().__class__.__name__
    getattr
except AttributeError:
    print("SKIP")  # skip failing on minimal ports
    raise SystemExit


# avoid failures in base-class setup
suppress = True

class NoSuper:
    @classmethod
    def __init_subclass__(cls, **kwargs):
        if not suppress:
            print("NoSuper.__init_subclass__({}, **{!r})".format(cls.__name__, kwargs))


class Base1:
    @classmethod
    def __init_subclass__(cls, **kwargs):
        if not suppress:
            print("Base1.__init_subclass__({}, **{!r})".format(cls.__name__, kwargs))
            super().__init_subclass__(**kwargs)


class NoSuper1(Base1):
    @classmethod
    def __init_subclass__(cls, **kwargs):
        if not suppress:
            print("NoSuper1.__init_subclass__({}, **{!r})".format(cls.__name__, kwargs))


class Child11(Base1):
    @classmethod
    def __init_subclass__(cls, **kwargs):
        if not suppress:
            print("Child11.__init_subclass__({}, **{!r})".format(cls.__name__, kwargs))
            super().__init_subclass__(**kwargs)


class Child12(Base1):
    @classmethod
    def __init_subclass__(cls, **kwargs):
        if not suppress:
            print("Child12.__init_subclass__({}, **{!r})".format(cls.__name__, kwargs))
            super().__init_subclass__(**kwargs)


class Base2:
    @classmethod
    def __init_subclass__(cls, **kwargs):
        if not suppress:
            print("Base2.__init_subclass__({}, **{!r})".format(cls.__name__, kwargs))
            super().__init_subclass__(**kwargs)

suppress = False


try:
    class A(Base1):
        pass
except Exception as e:
    print(e)

try:
    class B(Child11, Child12):
        pass
except Exception as e:
    print(e)

try:
    class C(Child11, Base2, Child12):
        pass
except Exception as e:
    print(e)

try:
    class D(Base1, NoSuper):
        pass
except Exception as e:
    print(e)

try:
    class E(NoSuper, Base1):
        pass
except Exception as e:
    print(e)

try:
    class F(NoSuper1):
        pass
except Exception as e:
    print(e)

