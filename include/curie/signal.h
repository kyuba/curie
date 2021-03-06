/**\file
 * \brief Signal Handling
 *
 * Platform-independent handling of process signals, such as segmentation
 * faults or termination requests.
 *
 * \copyright
 * Copyright (c) 2008-2014, Kyuba Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/curie
 * \see Project Source Code: http://git.becquerel.org/kyuba/curie.git
*/

#ifndef LIBCURIE_SIGNAL_H
#define LIBCURIE_SIGNAL_H

#include <curie/multiplex.h>

#ifdef __cplusplus
extern "C" {
#endif

/**\brief Signal Codes
 *
 * This is a list of signals which may be handled by the process. The list is
 * copied here so that the curie code doesn't need to worry about whether these
 * signals are actually available on the target platform.
 */
enum signal {
    sig_unused = 0,  /*!< stub code */

    /* posix.1-1990 */
    sig_hup = 1,     /*!< terminal hangup/controlling process died */
    sig_int = 2,     /*!< interrupt, for example with CTRL+C */
    sig_quit = 3,    /*!< quit */
    sig_ill = 4,     /*!< illegal instruction */
    sig_abrt = 5,    /*!< abort */
    sig_fpe = 6,     /*!< floating point exception */
    sig_kill = 7,    /*!< kill */
    sig_segv = 8,    /*!< segmentation violation */
    sig_pipe = 9,    /*!< broken pipe */
    sig_alrm = 10,   /*!< timer */
    sig_term = 11,   /*!< termination request */
    sig_chld = 12,   /*!< child stopped/terminated */
    sig_cont = 13,   /*!< continue */
    sig_stop = 14,   /*!< stop */
    sig_tstp = 15,   /*!< stop from tty */
    sig_ttin = 16,   /*!< tty input (background process) */
    sig_ttou = 17,   /*!< tty output (background process) */

    sig_usr1 = 18,   /*!< user-defined signal */
    sig_usr2 = 19,   /*!< user-defined signal */

    /* posix.1-2001 */
    sig_bus = 20,    /*!< bus error */
    sig_poll = 21,   /*!< pollable event */
    sig_prof = 22,   /*!< profiling timer expired */
    sig_sys = 23,    /*!< bad argument to routine */
    sig_trap = 24,   /*!< trace/breakpoint trap */
    sig_urg = 25,    /*!< urgent condition on socket */
    sig_vtalrm = 26, /*!< virtual alarm clock */
    sig_xcpu = 27,   /*!< cpu time limit exceeded */
    sig_xfsz = 28,   /*!< file size limit exceeded */

    /* random (i've seen these on linux) */
    sig_iot = 29,    /*!< IOT trap */
    sig_emt = 30,    /*!< ? */
    sig_stkflt = 31, /*!< stack fault on coprocessor */
    sig_io = 32,     /*!< I/O possible */
    sig_pwr = 33,    /*!< power failure */
    sig_info = 34,   /*!< power status information ? */
    sig_lost = 35,   /*!< file lock lost */
    sig_winch = 36   /*!< window resize */
};

/**\brief Signal Handler Result Code
 *
 * Possible result codes for a signal handler, which describe what will happen
 * to the handler.
 */
enum signal_callback_result {
    /**\brief Keep Signal Handler
     *
     * Returning this will not remove the signal handler from the list of
     * handlers.
     */
    scr_keep = 0,

    /**\brief Remove Signal Handler
     *
     * If this is returned, the handler that was just called will be removed
     * from the list of signal handlers.
     */
    scr_ditch = 1
};

/**\brief Catch Signals
 *
 * This function initialises the signal multiplexer, so that signals can be
 * handled by curie applications by grabbing them with multiplex_add_signal().
 */
void multiplex_signal ();

/**\brief Catch Signals
 *
 * This function is very similar to multiplex_signal(). The normal method of
 * handling signals uses a memory buffer to store the incoming signals. This
 * works very well with the multiplexer in regular programmes, i.e. those that
 * also wait for incoming data on stdio or some other socket or pipe. However,
 * this method fails to work properly on systems where the multiplexer uses
 * select() when no data is intended to come in through any of the other
 * multiplexers. This method uses the old self-pipe trick to check for
 * triggered signals, which works in this case and is generally more reliable,
 * but it also wastes precious file descriptors which might even be inherited
 * to child processes, which would then possibly also introduce security
 * issues.
 */
void multiplex_signal_primary ();

/**\brief Register Callback for a Signal
 * \param[in] signal  The signal to listen for.
 * \param[in] handler The function to call when the signal is caught.
 * \param[in] aux     Passed to the callback function.
 *
 * After calling this function, whenever the given signal comes in, the handler
 * function is called. This function may be called multiple times even for the
 * same signal -- it'll just add more callbacks for that signal then.
 */
void multiplex_add_signal
        (enum signal signal,
         enum signal_callback_result (*handler)(enum signal, void *),
         void *aux);

#ifdef __cplusplus
}
#endif

#endif
