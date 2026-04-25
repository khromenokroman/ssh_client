#pragma once

#include <libssh2.h>

#include <memory>
#include <string_view>

class Libssh2Guard {
   public:
    Libssh2Guard();

    ~Libssh2Guard();

    Libssh2Guard(const Libssh2Guard &) = delete;
    Libssh2Guard &operator=(const Libssh2Guard &) = delete;
};

struct SocketDeleter {
    void operator()(int *socketFd) const noexcept {
        if (socketFd) {
            if (*socketFd >= 0) {
                ::close(*socketFd);
            }
            delete socketFd;
        }
    }
};

struct SessionDeleter {
    void operator()(LIBSSH2_SESSION *session) const noexcept {
        if (session) {
            libssh2_session_disconnect(session, "Normal shutdown");
            libssh2_session_free(session);
        }
    }
};

struct ChannelDeleter {
    void operator()(LIBSSH2_CHANNEL *channel) const noexcept {
        if (channel) {
            libssh2_channel_close(channel);
            libssh2_channel_free(channel);
        }
    }
};

class SshClient {
   public:
    explicit SshClient(std::string_view host, int port, std::string_view username, std::string_view password);
    std::string execute(std::string_view command);

   private:
    static std::unique_ptr<int, SocketDeleter> connectTcp(std::string_view host, int port);

    std::string m_host;                                         // 32
    std::string m_username;                                     // 32
    std::string m_password;                                     // 32
    std::unique_ptr<Libssh2Guard> m_libssh2Guard;               // 8
    std::unique_ptr<int, SocketDeleter> m_socket;               // 8
    std::unique_ptr<LIBSSH2_SESSION, SessionDeleter> m_session; // 8
    int m_port;                                                 // 4
};