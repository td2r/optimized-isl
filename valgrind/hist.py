import os
import sys
import numpy as np
import matplotlib.pyplot as plt

if len(sys.argv) != 2:
    print('Usage: python3 hist.py DATA_FILEPATH')
    exit(1)

filepath =  sys.argv[1]
filename = os.path.basename(filepath)
testcase = os.path.splitext(filename)[0]
random_data = testcase == 'Random'

args = []
f_values = [] # optimized
g_values = [] # cgal

with open(filepath, 'r') as f:
    for line in f.readlines():
        values = line.strip().split()
        args.append(int(values[0].split('/')[1]))
        if line.startswith('Optimized'):
            f_values.append([int(v) for v in values[1:]])
        elif line.startswith('CGAL'):
            g_values.append([int(v) for v in values[1:]])

# each arg appears twice, uniq it
args = np.array(sorted(set(args)))

f_means = np.mean(f_values, axis=1)
g_means = np.mean(g_values, axis=1)
f_stds = np.std(f_values, axis=1) if random_data else None
g_stds = np.std(g_values, axis=1) if random_data else None
capsize = 10 if random_data else 0.0

width = 0.35
ind = np.arange(len(args))

fig = plt.figure()
ax = fig.add_subplot(111)

f_bar = ax.bar(ind, f_means, width, yerr=f_stds, color='royalblue', capsize=capsize)
g_bar = ax.bar(ind + width, g_means, width, yerr=g_stds, color='seagreen', capsize=capsize)

ax.set_ylabel('Bytes')
ax.set_title(testcase)
ax.set_xticks(ind + width / 2)
ax.set_xticklabels(args)
ax.legend((f_bar[0], g_bar[0]), ('optimized', 'CGAL'))

plt.show()
