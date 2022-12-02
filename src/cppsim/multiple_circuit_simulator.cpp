#include "multiple_circuit_simulator.hpp"

#include <omp.h>

#include <chrono>
#include <cstdio>

#include "state.hpp"
#include "state_gpu.hpp"

MultipleQuantumCircuitSimulator::MultipleQuantumCircuitSimulator() {}

MultipleQuantumCircuitSimulator::~MultipleQuantumCircuitSimulator() {
    // for (QuantumStateBase* state : _state_list) {
    //     delete state;
    // }
}

void MultipleQuantumCircuitSimulator::addQuantumCircuit(
    QuantumCircuit* circuit) {
    this->addQuantumCircuitState(circuit, 0);
}

void MultipleQuantumCircuitSimulator::addQuantumCircuitState(
    QuantumCircuit* circuit, ITYPE basis) {
    _circuit_list.push_back(circuit);
    _state_list.push_back(nullptr);
    _basis_list.push_back(basis);
}

void MultipleQuantumCircuitSimulator::simulate() {
    int max_threads = omp_get_max_threads();

    // omp_set_nested(1);
    // omp_set_num_threads(2);
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < _circuit_list.size(); ++i) {
        int tid = omp_get_thread_num();
        if (tid == 0) {  // 2 thread for gpu
            std::chrono::system_clock::time_point start, end;
            start = std::chrono::system_clock::now();

            if (_state_list[i] != nullptr) {
                delete _state_list[i];
            }
            QuantumStateGpu* state_gpu =
                new QuantumStateGpu(_circuit_list[i]->qubit_count);
            if (_basis_list[i] > 0) {
                state_gpu->set_computational_basis(_basis_list[i]);
            }
            _circuit_list[i]->update_quantum_state(state_gpu);
            _state_list[i] = state_gpu;

            end = std::chrono::system_clock::now();
            double t_update =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    end - start)
                    .count() /
                1e3;
            // printf("gpu: i=%d, t_update=%lf[s]\n", i, t_update);
        } else {  // other thread for cpu
            std::chrono::system_clock::time_point start, end;
            start = std::chrono::system_clock::now();
            // omp_set_num_threads(max_threads - 2);

            // #pragma omp parallel
            //             {
            // #pragma omp single
            //                 printf("cpu threads: %d\n",
            //                 omp_get_num_threads());
            //             }

            if (_state_list[i] != nullptr) {
                delete _state_list[i];
            }
            QuantumStateCpu* state_cpu =
                new QuantumStateCpu(_circuit_list[i]->qubit_count);
            if (_basis_list[i] > 0) {
                state_cpu->set_computational_basis(_basis_list[i]);
            }
            _circuit_list[i]->update_quantum_state(state_cpu);
            _state_list[i] = state_cpu;

            end = std::chrono::system_clock::now();
            double t_update =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    end - start)
                    .count() /
                1e3;
            // printf("cpu: i=%d, t_update=%lf[s]\n", i, t_update);
        }
    }
    omp_set_num_threads(max_threads);
    // omp_set_nested(0);
}

std::vector<QuantumStateBase*>
MultipleQuantumCircuitSimulator::get_state_list() {
    return _state_list;
}