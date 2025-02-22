#include <gtest/gtest.h>

#include <cppsim/circuit.hpp>
#include <cppsim/gate_factory.hpp>
#include <cppsim/multiple_circuit_simulator.hpp>
#include <cppsim/simulator.hpp>
#include <cppsim/state.hpp>

#include "../util/util.hpp"

TEST(SimulatorTest, basic_test) {
    UINT n = 3;
    Observable observable(n);
    observable.add_operator(1., "X 0");
    QuantumState state(n), test_state(n);
    QuantumCircuit circuit(n);
    for (UINT i = 0; i < n; ++i) {
        circuit.add_H_gate(i);
    }
    QuantumCircuitSimulator sim(&circuit, &state);
    ASSERT_EQ(sim.get_gate_count(), circuit.gate_list.size());
    sim.simulate();

    // Circuitに適用した量子状態の期待値とSimulatorの期待値が同じであること
    circuit.update_quantum_state(&test_state);
    ASSERT_EQ(sim.get_expectation_value(&observable),
        observable.get_expectation_value(&test_state));
}

TEST(SimulatorTest, swap_test) {
    UINT n = 3;
    Observable observable(n);
    observable.add_operator(1., "X 0");
    QuantumState* state = new QuantumState(n);
    QuantumCircuit circuit(n);
    for (UINT i = 0; i < n; ++i) {
        circuit.add_H_gate(i);
    }
    QuantumCircuitSimulator sim(&circuit, state);
    sim.initialize_random_state();
    sim.simulate();
    auto brefore = sim.get_expectation_value(&observable);
    sim.swap_state_and_buffer();
    sim.simulate();
    auto after = sim.get_expectation_value(&observable);
    // swapしたら期待値が別であること
    ASSERT_NE(brefore, after);

    delete state;
}

TEST(SimulatorTest, delete_test) {
    UINT n = 3;
    QuantumCircuit circuit(n);
    QuantumCircuitSimulator* sim = new QuantumCircuitSimulator(&circuit);
    sim->initialize_random_state();
    sim->simulate();
    // エラーにならないこと
    delete sim;

    QuantumCircuitSimulator* sim1 = new QuantumCircuitSimulator(&circuit);
    // swap1回
    sim1->swap_state_and_buffer();
    // エラーにならないこと
    delete sim1;

    QuantumCircuitSimulator* sim2 = new QuantumCircuitSimulator(&circuit);
    // swap2回
    sim2->swap_state_and_buffer();
    sim2->swap_state_and_buffer();
    // エラーにならないこと
    delete sim2;
}

TEST(SimulatorTest, copy_test) {
    UINT n = 3;
    Observable observable(n);
    observable.add_operator(1., "X 0");
    QuantumState* state = new QuantumState(n);
    QuantumCircuit circuit(n);
    QuantumCircuitSimulator sim(&circuit, state);
    sim.initialize_random_state();
    sim.simulate();
    auto brefore = sim.get_expectation_value(&observable);

    // copyしたら期待値が別であること
    sim.copy_state_from_buffer();
    sim.simulate();
    auto after = sim.get_expectation_value(&observable);
    ASSERT_NE(brefore, after);

    // copyして戻したら期待値が同じであること
    sim.copy_state_from_buffer();
    sim.copy_state_to_buffer();
    sim.simulate();
    auto after2 = sim.get_expectation_value(&observable);
    ASSERT_EQ(after, after2);

    delete state;
}

TEST(MultipleSimulatorTest, basic_test) {
    int n_state = 4;
    UINT qubits = 3;
    QuantumCircuit circuit(qubits);
    for (UINT i = 0; i < qubits; ++i) {
        circuit.add_H_gate(i);
    }
    MultipleQuantumCircuitSimulator sim;
    for (int i = 0; i < n_state; ++i) {
        sim.addQuantumCircuit(&circuit);
    }
    sim.simulate();
    std::vector<QuantumStateBase*> state_list = sim.get_state_list();

    std::vector<CPPCTYPE*> vec;
    for (QuantumStateBase* state : state_list) {
        CPPCTYPE* cpp;
        if (state->get_device_name() == "gpu") {
            cpp = state->duplicate_data_cpp();
        } else {
            cpp = state->data_cpp();
        }
        vec.push_back(cpp);
    }

    int dim = state_list[0]->dim;
    for (int i = 1; i < n_state; ++i) {
        int d = state_list[i]->dim;
        ASSERT_EQ(d, dim);
        for (ITYPE j = 0; j < dim; ++j) {
            ASSERT_TRUE(abs(vec[i][j] - vec[0][j]) < eps);
        }
    }
}
