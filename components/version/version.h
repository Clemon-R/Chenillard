#ifndef _VERSION_H_
#define _VERSION_H_

/** \brief Version pattern structure
 *
 */
typedef struct __attribute__((__packed__))  version_s {
    const char* header;     ///< Version pattern header which allows the whole pattern to be found in the binary or in the flash
    const char* version;    ///< Project version, retrieved by parsing the application Makefile
    const char* hash;       ///< Git SHA1 ending with "-dirty" when the project is not yet commited
} version_t;

extern const version_t version ;    ///< Version structure constant instance

extern void version_print(const char* p_tag);

#endif // _VERSION_H_