import matplotlib.pyplot as plt
import os, sys

logs = []

for file in os.listdir():
    if file.endswith(".log"):
        logs.append(file)

if len(logs) < 1:
    print("No logs found!")
    sys.exit(1)

index = 0
for log in logs:
    print(log + "[" + str(index) + "]")
    index = index + 1

index = int(input("Select logfile: "))

file = open(logs[index], "r")

timestamps = []
pressures = []
activity = []
for tmp in file.read().splitlines():
    if(len(tmp) > 2) and (not tmp.startswith("pressure")) and (len(tmp.split(",")) == 3):
        data = tmp.split(",")
        timestamps.append(int(data[0]))
        pressures.append(int(data[1]))
        activity.append(int(data[2]))
file2 = open("test.log","w")
for pressure in pressures:
    file2.write(str(pressure) + "\n")
file2.close()
plt.plot(pressures)
plt.ylabel("pressure")
plt.xlabel("timestamp")
plt.show()