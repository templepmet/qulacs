#include "multiple_circuit_simulator.hpp"

MultipleQuantumCircuitSimulator::MultipleQuantumCircuitSimulator() {}

MultipleQuantumCircuitSimulator::~MultipleQuantumCircuitSimulator() {}

void MultipleQuantumCircuitSimulator::addQuantumCircuitState(
    QuantumCircuit* circuit, QuantumStateBase* state) {
    _circuit_vector.push_back(circuit);
    _state_vector.push_back(state);
}

void MultipleQuantumCircuitSimulator::simulate() {
    for (int i = 0; i < _circuit_vector.size(); ++i) {
        _circuit_vector[i]->update_quantum_state(_state_vector[i]);
    }
}
