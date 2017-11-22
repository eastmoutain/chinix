#ifndef IOMAP_H
#define IOMAP_H

/**
 * i8253/i8254 timer port
 */
#define I8253_DATA_PORT          (0x40)
#define I8253_CONTROL_PORT       (0x43)

/**
 * i8042 keyboard port
 */
#define I8042_DATA_PORT          (0x60)
#define I8042_STATUS_PORT        (0x64)
#define I8042_COMMAND_PORT       (0x64)

/**
 * CGA port
 */
#define CGA_INDEX_PORT          (0x3D4)
#define CGA_DATA_PORT           (0x3D5)

#endif // IOMAP_H

