from decimal import Decimal
from matplotlib import pyplot as plt
from celluloid import Camera

fig = plt.figure(figsize=(20, 20))
plt.axis((0, 20, 0, 20))
camera = Camera(fig)

with open("C:\Parallel\lab1\output.txt") as f:
    for line in f:
        res = any(chr.isdigit() for chr in line)
        if res:
            values = list(map(Decimal, line.rstrip('\n,').split()[3:]))
            xs, ys = [], []
            for i in range(len(values)):
                if i % 2 == 0:
                    xs.append(values[i])
                else:
                    ys.append(values[i])
            plt.plot(xs, ys, 'ro')
            camera.snap()

animation = camera.animate(interval=2000)
plt.show()