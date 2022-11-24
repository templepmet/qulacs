#pragma once

#include "multiple_circuit_simulator.hpp"

QuantumCircuitState::QuantumCircuitState(
    QuantumCircuit* circuit, QuantumStateBase* state) {
    _circuit = circuit;
    _state = state;
}

QuantumCircuitState::~QuantumCircuitState() {}

MultipleQuantumCircuitSimulator::MultipleQuantumCircuitSimulator() {}

MultipleQuantumCircuitSimulator::MultipleQuantumCircuitSimulator(
    std::vector<QuantumCircuitState*>& circuit_state_vector) {
    _circuit_state_vector = circuit_state_vector;
}

MultipleQuantumCircuitSimulator::~MultipleQuantumCircuitSimulator() {}

void MultipleQuantumCircuitSimulator::addQuantumCircuitState(
    QuantumCircuitState* circuit_state) {
    _circuit_state_vector.push_back(circuit_state);
}

void MultipleQuantumCircuitSimulator::addQuantumCircuitState(
    QuantumCircuit* circuit, QuantumStateBase* state) {
    addQuantumCircuitState(new QuantumCircuitState(circuit, state));
}

void MultipleQuantumCircuitSimulator::simulate() {}

std::vector<QuantumStateBase*>
MultipleQuantumCircuitSimulator::get_all_vector() {}