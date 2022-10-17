import time

import numpy as np

from qulacs import QuantumCircuit, QuantumState, QuantumStateGpu

n = 26


def get_circuit():
    circuit = QuantumCircuit(n)
    for i in range(n):
        circuit.add_H_gate(i)
    for i in range(n // 2):
        circuit.add_CNOT_gate(i * 2, i * 2 + 1)
    for i in range(n):
        circuit.add_H_gate(i)
    return circuit


def time_sync_cpu_gpu():
    state_cpu = QuantumState(n)
    state_cpu.set_zero_state()
    state_gpu = QuantumStateGpu(n)
    state_gpu.set_zero_state()
    circuit = get_circuit()

    t = time.time()
    start = t
    circuit.update_quantum_state(state_gpu)
    gpu_time = time.time() - t
    t = time.time()
    circuit.update_quantum_state(state_cpu)
    cpu_time = time.time() - t
    wall_time = time.time() - start

    vector_cpu = state_cpu.get_vector()
    vector_gpu = state_gpu.get_vector()
    assert ((vector_cpu - vector_gpu) < 1e-10).all()

    return {"wall_time": wall_time, "cpu_time": cpu_time, "gpu_time": gpu_time}


def time_async_cpu_gpu():
    state_cpu = QuantumState(n)
    state_cpu.set_zero_state()
    state_gpu = QuantumStateGpu(n)
    state_gpu.set_zero_state()
    circuit = get_circuit()

    t = time.time()
    start = t
    circuit.update_quantum_state_async(state_gpu)
    gpu_time = time.time() - t
    t = time.time()
    circuit.update_quantum_state(state_cpu)
    cpu_time = time.time() - t
    state_gpu.synchronize_update()
    wall_time = time.time() - start

    vector_cpu = state_cpu.get_vector()
    vector_gpu = state_gpu.get_vector()
    assert ((vector_cpu - vector_gpu) < 1e-10).all()

    return {"wall_time": wall_time, "cpu_time": cpu_time, "gpu_time": gpu_time}


def main():
    print(f"{time_async_cpu_gpu()=}")
    print(f"{time_sync_cpu_gpu()=}")


if __name__ == "__main__":
    main()
