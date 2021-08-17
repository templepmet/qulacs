#include "fermion_operator.hpp"

#include <boost/range/combine.hpp>

#include "observable.hpp"
#include "pauli_operator.hpp"
#include "state.hpp"
#include "type.hpp"

FermionOperator::FermionOperator(){};

UINT FermionOperator::get_term_count() const {
    return (UINT)_fermion_terms.size();
}

std::pair<CPPCTYPE, SingleFermionOperator> FermionOperator::get_term(
    const UINT index) const {
    return std::make_pair(
        this->_coef_list.at(index), this->_fermion_terms.at(index));
}

void FermionOperator::add_term(
    const CPPCTYPE coef, SingleFermionOperator fermion_operator) {
    this->_coef_list.push_back(coef);
    this->_fermion_terms.push_back(fermion_operator);
}

void FermionOperator::add_term(const CPPCTYPE coef, std::string action_string) {
    this->_coef_list.push_back(coef);
    this->_fermion_terms.push_back(SingleFermionOperator(action_string));
}

void FermionOperator::remove_term(UINT index) {
    this->_coef_list.erase(this->_coef_list.begin() + index);
    this->_fermion_terms.erase(this->_fermion_terms.begin() + index);
}

const std::vector<SingleFermionOperator>& FermionOperator::get_fermion_list()
    const {
    return _fermion_terms;
}

const std::vector<CPPCTYPE>& FermionOperator::get_coef_list() const {
    return _coef_list;
}

Observable FermionOperator::jordan_wigner() {
    /**
     * Qubitとの対応をXとYで展開して計算する
     * a_p^\dagger \mapsto 1/2 (X_p - iY_p)Z_{p-1} ... Z_0
     *  = 1/2 (X_p Z_{p-1} ... Z_0) - i/2 (Y_p Z_{p-1} ... Z_0)
     */

    Observable transformed;
    for (auto fop_tuple : boost::combine(_fermion_terms, _coef_list)) {
        SingleFermionOperator sfop;
        CPPCTYPE coef;
        boost::tie(sfop, coef) = fop_tuple;

        Observable transformed_term;
        // 0に掛け算しても0になってしまうため
        transformed_term.add_term(1.0, "I 0");

        auto target_index_list = sfop.get_target_index_list();
        auto action_id_list = sfop.get_action_id_list();
        for (auto ladder_operator :
            boost::combine(target_index_list, action_id_list)) {
            UINT target_index;
            UINT action_id;
            boost::tie(target_index, action_id) = ladder_operator;

            Observable x_term, y_term;

            // Z factors
            std::vector<UINT> target_qubit_index_list(target_index + 1);
            std::vector<UINT> pauli_id_list(target_index + 1, PAULI_ID_Z);
            for (UINT i = 0; i < target_index + 1; i++) {
                target_qubit_index_list[i] = i;
            }

            // X factors
            pauli_id_list.at(target_index) = PAULI_ID_X;
            MultiQubitPauliOperator X_factors(
                target_qubit_index_list, pauli_id_list);
            x_term.add_term(coef * 0.5, X_factors);

            // Y factors
            pauli_id_list.at(target_index) = PAULI_ID_Y;
            MultiQubitPauliOperator Y_factors(
                target_qubit_index_list, pauli_id_list);
            CPPCTYPE coef_Y = coef * 0.5i;
            if (action_id) coef_Y *= -1;
            y_term.add_term(coef_Y, Y_factors);

            transformed_term *= x_term + y_term;
        }
        transformed += transformed_term;
    }

    return transformed;
}

Observable FermionOperator::bravyi_kitaev(UINT n_qubits) {
    UINT N = count_qubits(*this);
    if(n_qubits < N) assert(n_qubits < N && "Invalid number of qubits specified.");

    std::vector<Observable> transformed_terms;
    for (auto fop_tuple : boost::combine(_fermion_terms, _coef_list)) {
        SingleFermionOperator sfop;
        CPPCTYPE coef;
        boost::tie(sfop, coef) = fop_tuple;
        transformed_terms.push_back(_transform_operator_term(sfop, coef, n_qubits));
    }
    return inline_sum(transformed_terms, Observable());
}


UINT count_qubits(FermionOperator& fop) {
    UINT qubits = 0;
    std::vector<SingleFermionOperator> fermion_terms = fop.get_fermion_list();
    for (auto sfop : fermion_terms) {
        auto target_index_list = sfop.get_target_index_list();
        for (auto target_index : target_index_list) {
            qubits = std::max(qubits, target_index + 1);
        }
    }
    return qubits;
}

Observable inline_sum(std::vector<Observable> summands, Observable seed) {
    Observable result = seed;
    for (auto summand : summands) {
        result += summand;
    }
    return result;
}

Observable inline_product(std::vector<Observable> factors, Observable seed) {
    Observable result = seed;
    for (auto factor : factors) {
        result *= factor;
    }
    return result;
}

std::set<UINT> _update_set(UINT index, UINT n_qubits) {
    std::set<UINT> indices;

    // For bit manipulation we need to count from 1 rather than 0
    index += 1;

    while (index <= n_qubits) {
        indices.insert(index - 1);
        // Add least significant one to index
        // E.g. 00010100 -> 00011000
        index += index & -index;
    }
    return indices;
}

