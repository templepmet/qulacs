#include "multiple_circuit_simulator.hpp"

#include <omp.h>

#include <cstdio>

#include "state.hpp"
#include "state_gpu.hpp"

MultipleQuantumCircuitSimulator::MultipleQuantumCircuitSimulator() {}

MultipleQuantumCircuitSimulator::~MultipleQuantumCircuitSimulator() {
    // for (QuantumStateBase* state : _state_list) {
    //     delete state;
    // }
}

void MultipleQuantumCircuitSimulator::addQuantumCircuitState(
    QuantumCircuit* circuit, UINT qubits) {
    this->addQuantumCircuitState(circuit, qubits, 0);
}

void MultipleQuantumCircuitSimulator::addQuantumCircuitState(
    QuantumCircuit* circuit, UINT qubits, ITYPE basis) {
    _circuit_list.push_back(circuit);
    _state_list.push_back(nullptr);
    _qubits_list.push_back(qubits);
    _basis_list.push_back(basis);
}

void MultipleQuantumCircuitSimulator::simulate() {
    int max_threads = omp_get_max_threads();

    omp_set_nested(1);
    omp_set_num_threads(2);
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < _circuit_list.size(); ++i) {
        int tid = omp_get_thread_num();
        if (tid == 0) {  // 2 thread for gpu
            omp_set_num_threads(2);
            if (_state_list[i] != nullptr) {
                delete _state_list[i];
            }
            QuantumStateGpu* state_gpu = new QuantumStateGpu(_qubits_list[i]);
            if (_basis_list[i] > 0) {
                state_gpu->set_computational_basis(_basis_list[i]);
            }
            _circuit_list[i]->update_quantum_state(state_gpu);
            _state_list[i] = state_gpu;
        } else {  // other thread for cpu
            omp_set_num_threads(max_threads - 2);
            if (_state_list[i] != nullptr) {
                delete _state_list[i];
            }
            QuantumStateCpu* state_cpu = new QuantumStateCpu(_qubits_list[i]);
            if (_basis_list[i] > 0) {
                state_cpu->set_computational_basis(_basis_list[i]);
            }
            _circuit_list[i]->update_quantum_state(state_cpu);
            _state_list[i] = state_cpu;
        }
    }
    omp_set_num_threads(max_threads);
    omp_set_nested(0);
}

std::vector<QuantumStateBase*>
MultipleQuantumCircuitSimulator::get_state_list() {
    return _state_list;
}