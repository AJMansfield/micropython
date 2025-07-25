import re
import socket
from .transport_serial import SerialTransport
from .transport import TransportError

class SocketTransport(SerialTransport):
    def __init__(self, address, timeout=None):
        self.in_raw_repl = False
        self.use_raw_paste = True
        self.device_name = address
        self.mounted = False

        m = re.fullmatch(r"(?P<host>[^/]*):(?P<port>\d+)", address)
        if m:
            groups = m.groupdict()
            address = (groups['host'] or None, int(groups['port']))
            self.sock = socket.create_connection(address, timeout=timeout)
            self.sock.setblocking(False)
        elif hasattr(socket, 'AF_UNIX'): # unix file socket
            self.sock = socket.socket(socket.AF_UNIX)
            self.sock.settimeout(timeout)
            self.sock.connect(address)
            self.sock.setblocking(False)
        else:
            raise TransportError("unrecognised socket: " + address)
        self.__buf: bytes|None = None
        self.serial = self

    def close(self):
        self.sock.close()

    # Methods for emulating serial object:
    def inWaiting(self) -> bool:
        if self.__buf:
            return True
        try:
            self.__buf = self.sock.recv(1)
        except BlockingIOError:
            return False
        return bool(self.__buf)
    def read(self, bufsize: int) -> bytes:
        if bufsize == 0:
            return b''
        elif self.__buf:
            b = self.__buf
            if bufsize > 1:
                b += self.sock.recv(bufsize-1)
            self.__buf = None
            return b 
        else:
            return self.sock.recv(bufsize)
    def write(self, buf: bytes) -> int:
        return self.sock.send(buf)
    
    @property
    def fd(self):
        return self.sock.fileno()
    
