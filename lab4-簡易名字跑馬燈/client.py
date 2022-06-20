import socket

HOST = ''
PORT = 8020

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))

while True:
    outdata = raw_input('please input message: ')   # ex:"1111001100010001" for 'A'
    print 'send: ' + outdata 
    s.send(outdata)
    
    if len(outdata) == 0: # connection closed
        s.close()
        print 'client closed connection.'
        break
    # print('recv: ' + indata.decode())