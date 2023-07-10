#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "CQCSPModulator/CCompleteModulator/CCompleteModulator.hpp"

namespace py = pybind11;

class PythonCompleteModulator : private QCSP::CCompleteModulator {
public:
    std::vector<int> * process(const std::vector<int> & input) {
        std::vector<int> * output = new std::vector<int>(CQCSPModulator::frame_size(), 0);
        this->QCSP::CCompleteModulator::process(input, *output);
        return output;
    }

    PythonCompleteModulator(const std::vector<int> & _pn, const std::vector<int> & _om)
        : QCSP::CCompleteModulator(_pn, _om) {}

    ~PythonCompleteModulator() = default;
};

PYBIND11_MODULE(qcsp_stem, m) {
    py::class_<PythonCompleteModulator>(m, "CompleteModulator")
        .def(py::init<const std::vector<int> &, const std::vector<int> &>())
        .def("process", py::overload_cast<const std::vector<int> &>(&PythonCompleteModulator::process), py::return_value_policy::take_ownership);
}
