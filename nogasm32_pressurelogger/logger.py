import serial, signal, sys
from telnetlib import Telnet
from datetime import datetime

def signal_handler(sig, frame):
    global receiver
    print('Closing')
    receiver.close()
    file.close()
    sys.exit(0)

mode = int(input("Serial[0] or Telnet[1]? "))


if mode == 0:
    interface = input("Interface? ")
    receiver = serial.Serial(interface)

elif mode == 1:
    interface = input("IP? ")
    receiver = Telnet(interface, 23)

else:
    print("Unknown")
    sys.exit(1)

signal.signal(signal.SIGINT, signal_handler)

filename = "log_" + str(int(datetime.utcnow().timestamp())) + ".log"
file = open(filename, "w")

while 1:
    if mode == 0:
        data = receiver.read()
        
    elif mode == 1:
        data = receiver.read_eager()

    print(data.decode("utf-8"))
    file.write(str(data.decode("utf-8")))