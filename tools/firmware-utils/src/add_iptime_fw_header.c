/*
 * add_iptime_fw_header.c
 *
 * Copyright (C) 2020 Jaehoon You <teslamint@gmail.com>
 *
 * Based on add_header.c
 * Copyright (C) 2008 Imre Kaloz  <kaloz@openwrt.org>
 *                    Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/*
 * The add_iptime_fw_header utility used by specific ipTIME home router
 * with Mediatek SoC-based prepends the payload image with a 56 bytes +
 * padding 1992 bytes header containing a CRC32 value which is generated for
 * the model id + version + unused space + file size + reserved area for final
 * CRC32, and accumulate with CRC32 of payload buffer, then replaces the
 * reserved area with the actual CRC32. This replacement tool mimics this behavior.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <netinet/in.h>
#include <inttypes.h>

#include "cyg_crc.h"

struct header {
    unsigned char model[8];
    unsigned char version[8];
    uint32_t unused[8];
    uint32_t filesize;
    uint32_t crc;
};

static void usage(const char *) __attribute__ (( __noreturn__ ));

static void usage(const char *mess)
{
    fprintf(stderr, "Error: %s\n", mess);
    fprintf(stderr, "Usage: add_iptime_fw_header model_id version input_file output_file\n");
    fprintf(stderr, "\n");
    exit(1);
}

int main(int argc, char **argv)
{
    off_t len;			// of original buf
    off_t buflen;			// of the output file
    int fd;
    void *input_file;		// pointer to the input file (mmmapped)
    struct header header;
    unsigned char *final_buf; // pointer to final image (header + 1992 bytes of empty space + payload)
    cyg_uint32 header_crc;
    int i;

    // verify parameters
    if (argc != 5)
        usage("wrong number of arguments");

    // mmap input_file
    if ((fd = open(argv[3], O_RDONLY)) < 0
    || (len = lseek(fd, 0, SEEK_END)) < 0
    || (input_file = mmap(0, len, PROT_READ, MAP_SHARED, fd, 0)) == (void *) (-1)
    || (close(fd) < 0))
    {
        fprintf(stderr, "Error loading file %s: %s\n", argv[3], strerror(errno));
        exit(1);
    }

    // size of output file
    buflen = len + sizeof(header);

    // copy model name and version into header
    strncpy(header.model, argv[1], sizeof(header.model));
    strncpy(header.version, argv[2], sizeof(header.version));
    for (i = 0; i < 8; i += 1) {
        header.unused[i] = 0x0;
    }
    header.filesize = len;
    header.crc = 0;

    // get CRC of header
    header_crc = cyg_crc32_accumulate(0xffffffff, &header, sizeof(header));

    // get CRC of payload buffer with header CRC as initial value
    header.crc = (uint32_t)cyg_crc32_accumulate(header_crc, input_file, len);

    // create a firmware image in memory and copy the input_file to it
    final_buf = malloc(buflen);
    memcpy(final_buf, &header, sizeof(header));
    memcpy(&final_buf[sizeof(header)], input_file, len);

    // write the buf
    if ((fd = open(argv[4], O_CREAT|O_WRONLY|O_TRUNC,0644)) < 0
    || write(fd, final_buf, buflen) != buflen
    || close(fd) < 0)
    {
        fprintf(stderr, "Error storing file %s: %s\n", argv[3], strerror(errno));
        exit(2);
     }

    free(final_buf);

    munmap(input_file,len);

    return 0;
}
