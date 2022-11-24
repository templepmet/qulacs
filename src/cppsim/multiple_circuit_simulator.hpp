#pragma once

#include <vector>

#include "circuit.hpp"

class DllExport MultipleQuantumCircuitSimulator {
private:
    std::vector<QuantumCircuit*> _circuit_vector;
    std::vector<QuantumStateBase*> _state_vector;

public:
    MultipleQuantumCircuitSimulator();

    ~MultipleQuantumCircuitSimulator();

    void addQuantumCircuitState(
        QuantumCircuit* circuit, QuantumStateBase* state);

    void simulate();
};
