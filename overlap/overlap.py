import os
import time

import numpy as np

from qulacs import (MultipleQuantumCircuitSimulator, QuantumCircuit,
                    QuantumState, QuantumStateGpu)


def get_ansatz(qubits):
    circuit = QuantumCircuit(qubits)
    for i in range(qubits):
        circuit.add_H_gate(i)
    for i in range(qubits // 2):
        circuit.add_CNOT_gate(i * 2, i * 2 + 1)
    for i in range(qubits):
        circuit.add_H_gate(i)
    return circuit


def only_cpu(qubits, circuit, n_state):
    t = time.time()
    state = [QuantumState(qubits) for i in range(n_state)]
    for i in range(n_state):
        circuit.update_quantum_state(state[i])
    t_update = time.time() - t
    # print("cpu:", t_update / n_state)
    return state, t_update


def only_gpu(qubits, circuit, n_state):
    t = time.time()
    state = [QuantumStateGpu(qubits) for i in range(n_state)]
    for i in range(n_state):
        circuit.update_quantum_state(state[i])
    t_update = time.time() - t
    # print("gpu:", t_update / n_state)
    return state, t_update


def overlap(qubits, circuit, n_state):
    sim = MultipleQuantumCircuitSimulator()
    for i in range(n_state):
        sim.addQuantumCircuit(circuit)
    t = time.time()
    sim.simulate()
    t_update = time.time() - t
    state = sim.get_state_list()
    return state, t_update


def main():
    min_qubits = 4
    max_qubits = 22
    # max_qubits = 26
    step_qubits = 2
    update_func_list = [only_cpu, only_gpu, overlap]
    n_state = 30

    result_file = open("result_overlap.txt", "w")

    for qubits in range(min_qubits, max_qubits + 1, step_qubits):
        circuit = get_ansatz(qubits)
        true_vector = None
        t_cpu = 0
        t_gpu = 0
        for update_func in update_func_list:
            state_list, t = update_func(qubits, circuit, n_state)
            if true_vector is None:
                true_vector = state_list[0].get_vector()
            consis = True
            for i in range(n_state):
                vec = state_list[i].get_vector()
                if not ((true_vector - vec) < 1e-9).all():
                    consis = False

            func_name = update_func.__name__
            if func_name == "only_cpu":
                t_cpu = t
            elif func_name == "only_gpu":
                t_gpu = t
            elif func_name == "overlap":
                ngpus = len([1 for s in state_list if s.get_device_name()=="gpu"])
                ncpus = len(state_list) - ngpus
                ideal_cpu = t_cpu / n_state * ncpus
                ideal_gpu = t_gpu / n_state * ngpus
                ideal = max(ideal_cpu, ideal_gpu)
                t_min = min(t_cpu, t_gpu)
                print("qubits", qubits, "cpus:", ncpus, "gpus:", ngpus, f"result: {t:.3f}", f"min_cpugpu: {t_min:.3f}", f"ideal: {ideal:.3f}", f"ideal_cpu: {ideal_cpu:.3f}", f"ideal_gpu: {ideal_gpu:.3f}")

            result_file.write(f"{qubits}, {n_state}, {func_name}, {t}, {consis}\n")
            result_file.flush()

    result_file.close()


if __name__ == "__main__":
    main()
