#pragma once
#include <string>
#include <cstdint>
#include <poll.h>
#include <termios.h>

namespace dmbot_serial {

class UARTPort {
public:
    UARTPort() = default;
    void configure(const std::string &device, uint32_t baudrate, uint32_t read_poll_ms);
    bool open();
    void close();
    ssize_t read(uint8_t *buf, size_t len);
    ssize_t write(const uint8_t *buf, size_t len);
    bool is_open() const;

private:
    static speed_t baud_to_speed(uint32_t baud);
    std::string device_{};
    uint32_t baudrate_{0};
    uint32_t read_poll_ms_{100};
    int fd_{-1};
    struct pollfd poll_fd_[1] = {};
    uint32_t write_timeout_us_{20};
};

} // namespace dmbot_serial
