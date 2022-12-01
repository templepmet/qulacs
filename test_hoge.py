import time

from qulacs import (MultipleQuantumCircuitSimulator, QuantumCircuit,
                    QuantumState, QuantumStateGpu)

n_state = 4
qubits = 4

circuit = QuantumCircuit(qubits)
circuit.add_H_gate(0)
circuit.add_CNOT_gate(0, 1)

sim = MultipleQuantumCircuitSimulator()
for i in range(n_state):
    sim.addQuantumCircuitState(circuit, qubits)

print(0)
sim.simulate()
print(1)

state_list = sim.get_state_list()
assert len(state_list) == n_state
print(2)

ref = state_list[0].get_vector()
print(3)
for i in range(n_state):
    print(state_list[i].get_device_name())
    vec = state_list[i].get_vector()
    print(vec)
    assert ((vec - ref) < 1e-10).all()
print(4)

print(f"{len(state_list)=}")
# for state in state_list:
#     del state

print(5)

del sim

print(6)