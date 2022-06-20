'''
    python2 
    eleven segment display of two hex digits.
'''
import sys

if len(sys.argv) != 2:
	print "Usage: python seg.py [port]"
	exit()
port = int(sys.argv[1])

import Tkinter as tk
root = tk.Tk()
screen = tk.Canvas(root)
screen.grid()

# Order 7 segments clockwise from top left, with crossbar last.
# Coordinates of each segment are (x0, y0, x1, y1) 
# given as offsets from top left measured in segment lengths.
offsets = (
    (0, 0, 1, 0),  # top left
    (1, 0, 2, 0),  # top right
    (2, 0, 2, 1),  # upper right
    (2, 1, 2, 2),  # lower right
    (1, 2, 2, 2),  # bottom right
    (1, 2, 0, 2),  # bottom left
    (0, 1, 0, 2),  # lower left
    (0, 0, 0, 1),  # upper left
    (0, 0, 1, 1),  # upper left slash
    (1, 0, 1, 1),  # upper middle
    (1, 1, 2, 0),  # upper right slash
    (1, 1, 2, 1),  # middle right
    (1, 1, 2, 2),  # lower right slash
    (1, 2, 1, 1),  # upper middle
    (0, 2, 1, 1),  # lower left slash
    (0, 1, 1, 1)   # middle left
)

import socket
servsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
servsocket.bind(('', port))
servsocket.listen(2)
print "listen for client..."
(clisock, addr) = servsocket.accept()

class Digit:
    def __init__(self, canvas, x=10, y=10, length=100, width=10):
        self.canvas = canvas
        l = length
        self.segs = []
        for x0, y0, x1, y1 in offsets:
            self.segs.append(canvas.create_line(
                x + x0*l, y + y0*l, x + x1*l, y + y1*l,
                width=width, fill='white'))
    def show(self, digits):
        print "self.segs"
        print self.segs
        for iid, on in zip(self.segs, digits):
            self.canvas.itemconfigure(iid, fill = 'red' if on else 'white')

dig = Digit(screen)

def update():
    data = clisock.recv(16)
    #data = data.replace("\n", "")
    if data:
        d = []
        for i in data:
            d.append(int(i))  
        d = tuple(d)
        print d
        dig.show(d)
    root.after(100, update)
root.after(100, update)
root.mainloop()
servsocket.close()
clisock.close()

