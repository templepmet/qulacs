#pragma once

#include <vector>

#include "circuit.hpp"

class DllExport QuantumCircuitState {
public:
    QuantumCircuit* _circuit;
    QuantumStateBase* _state;

    QuantumCircuitState(QuantumCircuit* circuit, QuantumStateBase* state);
    ~QuantumCircuitState();
};

class DllExport MultipleQuantumCircuitSimulator {
private:
    std::vector<QuantumCircuitState*> _circuit_state_vector;

public:
    MultipleQuantumCircuitSimulator();
    MultipleQuantumCircuitSimulator(
        std::vector<QuantumCircuitState*>& circuit_state_vector);

    ~MultipleQuantumCircuitSimulator();

    void addQuantumCircuitState(QuantumCircuitState* circuit_state);
    void addQuantumCircuitState(
        QuantumCircuit* circuit, QuantumStateBase* state);

    void simulate();

    std::vector<QuantumStateBase*> get_all_vector();
};
