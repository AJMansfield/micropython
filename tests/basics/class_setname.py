# test calling __set_name__ during class creation
# https://docs.python.org/3/reference/datamodel.html#object.__set_name__

# skip failing on minimal port without descriptor support
try:
    class TestForClass:
        pass
    test = TestForClass()
    test.__class__
except AttributeError:
    print("SKIP")
    raise SystemExit



class A:
    def __set_name__(self, owner, name):
        print("owner", owner.__name__)
        print("name", name)

class B:
    a = A()

