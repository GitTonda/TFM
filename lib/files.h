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

/**
 * @brief Frees the memory allocated for the directory contents.
 *
 * This function deallocates the memory used by the global variable `current_dir`
 * and all of its elements, which represent the entries of the previously loaded
 * directory. It also resets the global variable `n_files` to zero and sets
 * `current_dir` to NULL to indicate that no directory contents are currently loaded.
 *
 * If there is no memory allocated (i.e., `n_files` is zero or `current_dir` is NULL),
 * the function does nothing.
 *
 * @note This function assumes that `current_dir` and `n_files` are properly
 * initialized and accessible. It should only be called after a directory
 * has been loaded via a function such as `load_directory`.
 *
 * Global Variables:
 * - `struct dirent **current_dir`: Array of directory entries to be freed.
 * - `int n_files`: The number of loaded directory entries, which will be reset to zero.
 */
void free_directory();

unsigned long long calculate_folder_size(const char *path, int depth);
unsigned long long get_directory_size();

void delete();

#endif