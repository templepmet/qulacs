import time

import numpy as np

from qulacs import QuantumCircuit, QuantumState, QuantumStateGpu

n = 24
dim = 2**n
vector_ans = np.zeros(dim)
vector_ans[0] = np.sqrt(0.5)
vector_ans[3] = np.sqrt(0.5)


state_cpu = QuantumState(n)
state_cpu.set_zero_state()
state_gpu = QuantumStateGpu(n)
state_gpu.set_zero_state()

circuit = QuantumCircuit(n)
circuit.add_H_gate(0)
circuit.add_CNOT_gate(0, 1)

t = time.time()
circuit.update_quantum_state(state_cpu)
time_cpu = time.time() - t

t = time.time()
circuit.update_quantum_state_async(state_gpu)
time_gpu_call = time.time() - t
t = time.time()
state_gpu.synchronize_update()
time_gpu_update = time.time() - t

print(f"{time_cpu=}")
print(f"{time_gpu_call=}")
print(f"{time_gpu_update=}")

# vector = state.get_vector()

# print(vector)
# print(vector_ans)
# assert ((vector - vector_ans) < 1e-10).all()
