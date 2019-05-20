/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include "esp_log.h"

#include "version.h"
#include "version_hash.h"

/*----------------------------------------------------------------------------*/

#define VERSION_HEADER  "$VER"

/*----------------------------------------------------------------------------*/

const version_t version __attribute__((aligned(8))) = {
    .header  = VERSION_HEADER "\0|\0",
    .version = VERSION_VERSION "\0|\0",
    .hash    = VERSION_HASH "\0|\0"
};

/*----------------------------------------------------------------------------*/

/** \brief Prints the version structure into the output interface
 *  \param [in] p_tag   pointer to the module log tag
 *
 *  This function uses the esp_log function to dump the version pattern.
 *
 *  When  the tag is null, a single printf is prerformed.
 */
void version_print(const char* p_tag)
{
    if (p_tag) {
        ESP_LOGI(p_tag, "Version Header: \"%s\"", version.header);
        ESP_LOGI(p_tag, "Version:        \"%s\"", version.version);
        ESP_LOGI(p_tag, "Version Hash:   \"%s\"", version.hash);
    } else {
        printf("Version Header: \"%s\"\n", version.header);
        printf("Version:        \"%s\"\n", version.version);
        printf("Version Hash:   \"%s\"\n", version.hash);
    }
}

/*----------------------------------------------------------------------------*/