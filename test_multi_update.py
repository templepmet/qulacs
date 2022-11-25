import time

from qulacs import (MultipleQuantumCircuitSimulator, QuantumCircuit,
                    QuantumState)

n = 26
state0 = QuantumState(n)
state1 = QuantumState(n)
state2 = QuantumState(n)

circuit1 = QuantumCircuit(n)
circuit2 = QuantumCircuit(n)
circuit1.add_H_gate(0)
circuit1.add_CNOT_gate(0, 1)
circuit2.add_H_gate(0)
circuit2.add_CNOT_gate(0, 1)

t = time.time()
circuit1.update_quantum_state(state0)
time_single = time.time() - t

sim = MultipleQuantumCircuitSimulator()
sim.addQuantumCircuitState(circuit1, state1)
sim.addQuantumCircuitState(circuit2, state2)

t = time.time()
sim.simulate()
time_multi = time.time() - t
time_per = time_multi / 2

vec0 = state0.get_vector()
vec1 = state1.get_vector()
vec2 = state2.get_vector()
assert (((vec0 - vec1) < 1e-10).all())
assert (((vec0 - vec2) < 1e-10).all())

print(f"{time_single=}")
print(f"{time_per=}, {time_multi=}")

