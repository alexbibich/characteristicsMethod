import csv
import matplotlib.pyplot as plt
import time

x = list()
Ro = list()

with open("res.csv", encoding='utf-8') as myFile:
	file_reader = csv.reader(myFile, delimiter=";")

	count = 1

	for row in file_reader:
		x.append(row[0])
		Ro.append(row[1])

	for i in range(0,100,10):
		plt.plot(x[i:(i+10)], Ro[i:(i+10)])
		
		plt.grid(visible=True)
		time.sleep(1)

	plt.show()