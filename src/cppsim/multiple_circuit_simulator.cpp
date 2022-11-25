#include "multiple_circuit_simulator.hpp"

#include <nvtx3/nvToolsExt.h>
#include <omp.h>

#include <cstdio>

#include "state.hpp"
#include "state_gpu.hpp"

MultipleQuantumCircuitSimulator::MultipleQuantumCircuitSimulator() {}

MultipleQuantumCircuitSimulator::~MultipleQuantumCircuitSimulator() {}

void MultipleQuantumCircuitSimulator::addQuantumCircuitState(
    QuantumCircuit* circuit, QuantumStateBase* state) {
    _circuit_vector.push_back(circuit);
    _state_vector.push_back(state);
}

void MultipleQuantumCircuitSimulator::simulate() {
    int max_threads = omp_get_max_threads();

    omp_set_nested(1);
    omp_set_num_threads(2);
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < _circuit_vector.size(); ++i) {
        int tid = omp_get_thread_num();
        if (tid == 0) {  // 2 thread for gpu
            omp_set_num_threads(2);
            if (_state_vector[i]->get_device_name() == "gpu") {
                _circuit_vector[i]->update_quantum_state(_state_vector[i]);
            } else {  // cpu -> gpu data transfer
                QuantumStateGpu* state_gpu;
                state_gpu->load(_state_vector[i]);
                _circuit_vector[i]->update_quantum_state(state_gpu);
                _state_vector[i]->load(state_gpu);
                delete state_gpu;
            }
        } else {  // other thread for cpu
            omp_set_num_threads(max_threads - 2);
            if (_state_vector[i]->get_device_name() ==
                "gpu") {  // gpu -> cpu data transfer
                QuantumState* state_cpu;
                state_cpu->load(_state_vector[i]);
                _circuit_vector[i]->update_quantum_state(state_cpu);
                _state_vector[i]->load(state_cpu);
                delete state_cpu;
            } else {
                _circuit_vector[i]->update_quantum_state(_state_vector[i]);
            }
        }
    }
    omp_set_num_threads(max_threads);
    omp_set_nested(0);
}
