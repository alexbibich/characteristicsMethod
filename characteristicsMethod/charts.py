import csv
import matplotlib.pyplot as plt
from matplotlib import animation
import time


def animate(n):
	plt.cla()
	plt.autoscale(enable=False)
	plt.grid(visible=True)
	plt.xlabel('X')
	plt.ylabel('Density')
	ax.set_xlim(limX)
	ax.set_ylim(limY)
	line = plt.plot(x[n * N: (n+1) * N], y[n * N: (n + 1) * N], color='g')
	return line

fig = plt.figure()
ax = plt.gca()


with open("res.csv", encoding='utf-8') as myFile:
	file_reader = csv.reader(myFile, delimiter=";")
	t, x, y = zip(*([float(i) for i in row] for row in file_reader))
	
limX = [min(x) - 0.1 * (max(x) - min(x)), max(x) + 0.1 * (max(x) - min(x))]
limY = [min(y) - 0.1 * (max(y) - min(y)), max(y) + 0.1 * (max(y) - min(y))]

N = len(set(x))

anim = animation.FuncAnimation(fig, animate, frames=len(set(t)), interval=1, repeat=False)
plt.show()



	