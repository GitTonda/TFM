#ifndef TFM_TERM_KERNEL_H
#define TFM_TERM_KERNEL_H

extern struct termios orig_termios;

enum Key
{
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN
};

/**
 * Enables raw mode for the terminal.
 *
 * This function configures the terminal to operate in raw mode. In raw mode,
 * the terminal input is provided directly to the program without line buffering,
 * and without interpreting or processing special characters such as Ctrl+C.
 * Additionally, echoing of input characters is disabled.
 *
 * Raw mode is applied by modifying the terminal attributes using the `termios`
 * structure. The original terminal settings are saved in a global variable
 * `orig_termios`, which can later be restored using the `disableRawMode` function.
 *
 * This function also registers the `disableRawMode` function to be called
 * automatically on program exit using `atexit`. This ensures the terminal
 * settings are restored when the program terminates.
 *
 * As part of configuring raw mode, this function disables the display of the
 * cursor and switches the terminal to an alternate screen buffer for improved
 * control over the terminal's output and layout.
 *
 * Note that using this function will directly modify the behavior of the standard
 * input (STDIN). Restoring the terminal's original behavior is crucial after the
 * program completes to avoid leaving the terminal in an inconsistent state.
 */
void enableRawMode();

/**
 * Restores the terminal to its original settings.
 *
 * This function reverts the terminal's configuration back to its original state,
 * which was saved before enabling raw mode. It restores the default terminal
 * attributes using the `orig_termios` global variable.
 *
 * In addition to adjusting the terminal attributes, this function restores
 * the default screen buffer and enables the display of the cursor, which
 * were modified when raw mode was enabled.
 *
 * This function is typically called automatically at program exit by being
 * registered with `atexit`. Calling this function ensures that the terminal's
 * state is consistent and usable after the program terminates.
 *
 * Note: This function operates on the standard input (STDIN) terminal descriptor.
 */
void disableRawMode();

/**
 * Reads a single keypress from the terminal.
 *
 * This function waits for input from the terminal and returns the corresponding
 * keypress. It handles both regular characters and special keys, such as arrow
 * keys. Arrow keys and other escape sequences are interpreted and mapped to
 * predefined constant values defined in the `Key` enum.
 *
 * The function reads from `STDIN_FILENO` in raw mode to allow character-level
 * input without requiring the Enter key to be pressed. If an escape sequence
 * is detected (starting with the ESC key), additional characters are read to
 * determine if it corresponds to a special key. If an unrecognized or incomplete
 * escape sequence is detected, it defaults to returning the ESC key.
 *
 * @return The ASCII value of the key pressed or a mapped value for special keys
 *         such as arrow keys. If no input is available, it returns '\0'.
 */
int readKey();

#endif