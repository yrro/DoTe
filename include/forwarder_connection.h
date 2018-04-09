
#pragma once

#include "openssl/ssl_connection.h"

#include <memory>
#include <vector>
#include <deque>
#include <string>
#include <functional>

namespace dote {

class Loop;
class ForwarderConfig;
class Socket;

namespace openssl {
class Context;
}  // namespace openssl

/// \brief  A class representing a connection to a forwarder.
///
/// There is an instance of this class for each connection to
/// a forwarder for each request address.
class ForwarderConnection
{
  public:
    /// The type of the callback to call when data is read on the socket
    using IncomingCallback =
        std::function<void(ForwarderConnection&, std::vector<char>)>;
    
    /// The type of callback to call when the socket is closed
    using ShutdownCallback = std::function<void(ForwarderConnection&)>;

    /// \brief  Create a connection to a forwarder
    ///
    /// \param loop      The looper to manage the connection
    /// \param config    The configuration for the possible forwarders
    /// \param incoming  The function to call when incoming data arrives
    /// \param shutdown  The callback to call on socket shutdown
    /// \param context   The OpenSSL context to communicate with
    ForwarderConnection(std::shared_ptr<Loop> loop,
                        std::shared_ptr<ForwarderConfig> config,
                        IncomingCallback incoming,
                        ShutdownCallback shutdown,
                        std::shared_ptr<openssl::Context> context);

    ForwarderConnection(const ForwarderConnection&) = delete;
    ForwarderConnection& operator=(const ForwarderConnection&) = delete;

    /// \brief  Clean up the connection
    ~ForwarderConnection();

    /// \brief  Check if the socket is closed
    ///
    /// \return  True if the socket is closed (or closing)
    bool closed();

    /// \brief  Start the shutdown of the underlying socket
    void shutdown();
    
    /// \brief  Send some data
    ///
    /// \param buffer  The buffer to send
    ///
    /// \return  True if queued to send, false if socket not open or closed
    bool send(std::vector<char> buffer);

  private:
    /// \brief  The current state of the connection to the server
    enum State
    {
        /// The socket is connecting
        CONNECTING,
        /// The socket is open and ready to communicate
        OPEN,
        /// The socket is in the process of closing down
        SHUTTING_DOWN,
        /// The socket is closed and cannot be re-opened
        CLOSED
    };

    /// \brief  Verify the certificate of the connection matches
    ///         the configuration pinning
    ///
    /// \return  True if the hostname and certificate are valid
    bool verifyConnection();

    /// \brief  Perform the initial connection
    ///
    /// \param handle  The socket that is available to connect on
    void connect(int handle);

    /// \brief  Nicely shutdown the connection
    ///
    /// \param handle  The socket that is available to shutdown on
    void _shutdown(int handle);

    /// \brief  An exception occurred on the socket
    ///
    /// \param handle  The socket the exception occurred on
    void exception(int handle);
    
    /// \brief  Handle incoming data
    ///
    /// \param handle  The socket that is available to read on
    void incoming(int handle);
    
    /// \brief  Handle outgoing data
    ///
    /// \param handle  The socket that is available to write on
    void outgoing(int handle);

    /// \brief  Remove from the looper and close
    void close();

    /// The looper used to manage the connection
    std::shared_ptr<Loop> m_loop;
    /// The configuration for the available forwarders
    std::shared_ptr<ForwarderConfig> m_config;
    /// A function to handle incoming data on the socket
    IncomingCallback m_incoming;
    /// A function to call when the socket is closed
    ShutdownCallback m_shutdown;
    /// The underlying OpenSSL connection
    openssl::SslConnection m_connection;
    /// The state of the connection
    State m_state;
    /// The established connection to the forwarder
    std::shared_ptr<Socket> m_socket;
    /// A queue of pending write buffers
    std::deque<std::vector<char>> m_buffers;
    /// The expected hostname for the remote client
    std::string m_hostname;
    /// The expected pin for the remote client
    std::vector<unsigned char> m_pin;
};

}  // namespace dote
