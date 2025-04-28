# test machine module

try:
    import machine

    machine.mem8
except:
    print("SKIP")
    raise SystemExit

import unittest


class Test(unittest.TestCase):
    def test_mem8_print(self):
        self.assertEqual(repr(machine.mem8), "<8-bit memory>")
        self.assertEqual(repr(machine.mem8[0x100:]), "mem8[0x100:]")
        self.assertEqual(repr(machine.mem8[:0x200]), "mem8[:0x200]")
        self.assertEqual(repr(machine.mem8[0x100:0x200]), "mem8[0x100:0x200]")
        self.assertEqual(repr(machine.mem16), "<16-bit memory>")
        self.assertEqual(repr(machine.mem16[0x100:]), "mem16[0x100:]")
        self.assertEqual(repr(machine.mem16[:0x200]), "mem16[:0x200]")
        self.assertEqual(repr(machine.mem16[0x100:0x200]), "mem16[0x100:0x200]")
        self.assertEqual(repr(machine.mem32), "<32-bit memory>")
        self.assertEqual(repr(machine.mem32[0x100:]), "mem32[0x100:]")
        self.assertEqual(repr(machine.mem32[:0x200]), "mem32[:0x200]")
        self.assertEqual(repr(machine.mem32[0x100:0x200]), "mem32[0x100:0x200]")

    def test_alignment(self):
        with self.assertRaises(ValueError):
            machine.mem16[1]

        with self.assertRaises(ValueError):
            machine.mem16[1] = 1

        with self.assertRaises(ValueError):
            machine.mem16[:3]

        with self.assertRaises(ValueError):
            machine.mem16[:3] = [0x0000, 0x0000]

    def test_operations(self):
        with self.assertRaises(TypeError):
            del machine.mem8[0]

        with self.assertRaises(NotImplementedError):
            machine.mem8[0:1:2]

        with self.assertRaises(NotImplementedError):
            machine.mem8[2:1]

        with self.assertRaises(TypeError):
            machine.mem8["hello"]

        with self.assertRaises(TypeError):
            machine.mem8["hello"] = 10


if __name__ == "__main__":
    unittest.main()
