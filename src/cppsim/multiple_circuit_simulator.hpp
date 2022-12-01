#pragma once

#include <vector>

#include "circuit.hpp"

class DllExport MultipleQuantumCircuitSimulator {
private:
    std::vector<QuantumCircuit*> _circuit_list;
    std::vector<QuantumStateBase*> _state_list;
    std::vector<UINT> _qubits_list;
    std::vector<ITYPE> _basis_list;

public:
    MultipleQuantumCircuitSimulator();

    ~MultipleQuantumCircuitSimulator();

    void addQuantumCircuitState(QuantumCircuit* circuit, UINT qubits);

    void addQuantumCircuitState(
        QuantumCircuit* circuit, UINT qubits, ITYPE comp_basis);

    void simulate();

    std::vector<QuantumStateBase*> get_state_list();
};
