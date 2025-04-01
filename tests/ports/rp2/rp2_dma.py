# Test rp2.DMA functionality.

import sys
import time
import machine
import rp2

is_rp2350 = "RP2350" in sys.implementation._machine

src = bytes(i & 0xFF for i in range(16 * 1024))

print("# test basic usage")

dma = rp2.DMA()

# Test printing.
print(dma)

# Test pack_ctrl/unpack_ctrl.
ctrl_dict = rp2.DMA.unpack_ctrl(dma.pack_ctrl())
if is_rp2350:
    for entry in ("inc_read_rev", "inc_write_rev"):
        assert entry in ctrl_dict
        del ctrl_dict[entry]
for key, value in sorted(ctrl_dict.items()):
    print(key, value)

# Test register access.
dma.read = 0
dma.write = 0
dma.count = 0
dma.ctrl = dma.pack_ctrl()
print(dma.read, dma.write, dma.count, dma.ctrl & 0x01F, dma.channel, dma.registers)
dma.close()

# Test closing when already closed.
dma.close()

# Test using when closed.
try:
    dma.active()
    assert False
except ValueError:
    print("ValueError")

# Test simple memory copy.
print("# test memory copy")
dest = bytearray(1024)
dma = rp2.DMA()
dma.config(read=src, write=dest, count=len(dest) // 4, ctrl=dma.pack_ctrl(), trigger=False)
print(not any(dest))
dma.active(True)
while dma.active():
    pass
print(dest[:8], dest[-8:])
dma.close()


# Test time taken for a large memory copy.
def run_and_time_dma(dma):
    ticks_us = time.ticks_us
    irq_state = machine.disable_irq()
    t0 = ticks_us()
    dma.active(True)
    while dma.active():
        pass
    t1 = ticks_us()
    machine.enable_irq(irq_state)
    return time.ticks_diff(t1, t0)


def us_to_sysclks(dt, freq=machine.freq()):
    return dt * freq // 1_000_000


print("# test timing")
dest = bytearray(16 * 1024)
dma = rp2.DMA()
dma.read = src
dma.write = dest
dma.count = len(dest) // 4
dma.ctrl = dma.pack_ctrl()
dt = run_and_time_dma(dma)
sysclks = us_to_sysclks(dt)
expected_sysclk_range = range(
    4000, 20000
)  # expecting 4096 transfers @ 1-per-sysclk; plus possibly quite a lot of overhead
print(sysclks in expected_sysclk_range)
if sysclks not in expected_sysclk_range:
    print(sysclks, expected_sysclk_range)
print(dest[:8], dest[-8:])
dma.close()

# Test using .config(trigger=True).
print("# test immediate trigger")
dest = bytearray(1024)
dma = rp2.DMA()
dma.config(read=src, write=dest, count=len(dest) // 4, ctrl=dma.pack_ctrl(), trigger=True)
while dma.active():
    pass
print(dest[:8], dest[-8:])
dma.close()

# Test the DMA.irq() method.
print("# test irq")
dest = bytearray(1024)
dma = rp2.DMA()
dma.irq(lambda dma: print("irq fired", dma.irq().flags()))
dma.config(
    read=src, write=dest, count=len(dest) // 4, ctrl=dma.pack_ctrl(irq_quiet=0), trigger=True
)
while dma.active():
    pass
print(dest[:8], dest[-8:])
dma.close()
