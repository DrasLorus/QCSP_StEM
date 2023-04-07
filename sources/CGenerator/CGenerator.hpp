#ifndef _GENERATOR_HPP_
#define _GENERATOR_HPP_

#include <cstdint>
#include <vector>

namespace QCSP {

class CGenerator {
    public:
        virtual void process(std::vector<float> & output);
};

}

#endif /* _GENERATOR_HPP_ */
