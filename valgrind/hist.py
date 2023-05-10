import os
import sys
import numpy as np
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print('Usage: python3 hist.py DATA_FILEPATH [OUTPUT_FILE]')
    exit(1)

filepath =  sys.argv[1]
filename = os.path.basename(filepath)
testcase = os.path.splitext(filename)[0]
random_data = testcase == 'Random'

output_file = None if len(sys.argv) < 3 else sys.argv[2]

args = []
data = dict()

# Example of input data:
#  CGAL/10 1 2 3
#  CGAL/20 3 4 5
#  Optimized/10 1 2 3
#  Optimized/20 2 3 4
#  Optimized/30 3 4 5
# i.e. following regex:
# ${data_structure}/${argument} ${values...}
#
# Strictly two different data_structures in one file
#
# For each data structure, arguments for it must appear in file in increasing order
# i.e. for following:
#  CGAL/20 3 4 5
#  Optimized/10 1 2 3
#  CGAL/10 1 2 3
#  Optimized/20 2 3 4
# Optimized is okay, CGAL -- not
#
# Sets of arguments for structures can be different,
# but at least for one it must be subset of other's

with open(filepath, 'r') as f:
    for line in f.readlines():
        label, *y = line.strip().split()
        name, x = label.split('/')
        args.append(int(x))
        data.setdefault(name, []).append(list(map(int, y)))

names = data.keys()
assert len(names) == 2

def equalize_values_len(ys1, ys2):
    l1 = len(ys1)
    l2 = len(ys2)
    if l1 < l2:
        ys1, ys2 = ys2, ys1
    ys2.extend([[0] * len(ys2[0]) for _ in range(l1 - l2)])

equalize_values_len(*data.values())

# uniq
args = np.array(sorted(set(args)))

barcolor = dict(zip(names, ['royalblue', 'seagreen']))
means = {k : np.mean(v, axis=1) for k, v in data.items()}
stds = {k : np.std(v, axis=1) if random_data else None for k, v in data.items()}
capsize = 10 if random_data else 0.0

width = 0.35
ind = np.arange(len(args))
shiftw = dict(zip(names, [0, width]))

fig = plt.figure()
ax = fig.add_subplot(111)

bars = {k : ax.bar(ind + shiftw[k],
                   means[k],
                   width,
                   yerr=stds[k],
                   color=barcolor[k],
                   capsize=capsize) for k, v in data.items() }

ax.set_ylabel('Bytes')
ax.set_title(testcase)
ax.set_xticks(ind + width / 2)
ax.set_xticklabels(args)
ax.legend([bars[name][0] for name in names], names)

if output_file is None:
    plt.show()
else:
    plt.savefig(output_file)
