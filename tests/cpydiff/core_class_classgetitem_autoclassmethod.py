"""
categories: Core,Classes
description: __class_getitem__ isn't an implicit classmethod.
cause: MicroPython does not currently implement PEP 560. __class_getitem__ is not currently in the list of special-cased class/static methods.
workaround: Decorate declarations of __class_getitem__ with @classmethod.
"""

def regularize_spelling(text, prefix="bound_"):
    # for regularizing across the CPython "method" vs Micropython "bound_method" spelling for the type of a bound classmethod
    if text.startswith(prefix):
        return text[len(prefix):]
    return text

class A:
    def __class_getitem__(cls, key):
        pass
    @classmethod
    def manual_decorated(cls):
        pass

a = type(A.__class_getitem__).__name__
b = type(A.manual_decorated).__name__

print(regularize_spelling(a))
print(regularize_spelling(b))
if a != b:
    print("FAIL")