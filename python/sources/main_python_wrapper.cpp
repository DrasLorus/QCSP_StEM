#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "CQCSPModulator/CCompleteModulator/CCompleteModulator.hpp"
#include "utilities/conversions.hpp"
#include "utilities/definitions.hpp"

namespace py = pybind11;

using QCSP::uint_gf_t;

constexpr const size_t msg_size    = QCSP::CCompleteModulator::message_size();
constexpr const size_t string_size = (msg_size * QCSP::_LOG2GF_) / 8;

std::vector<uint_gf_t> * python_bytes_to_int(const std::string & input_char) {
    const std::vector<char>  input(input_char.begin(), input_char.end());
    std::vector<uint_gf_t> * output = new std::vector<uint_gf_t>(msg_size);

    if (input.size() != string_size) {
        throw std::invalid_argument(
            "Valid size is " + std::to_string(string_size)
            + " and input is of size " + std::to_string(input.size()) + ".");
    }

    QCSP::bytes_to_int<QCSP::_LOG2GF_, msg_size>(input, *output);
    return output;
}

class PythonCompleteModulator : private QCSP::CCompleteModulator {
public:
    std::vector<int> * process(const std::vector<uint_gf_t> & input) {
        std::vector<int> * output = new std::vector<int>(CQCSPModulator::frame_size(), 0);

        if (input.size() != msg_size) {
            throw std::invalid_argument(
                "Valid size is " + std::to_string(msg_size)
                + " and input is of size " + std::to_string(input.size()) + ".");
        }

        this->QCSP::CCompleteModulator::process(input, *output);
        return output;
    }

    PythonCompleteModulator(const std::vector<int> & _pn, const std::vector<int> & _om)
        : QCSP::CCompleteModulator(_pn, _om) {}

    ~PythonCompleteModulator() = default;
};

PYBIND11_MODULE(qcsp, m_qcsp) {
    m_qcsp.doc() = "This is the top module for QCSP Python bindings.";

    pybind11::module_ m_stem = m_qcsp.def_submodule("stem");
    m_stem.doc()             = "The QCSP Standalone Emitter (StEm)\n\nThis module contains functions to handle the transmitter.";

    py::class_<PythonCompleteModulator>(m_stem, "CompleteModulator")
        .def(py::init<const std::vector<int> &,
                      const std::vector<int> &>())
        .def("process",
             py::overload_cast<const std::vector<uint_gf_t> &>(&PythonCompleteModulator::process),
             py::return_value_policy::take_ownership);
    m_stem.def("string_to_message",
               &python_bytes_to_int,
               py::return_value_policy::take_ownership);
}
