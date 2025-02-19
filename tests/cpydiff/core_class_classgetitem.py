"""
categories: Core,Classes
description: __class_getitem__ is not called when indexing a class object.
cause: MicroPython does not currently implement PEP 560.
workaround: None
"""

class A:
    @classmethod
    def __class_getitem__(cls, key):
        print(f"{cls.__name__}.__class_getitem__({key!r})")

A['test']
