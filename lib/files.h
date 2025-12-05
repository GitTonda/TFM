#ifndef TFM_FILES_H
#define TFM_FILES_H

extern struct dirent **current_dir;
extern int n_files;

/**
 * @brief Loads the contents of the current directory into memory.
 *
 * This function retrieves the files and directories present in the current
 * working directory and stores them in the global variable `current_dir` as
 * an array of `struct dirent` pointers. The total number of files and
 * directories is stored in the global variable `n_files`.
 *
 * If the `current_dir` array is already populated, it will first free the
 * existing memory to prevent memory leaks.
 *
 * On failure, such as an error from `scandir`, the function will print an
 * error message and exit the program with a failure status.
 *
 * The entries in `current_dir` are sorted in alphanumeric order by the
 * `scandir` function.
 *
 * @note The caller is responsible for ensuring that the contents of
 * `current_dir` are properly managed and freed when no longer needed.
 *
 * Global Variables:
 * - `struct dirent **current_dir`: Array of directory entries loaded by
 *   the function.
 * - `int n_files`: The number of files and directories in the current
 *   working directory.
 *
 * Dependencies:
 * - Uses the `scandir`, `alphasort`, `perror`, and `exit` functions
 *   from the C standard library.
 * - Assumes `current_dir` and `n_files` are defined and accessible.
 */
void load_directory();

#endif