#ifndef _USRP_FUNCTIONS_HPP_
#define _USRP_FUNCTIONS_HPP_

#include "utilities/structures.hpp"
#include <fstream>
#include <string>

#include <boost/program_options.hpp>

#include <uhd/usrp/multi_usrp.hpp>

namespace QCSP {

/**
 * @brief check for valid USRP device, like the UHD-provided program uhd_find_devices 
 * 
 * @details see https://files.ettus.com/manual/page_identification.html
 *
 * @param device_args a device ID string
 */
void check_usrp(uhd::device_addr_t device_args = std::string(""));

/**
 * @brief safely initialize a USRP
 * 
 * @details see https://files.ettus.com/manual/classuhd_1_1usrp_1_1multi__usrp.html
 * and https://files.ettus.com/manual/classuhd_1_1tx__streamer.html
 *
 * @param prm transmitter settings
 * @param usrp_transmitter an initialized uhd::usrp::multi_usrp::sptr
 * @param send_stream an ready-to-use uhd::tx_streamer::sptr
 */
void init_usrp(
    const emitter_parameters &    prm,
    uhd::usrp::multi_usrp::sptr & usrp_transmitter,
    uhd::tx_streamer::sptr &      send_stream);

/**
 * @brief Transmit buffer to the USRP using tx_streamer, using max_samps_per_buff sized OTW buffers
 * 
 * @details see https://files.ettus.com/manual/classuhd_1_1tx__streamer.html#aeb2e0f44810693d9da99ea1e04fad21f
 * and https://files.ettus.com/manual/structuhd_1_1tx__metadata__t.html
 *
 * @tparam samp_type sample type ; must match prm.otw_format used in init_usrp()
 * @param tx_stream a well-formed uhd::tx_streamer::sptr
 * @param buffer an arbitrary sized vector of samp_type
 * @param max_samps_per_buff number of samples in over-the-wire buffers
 */
template <class samp_type>
void send_from_memory(uhd::tx_streamer::sptr tx_stream, const std::vector<samp_type> & buffer, size_t max_samps_per_buff) {
    uhd::tx_metadata_t md;
    md.start_of_burst = false;
    md.end_of_burst   = false;
    md.has_time_spec  = false;

    const size_t buffer_size = buffer.size();

    // loop until the entire buffer has been read
    const size_t samps_per_buff = ((max_samps_per_buff > 0) && (max_samps_per_buff < buffer_size)
                                       ? max_samps_per_buff
                                       : buffer_size);

    size_t cnt = 0;
    while (!md.end_of_burst) {
        md.end_of_burst = cnt + samps_per_buff >= buffer_size;

        const size_t num_tx_samps = std::min(samps_per_buff, buffer_size - cnt);
        const size_t samples_sent = tx_stream->send(&buffer.front(), samps_per_buff, md, 2.0);
        if (samples_sent != num_tx_samps) {
            UHD_LOG_ERROR("TX-STREAM",
                          "The tx_stream timed out sending " << num_tx_samps << " samples ("
                                                             << samples_sent << " sent).");
            return;
        }

        cnt += samples_sent;
    }
}

/**
 * @brief Transmit buffer to the USRP using tx_streamer in one go
 * 
 * @details see https://files.ettus.com/manual/classuhd_1_1tx__streamer.html#aeb2e0f44810693d9da99ea1e04fad21f
 * and https://files.ettus.com/manual/structuhd_1_1tx__metadata__t.html
 *
 * @tparam samp_type sample type ; must match prm.otw_format used in init_usrp()
 * @param tx_stream a well-formed uhd::tx_streamer::sptr
 * @param buffer an arbitrary sized vector of samp_type
 */
template <class samp_type>
void send_from_memory(uhd::tx_streamer::sptr tx_stream, const std::vector<samp_type> & buffer) {
    uhd::tx_metadata_t md;
    md.start_of_burst = false;
    md.end_of_burst   = true;
    md.has_time_spec  = false;

    const size_t buffer_size = buffer.size();

    const size_t samples_sent = tx_stream->send(&buffer.front(), buffer_size, md, 3.0);
    if (samples_sent != buffer_size) {
        UHD_LOG_ERROR("TX-STREAM",
                      "The tx_stream timed out sending " << buffer_size << " samples ("
                                                         << samples_sent << " sent).");
        return;
    }
}

/**
 * @brief write buffer to the file filename
 * 
 * @tparam samp_type sample type
 * @param filename file to write to
 * @param buffer samples to write in filename
 */
template <class samp_type>
void write_to_file(std::string filename, const std::vector<samp_type> & buffer) {
    const size_t buffer_size = buffer.size();

    std::ofstream toFile(filename, std::ios::binary | std::ios::app);

    toFile.write(reinterpret_cast<const char *>(buffer.data()), sizeof(samp_type) * buffer_size);
}

} // namespace QCSP

#endif
