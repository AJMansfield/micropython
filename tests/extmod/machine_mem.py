# test machine module

import struct
import array

bytes_native = struct.pack('@I', 0x44332211)
bytes_little = struct.pack('@I', 0x44332211)
if bytes_native != bytes_little:
    print("SKIP")
    raise SystemExit


try:
    from machine import mem8, mem16, mem32
    import uctypes

    buf = bytearray(b'\xff' * 1024)
    base = uctypes.addressof(buf)

except ImportError:

    class MemN:
        def __init__(self, basis, fmt):
            self.fmt = fmt
            self.itemsize = struct.calcsize(fmt)
            self.offsets = [None] * self.itemsize  # for unaligned accesses
            for i in range(self.itemsize):
                length = len(basis) - i
                length -= length % self.itemsize
                self.offsets[i] = basis[i : i + length].cast(fmt)

        def _calc_idx_off(self, key):
            if isinstance(key, slice):
                if key.step is not None and key.step != 1:
                    raise IndexError("slice step must be 1")
                start_idx, start_off = divmod(key.start, self.itemsize)
                stop_idx, stop_off = divmod(key.stop, self.itemsize)
                if start_off != stop_off:
                    raise IndexError("length must be a multiple of itemsize")
                return slice(start_idx, stop_idx), start_off
            else:
                return divmod(key, self.itemsize)

        def __getitem__(self, key):
            idx, off = self._calc_idx_off(key)
            return self.offsets[off][idx]

        def __setitem__(self, key, value):
            idx, off = self._calc_idx_off(key)
            try:
                self.offsets[off][idx] = value
            except TypeError:
                for i, v in enumerate(value):
                    if v is not None:
                        self.offsets[off][idx][i] = v

        def __delitem__(self, key):
            idx, off = self._calc_idx_off(key)
            del self.offsets[off][idx]

        def __getattr__(self, name):
            return getattr(self.offsets[0], name)

    buf = bytearray(b'\xff' * 1024)
    base = 0
    mem8 = memoryview(buf)
    mem16 = MemN(mem8, 'H')
    mem32 = MemN(mem8, 'I')


# verify no errors with open-ended slicing
mem8[: base + 1024]
mem8[base:]

print(mem8[base + 0])
print(mem16[base + 0])
print(mem32[base + 0])

mem8[base + 0] = 0x22

print(mem8[base + 0])
print(mem16[base + 0])
print(mem32[base + 0])

mem16[base + 4] = 0x4433

print(mem8[base + 4])
print(mem16[base + 4])
print(mem32[base + 4])

mem32[base + 8] = 0x88776655

print(mem8[base + 8])
print(mem16[base + 8])
print(mem32[base + 8])

mem8[base + 16 : base + 32] = b'\xaa' * 16

print(list(mem8[base + 16 : base + 32]))
print(list(mem16[base + 16 : base + 32]))
print(list(mem32[base + 16 : base + 32]))

mem16[base + 32 : base + 48] = [0xBBBB] * 8

print(list(mem8[base + 32 : base + 48]))
print(list(mem16[base + 32 : base + 48]))
print(list(mem32[base + 32 : base + 48]))

mem32[base + 48 : base + 64] = [0xCCCCCCCC] * 4

print(list(mem8[base + 48 : base + 64]))
print(list(mem16[base + 48 : base + 64]))
print(list(mem32[base + 48 : base + 64]))

mem32[base + 64 : base + 80] = [0xDDDDDDDD, None, 0xDDDDDDDD, None]

print(list(mem8[base + 64 : base + 80]))
print(list(mem16[base + 64 : base + 80]))
print(list(mem32[base + 64 : base + 80]))

print(buf)
