#ifndef _QUBESDB_CLIENT_H
#define _QUBESDB_CLIENT_H

/** @file qubesdb-client.h
 * This file describes public QubesDB client interface
 *
 * QubesDB is contiguration interface for Qubes VMs. It consists of two daemons
 * per VM - one in dom0 and one in VM. If you want configure VM from dom0, you
 * need specify which VM at connection time.
 *
 * Database consists of (path, value) pairs. Path must begins with '/', must
 * not end with '/' and can have maximum QDB_MAX_PATH (64) characters
 * (including terminating NULL) from [a-zA-Z0-9_.:/-].  You can use '/' inside
 * of path to specify directories - then you can perform some operations (like
 * LIST, RM or WATCH) on whole directory.
 * Value can consists of at most QDB_MAX_DATA (3072) non-null bytes.
 *
 */


struct qdb_handle;

/** Type of QubesDB connection handle
 */
typedef struct qdb_handle* qdb_handle_t;

/** Open connection to QubesDB daemon for given domain
 *
 * Each VM have own QubesDB daemon, so you need as many connections as many VMs
 * you need to configure
 * @param vmname Name of VM to which you want connect or NULL to connect to
 * local daemon
 * @return Connection handle or NULL in case of failure, should be closed with
 * qdb_close after use
 */
qdb_handle_t qdb_open(char *vmname);

/** Close connection to QubesDB daemon
 * @param h Connection handle
 */
void qdb_close(qdb_handle_t h);

/** Read single value from QubesDB
 * @param h Connection handle
 * @param path Path to read
 * @param value_len[out] Size of returned data (optional)
 * @return Key contents (NULL terminated) or NULL on failure. Value must be freed with free().
 */
char *qdb_read(qdb_handle_t h, char *path, unsigned int *value_len);

/** Get path list matching given prefix
 * @param h Connection handle
 * @param path Path prefix to match
 * @param list_len[out] Length of returned list (optional)
 * @return NULL terminated list of NULL terminated strings with list of paths.
 *         Values must be freed with free().
 */
char **qdb_list(qdb_handle_t h, char *path, unsigned int *list_len);

/** Write single value to QubesDB, override existing entry
 * @param h Connection handle
 * @param path Path to write
 * @param value Value to write
 * @param value_len Size of 'value' param
 * @return 1 on success, 0 on failure
 */
int qdb_write(qdb_handle_t h, char *path, char *value, unsigned int value_len);

/** Remove value from QubesDB
 * @param h Connection handle
 * @param path Path to remove, if ends with '/' will remove whole directory
 * @return 1 on success (even if no entries removed), 0 on failure
 */
int qdb_rm(qdb_handle_t h, char *path);


/** Register watch for given path.
 * Fired events should be received via qdb_read_watch().
 * @param h Connection handle
 * @param path Path to watch, if ends with '/' will watch whole directory
 * @return 1 on success, 0 on failure
 */
int qdb_watch(qdb_handle_t h, char *path);

/** Unregister watch for given path.
 * Note that even (shortly) after qdb_unwatch you can receive events for such
 * watch. Probably you want to ignore them, but must be prepared to do so.
 * @param h Connection handle
 * @param path Path of watch to be unregistered.
 * @return 1 on success, 0 on failure
 */
int qdb_unwatch(qdb_handle_t h, char *path);

/** Wait for watch event. If some event received earlier (but after last
 * qdb_read_watch call) returns immediately, otherwise block. You can also use
 * select() on FD returned by qdb_watch_fd to wait for events.
 * @param h Connection handle
 * @return Modified path or NULL on failure. Value must be freed with free().
 */
char *qdb_read_watch(qdb_handle_t h);

/** Return FD for select().
 * @param h Connection handle
 * @return FD number to use in select() call
 */
int qdb_watch_fd(qdb_handle_t h);


#endif /* _QUBESDB_CLIENT_H */
