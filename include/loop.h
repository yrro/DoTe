
#pragma once

#include "i_loop.h"

#include <poll.h>

#include <vector>
#include <map>

namespace dote {

/// \brief  An event loop that does polling
class Loop : public ILoop
{
  public:
    /// \brief  Create the looper
    Loop() = default;

    Loop(const Loop&) = delete;
    Loop& operator=(const Loop&) = delete;

    /// \brief  Has to be noexcept as override
    ~Loop() noexcept = default;

    /// \brief  Run the loop
    void run();

    /// \brief   Register for read availability on a given handle
    ///
    /// \param handle    The handle to register for reading
    /// \param callback  The callback to call if it triggers
    /// \param timeout  The time at which to call exception on the handle
    ///
    /// \return  True if the handle is not already registered and now is
    bool registerRead(int handle, Callback callback, time_t timeout) override;

    /// \brief   Register for write availability on a given handle
    ///
    /// \param handle    The handle to register for writing
    /// \param callback  The callback to call if it triggers
    /// \param timeout  The time at which to call exception on the handle
    ///
    /// \return  True if the handle is not already registered and now is
    bool registerWrite(int handle, Callback callback, time_t timeout) override;

    /// \brief  Register for exceptions on a given handle
    ///
    /// \param handle    The handle to register for exceptions
    /// \param callback  The callback to call if it triggers
    ///
    /// \return  True if the handle is not already registered and now is
    bool registerException(int handle, Callback callback) override;

    /// \brief  Remove a read handle from the loop
    ///
    /// \param handle  The handle to remove read handles for
    void removeRead(int handle) override;

    /// \brief  Remove a write handle from the loop
    ///
    /// \param handle  The handle to remove write handles for
    void removeWrite(int handle) override;

    /// \brief  Remove a exception handle from the loop
    ///
    /// \param handle  The handle to remove exception handles for
    void removeException(int handle) override;

  private:
    /// \brief  Register a given handle with m_fds for the poll
    ///
    /// \param handle  The handle to register for events for
    /// \param event   The event type to register for
    void registerFd(int handle, short event);

    /// \brief  Deregister a given handle with m_fds for the poll
    ///
    /// \param handle  The handle to deregister for events for
    /// \param event   The event type to deregister for
    void deregisterFd(int handle, short event);

    /// \brief  If the handle doesn't exist in m_readFunctions,
    ///         m_writeFunctions or m_exceptFunctions then remove
    ///         it from m_fds.
    ///
    /// \param handle  The handle to check and remove
    void cleanFd(int handle);

    /// \brief  Call a callback in a set of functions
    ///
    /// \param functions  The functions to lookup the callback in
    /// \param handle  The handle to lookup in the functions
    static void callCallback(
        const std::map<int, std::pair<Callback, time_t>>& functions,
        int handle);

    /// \brief  Find the earliest timeout in the list of functions, calling exception if it has occurred
    ///
    /// \param now  The current time (to see if it has expired)
    /// \param functions  The functions to find the earliest timeout in
    ///
    /// \return  The earliest timeout in the functions or 0 if there aren't any that haven't expired
    time_t timeout(time_t now, std::map<int, std::pair<Callback, time_t>>& functions);

    /// \brief  Work out how long until a timeout
    ///
    /// \return  The number of milliseconds until the next timeout
    int timeout();
    
    /// \brief  Raise an exception for a given file descriptor
    ///
    /// \param handle  The handle to raise the exception for
    ///
    /// \return  True if an exception handler existed for the handle and was called
    bool raiseException(int handle);

    /// The read handles
    std::map<int, std::pair<Callback, time_t>> m_readFunctions;
    /// The write handles
    std::map<int, std::pair<Callback, time_t>> m_writeFunctions;
    /// The exception handles
    std::map<int, Callback> m_exceptFunctions;
    /// The contraction of all the callback handles for use with poll
    std::vector<pollfd> m_fds;
};

}  // namespace dote
