#include "multiple_circuit_simulator.hpp"

#include <nvtx3/nvToolsExt.h>
#include <omp.h>

#include <cstdio>

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
#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        if (tid == 0) {
            omp_set_num_threads(max_threads / 2);
            printf("call update 0\n");
            _circuit_vector[0]->update_quantum_state(_state_vector[0]);
            omp_set_num_threads(1);
        } else {
            omp_set_num_threads(max_threads / 2);
            printf("call update 1\n");
            _circuit_vector[1]->update_quantum_state(_state_vector[1]);
            omp_set_num_threads(1);
        }
    }
    omp_set_num_threads(max_threads);
    omp_set_nested(0);
}
