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



# Test whether a class member can remove itself from the class
class Vanish:
    def __set_name__(self, owner, name):
        delattr(owner, name)

class Houdini:
    trick = Vanish()

houdini = Houdini()

try:
    print("attr", houdini.trick)
except AttributeError:
    print("attr deleted")


# Test whether a class member can add other members to the class
class Cat_In_Hat:
    def __set_name__(self, owner, name):
        owner.thing1 = Thing(1)
        owner.thing2 = Thing(2)

class Thing:
    def __init__(self, key):
        self.key = key
    def __get__(self, instance, owner=None):
        return self.key

class House:
    cat = Cat_In_Hat()

house = House()
print("thing1", house.thing1)
print("thing2", house.thing2)

