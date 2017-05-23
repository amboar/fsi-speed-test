/*
 * Copyright 2017 IBM Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <inttypes.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define MEMORY "/dev/mem"
#define FSI_CLOCK_VAL 0x1e0
#define FSI_CLOCK_BIT 16

#define NDEBUG
#ifdef NDEBUG
#define printf(...)
#else
#define printf(...) printf(__VA_ARGS__)
#endif


static uint32_t readl(void *addr)
{
    printf("read from %p\n", addr);
    asm volatile("" : : : "memory");
    return *(volatile uint32_t *) addr;
}

static void writel(void *addr, uint32_t data)
{
    printf("write 0x%" PRIx32 " to %p\n", data, addr);
    asm volatile("" : : : "memory");
    *(volatile uint32_t *)addr = data;
}

int main(void)
{
    int fd;
    void *gc, *fsi;
    uint32_t up, down;

    printf("Opening " MEMORY "\n");

    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    printf("Mapping %d\n", fd);

    gc = mmap(NULL, 4 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x1e780000);
    if (gc == MAP_FAILED) {
        perror("mmap");
        exit(2);
    }

    printf("%d mapped to %p for %d\n", fd, gc, 4 * 1024);

    down = readl(gc + FSI_CLOCK_VAL) & ~(1 << FSI_CLOCK_BIT);
    up = down | (1 << FSI_CLOCK_BIT);
    fsi = gc + FSI_CLOCK_VAL;
    while (1) {
        writel(fsi, down);
        writel(fsi, up);
    }
}