std::set<UINT> _occupation_set(UINT index) {
    std::set<UINT> indices;
    // For bit manipulation we need to count from 1 rather than 0
    index += 1;

    indices.insert(index - 1);
    UINT parent = index & (index - 1);
    index -= 1;

    while (index != parent) {
        indices.insert(index - 1);
        // Remove least significant one from index
        // E.g. 00010100 -> 00010000
        index &= index - 1;
    }
    return indices;
}

std::set<UINT> _parity_set(UINT index) {
    std::set<UINT> indices;
    // For bit manipulation we need to count from 1 rather than 0
    index += 1;

    while (index > 0) {
        indices.insert(index - 1);
        // Remove least significant one from index
        // E.g. 00010100 -> 00010000
        index &= index - 1;
    }
    return indices;
}

Observable _transform_ladder_operator(
    UINT target_index, UINT action_id, UINT n_qubits) {
    std::set<UINT> update_set = _update_set(target_index, n_qubits);
    std::set<UINT> occupation_set = _occupation_set(target_index);
    std::set<UINT> parity_set = _parity_set(target_index - 1);

    // Initialize the transformed majorana operator (a_p^\dagger + a_p) / 2
    Observable transformed_operator;

    std::vector<UINT> target_index_list;
    target_index_list.insert(
        target_index_list.end(), update_set.begin(), update_set.end());
    target_index_list.insert(
        target_index_list.end(), parity_set.begin(), parity_set.end());

    std::vector<UINT> pauli_x_list(update_set.size(), PAULI_ID_X);
    std::vector<UINT> pauli_z_list(parity_set.size(), PAULI_ID_Z);
    std::vector<UINT> pauli_list;
    pauli_list.insert(pauli_list.end(), pauli_x_list.begin(), pauli_x_list.end());
    pauli_list.insert(pauli_list.end(), pauli_z_list.begin(), pauli_z_list.end());

    transformed_operator.add_term(
        0.5, MultiQubitPauliOperator(target_index_list, pauli_list));

    // Get the transformed (a_p^\dagger - a_p) / 2
    // Below is equivalent to X(update_set) * Z(parity_set ^ occupation_set)
    Observable transformed_majorana_difference;

    std::set<UINT> index_only = {target_index};
    std::set<UINT> update_minus_index_set;
    std::set_difference(update_set.begin(), update_set.end(),
        index_only.begin(), index_only.end(),
        std::inserter(update_minus_index_set, update_minus_index_set.end()));
    std::set<UINT> parity_xor_occupation_set;
    std::set_symmetric_difference(parity_set.begin(), parity_set.end(),
        occupation_set.begin(), occupation_set.end(),
        std::inserter(
            parity_xor_occupation_set, parity_xor_occupation_set.end()));
    std::set<UINT> p_xor_o_minus_index_set;
    std::set_difference(parity_xor_occupation_set.begin(),
        parity_xor_occupation_set.end(), index_only.begin(), index_only.end(),
        std::inserter(p_xor_o_minus_index_set, p_xor_o_minus_index_set.end()));

    target_index_list.clear();
    pauli_list.clear();
    pauli_x_list.clear();
    pauli_z_list.clear();

    pauli_x_list = std::vector<UINT>(update_minus_index_set.size(), PAULI_ID_X);
    pauli_z_list = std::vector<UINT>(p_xor_o_minus_index_set.size(), PAULI_ID_Z);

    target_index_list.push_back(target_index);
    target_index_list.insert(target_index_list.end(),
        update_minus_index_set.begin(), update_minus_index_set.end());
    target_index_list.insert(target_index_list.end(),
        p_xor_o_minus_index_set.begin(), p_xor_o_minus_index_set.end());

    pauli_list.push_back(PAULI_ID_Y);
    pauli_list.insert(pauli_list.end(), pauli_x_list.begin(), pauli_x_list.end());
    pauli_list.insert(pauli_list.end(), pauli_z_list.begin(), pauli_z_list.end());

    transformed_majorana_difference.add_term(
        -0.5i, MultiQubitPauliOperator(target_index_list, pauli_list));

    // Raising
    if (action_id == 1)
        transformed_operator += transformed_majorana_difference;
    else
        transformed_operator -= transformed_majorana_difference;
    return transformed_operator;
}

Observable _transform_operator_term(
    SingleFermionOperator& sfop, CPPCTYPE coef, UINT n_qubits) {
    auto target_index_list = sfop.get_target_index_list();
    auto action_id_list = sfop.get_action_id_list();
    std::vector<Observable> transformed_ladder_ops;
    for (auto ladder_operator :
        boost::combine(target_index_list, action_id_list)) {
        UINT target_index;
        UINT action_id;
        boost::tie(target_index, action_id) = ladder_operator;
        transformed_ladder_ops.push_back(
            _transform_ladder_operator(target_index, action_id, n_qubits));
    }
    Observable seed;
    seed.add_term(coef, "I 0");
    return inline_product(transformed_ladder_ops, seed);
}