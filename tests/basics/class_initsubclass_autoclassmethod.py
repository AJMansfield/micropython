try:

    class T:
        pass

    T().__class__.__name__
    getattr
except AttributeError:
    print("SKIP")  # skip failing on minimal ports
    raise SystemExit


def regularize_spelling(text):
    # for regularizing across the CPython "method" vs Micropython "bound_method" spelling for the name of the type of a bound classmethod
    prefix="bound_"
    if text.startswith(prefix):
        return text[len(prefix) :]
    return text


class A:
    def __init_subclass__(cls):
        pass

    @classmethod
    def manual_decorated(cls):
        pass


a = type(A.__init_subclass__).__name__
b = type(A.manual_decorated).__name__

print(regularize_spelling(a))
print(regularize_spelling(b))
print(a == b)
