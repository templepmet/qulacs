import time

from qulacs import (MultipleQuantumCircuitSimulator, QuantumCircuit,
                    QuantumState, QuantumStateGpu)

print("start")

n = 26
m = 100

state = QuantumState(n)
circuit = QuantumCircuit(n)
circuit.add_H_gate(0)
circuit.add_CNOT_gate(0, 1)

# naive cpu
t = time.time()
for i in range(m):
    state.set_zero_state()
    circuit.update_quantum_state(state)
time_naive_cpu = time.time() - t

print(f"{time_naive_cpu=}")

# naive gpu
state_gpu = QuantumStateGpu(n)
t = time.time()
for i in range(m):
    state_gpu.set_zero_state()
    circuit.update_quantum_state(state_gpu)
time_naive_gpu = time.time() - t

print(f"{time_naive_gpu=}")

# enhance cpu&gpu
sim = MultipleQuantumCircuitSimulator()
for i in range(m):
    sim.addQuantumCircuitState(circuit, state_gpu)

t = time.time()
sim.simulate()
time_cpu_gpu_overlap = time.time() - t

# vec0 = state0.get_vector()
# vec1 = state1.get_vector()
# vec2 = state2.get_vector()
# assert (((vec0 - vec1) < 1e-10).all())
# assert (((vec0 - vec2) < 1e-10).all())

print(f"{time_cpu_gpu_overlap=}")
