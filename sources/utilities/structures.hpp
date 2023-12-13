#ifndef _QCSP_STRUCTURES_HPP_
#define _QCSP_STRUCTURES_HPP_

#include <string>

namespace QCSP {

/**
 * @brief Define the different possible generators
 * 
 */
typedef enum {
    GEN_UNKNOW,
    // GEN_RANDOM,
    GEN_TIMER,
    GEN_ZERO,
    GEN_GPS,
    GEN_FILE,
    GEN_CIN
} generator_t;

/**
 * @brief Define the different possible modulators
 * 
 */
typedef enum {
    MOD_UNKNOW,
    MOD_NOPC,
    // MOD_ZERO,
    MOD_REAL
} modulator_t;

/**
 * @brief Helper structure containing emitter settings  
 * 
 */
struct emitter_parameters {
    std::string device_args;
    std::string ant;
    double      rate;
    double      freq;
    double      gain;
    size_t      inter_delay;
    size_t      max_count;
    bool        count_limited;
    double      ttl_us;
    bool        time_limited;
    bool        to_file;
    std::string output_file;
    bool        save_frames;
    bool        no_ui;
    generator_t gen_type;
    modulator_t mod_type;
    std::string gps_tty;
    std::string input_file;
    bool        has_clock_source;
    std::string clock_source;
    bool        has_subdev;
    std::string subdev;
    bool        has_bandwidth;
    double      bandwidth;
    std::string cpu_format;
    std::string otw_format;
};

} // namespace QCSP

#endif // _QCSP_STRUCTURES_HPP_
