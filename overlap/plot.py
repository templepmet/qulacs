from collections import defaultdict

import matplotlib.pyplot as plt
import numpy as np

methods = []
qubits = []
time_data = defaultdict(lambda: [])

with open("result_overlap.txt") as f:
    for row in f:
        items = row.split(", ")
        q = int(items[0])
        n_state = int(items[1])
        method = items[2]
        t = float(items[3])

        if method == "only_cpu":
            qubits.append(q)
        time_data[method].append(t)
        if method not in methods:
            methods.append(method)

print(methods)
print(qubits)
print(time_data)

max_speedup = 0
max_qubits = 0
for i, q in enumerate(qubits):
    speedup = (
        min(time_data["only_cpu"][i], time_data["only_gpu"][i])
        / time_data["overlap"][i]
    )
    if max_speedup < speedup:
        max_speedup = speedup
        max_qubits = q

print(max_speedup, max_qubits)

plt.figure()
plt.grid()
plt.title("GateFabric N/2 layer, 10 Update")
plt.xticks(qubits)
plt.yscale("log")
plt.xlabel("N = Number of qubits")
plt.ylabel("Sum Time of Update [s]")

for method in methods:
    plt.plot(qubits, time_data[method], "-o", label=method)

plt.legend()
plt.savefig(f"result.png")
