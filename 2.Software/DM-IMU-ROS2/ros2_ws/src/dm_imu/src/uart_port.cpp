#include "dm_imu/uart_port.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <chrono>

namespace dmbot_serial {

void UARTPort::configure(const std::string &device, uint32_t baudrate, uint32_t read_poll_ms) {
    device_ = device;
    baudrate_ = baudrate;
    read_poll_ms_ = read_poll_ms;
}

bool UARTPort::open() {
    if (fd_ != -1) return true;
    fd_ = ::open(device_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd_ < 0) return false;

    struct termios tty{};
    if (tcgetattr(fd_, &tty) != 0) return false;

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | IXON | IXOFF | IXANY);
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_oflag &= ~OPOST;

    speed_t speed = baud_to_speed(baudrate_);
    cfsetispeed(&tty, speed);
    cfsetospeed(&tty, speed);

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd_, TCSANOW, &tty) != 0) return false;
    tcflush(fd_, TCIOFLUSH);

    poll_fd_[0].fd = fd_;
    poll_fd_[0].events = POLLIN;
    return true;
}

void UARTPort::close() {
    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
        poll_fd_[0].fd = -1;
    }
}

bool UARTPort::is_open() const { return fd_ != -1; }

ssize_t UARTPort::read(uint8_t *buf, size_t len) {
    if (fd_ == -1) return -1;
    size_t total = 0;
    while (total < len) {
        int ret = ::poll(poll_fd_, 1, static_cast<int>(read_poll_ms_));
        if (ret == 0) continue;
        if (ret < 0) return -1;
        ssize_t n = ::read(fd_, buf + total, len - total);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
            return -1;
        }
        if (n == 0) continue;
        total += static_cast<size_t>(n);
    }
    return static_cast<ssize_t>(total);
}

ssize_t UARTPort::write(const uint8_t *buf, size_t len) {
    if (fd_ == -1) return -1;
    size_t total = 0;
    while (total < len) {
        ssize_t n = ::write(fd_, buf + total, len - total);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                ::usleep(write_timeout_us_);
                continue;
            }
            return -1;
        }
        total += static_cast<size_t>(n);
    }
    return static_cast<ssize_t>(total);
}

speed_t UARTPort::baud_to_speed(uint32_t baud) {
    switch (baud) {
        case 0: return B0;
        case 50: return B50;
        case 75: return B75;
        case 110: return B110;
        case 134: return B134;
        case 150: return B150;
        case 200: return B200;
        case 300: return B300;
        case 600: return B600;
        case 1200: return B1200;
        case 1800: return B1800;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        case 460800: return B460800;
        case 500000: return B500000;
        case 576000: return B576000;
        case 921600: return B921600;
        default: return B115200;
    }
}

} // namespace dmbot_serial
