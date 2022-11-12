#include <stdio.h>
#include <stdlib.h>
#include "libusb.h"

void cleanup(libusb_device_handle* dev_handle, libusb_context* ctx)
{
    if (dev_handle != NULL) libusb_close(dev_handle); // close the device
    if (ctx != NULL) libusb_exit(ctx); // exit the context
}

int main()
{
    libusb_device_handle *dev_handle = NULL; // a device handle
    libusb_context *ctx = NULL; // a libusb session
    int r; // for return values
    r = libusb_init(&ctx); // initialize the library for the session we just declared
    if (r != 0)
    {
        printf("Error initializing libusb: %s\n", libusb_error_name(r));
        cleanup(dev_handle, ctx);
        return -1;
    }
    // libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, 3);
    // libusb_set_debug(ctx, 3); // set verbosity level to 3, as suggested in the documentation

    // pair (vendorID, productID) which is unique to Razer Nari USB dongle
    dev_handle = libusb_open_device_with_vid_pid(ctx, 0x1532, 0x051c);
    if (dev_handle == NULL)
    {
        printf("Error opening device\n");
        cleanup(dev_handle, ctx);
        return -2;
    }
    printf("Device opened\n");

    const int interface_id = 5; // Razer Nari USB dongle has 3 interfaces: 0 - chat, 3 - game, 5 - hid
    if (libusb_kernel_driver_active(dev_handle, interface_id) == 1) // if a kernel driver is active
    {
        printf("Kernel driver active\n");
        r = libusb_detach_kernel_driver(dev_handle, interface_id); // detach it because device with attached kernel driver is inaccessible
        if (r != 0)
        {
            printf("Error detaching kernel driver: %s\n", libusb_error_name(r));
            cleanup(dev_handle, ctx);
            return -3;
        }
        else
            printf("Kernel driver detached\n");
    }
    r = libusb_claim_interface(dev_handle, interface_id);
    if (r != 0)
    {
        printf("Error claiming interface: %s\n", libusb_error_name(r));
        cleanup(dev_handle, ctx);
        return -4;
    }
    printf("Claimed interface\n");

    // the first control transfer (1st frame 'SET_REPORT Request') that we need to fabricate
    uint8_t *data = malloc(64);
    data[0] = 0xff;
    data[1] = 0x0a;
    data[2] = 0x00;
    data[3] = 0xfd;
    data[4] = 0x04;
    data[5] = 0x12;
    data[6] = 0xf1;
    data[7] = 0x02;
    data[8] = 0x05;
    int i;
    for (i = 9; i < 64; ++i)
        data[i] = 0x00;
    /* ff 0a 00 fd 04 12 f1 02 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 (54 times 00 byte)*/

    /* From libusb_control_transfer documentation:
    dev_handle      a handle for the device to communicate with
    bmRequestType   the request type field for the setup packet
    bRequest        the request field for the setup packet
    wValue          the value field for the setup packet
    wIndex          the index field for the setup packet
    data            a suitably-sized data buffer for either input or output (depending on direction bits within bmRequestType)
    wLength         the length field for the setup packet. The data buffer should be at least this size.
    timeout         timeout (in milliseconds) that this function should wait before giving up due to no response being received. For an unlimited timeout, use value 0. */
    // https://libusb.sourceforge.io/api-1.0/group__libusb__syncio.html
    // parameters from 'SET_REPORT Request' Setup Data
    r = libusb_control_transfer(dev_handle, 0x21, 0x09, 0x03ff, 5, data, 64, 0);
    if (r < 0)
    {
        printf("Error requesting data: %s\n", libusb_error_name(r));
        cleanup(dev_handle, ctx);
        return -5;
    }
    else
        printf("Request succesful\n");

    // We ignore the 2nd frame 'SET_REPORT Response' because we assume that the device manages to prepare the report for the host. If it doesn't, in 3-4 request-response frame pair we still get some invalid data (e.g. 0 millivolts after decoding).

    for (i = 0; i < 64; ++i) // set all data bytes to 0
        data[i] = 0;
    // the second control transfer (3rd frame 'GET_REPORT Request') that we need to fabricate
    // parameters from 'GET_REPORT Request' Setup Data
    r = libusb_control_transfer(dev_handle, 0xa1, 0x01, 0x03ff, 5, data, 64, 0);
    if (r < 0)
    {
        printf("Error receiving data: %s\n", libusb_error_name(r));
        cleanup(dev_handle, ctx);
        return -6;
    }
    else
        printf("Data receipt succesful\n");

    // bytes received in the 4th frame 'GET_REPORT Response' should form the following structure
    // index: 0   1  2  3  4  5  6  7  8  9  10 11  12  13  14 15 ...
    // byte:  |ff 01 00 fe 07 04 1f 14 02 30 30 31| |35 2c| 63 6c ...
    //        ^not really important to us           ^battery voltage to be interpreted as 16-bit unsigned value
    printf("Received data: ");
    for (i = 0; i < 64; ++i)
        printf("%02X ", (unsigned int)data[i]);
    printf("\n");

    r = libusb_release_interface(dev_handle, interface_id); // release the claimed interface
    if (r != 0)
    {
        printf("Error releasing interface: %s\n", libusb_error_name(r));
        r = libusb_attach_kernel_driver(dev_handle, interface_id); // reattach the detached kernel driver
        if (r != 0)
            printf("Error reattaching kernel driver: %s\n", libusb_error_name(r));
        else
            printf("Kernel driver reattached\n");
        cleanup(dev_handle, ctx);
        return -7;
    }
    printf("Released interface\n");
    r = libusb_attach_kernel_driver(dev_handle, interface_id); // reattach the detached kernel driver
    if (r != 0)
        printf("Error reattaching kernel driver: %s\n", libusb_error_name(r));
    else
        printf("Kernel driver reattached\n");
    cleanup(dev_handle, ctx);

    // after the end of the transmission, decode the battery voltage
    uint16_t value;
    char *byte = (char*) &value; // pointer to the less-significant byte of the battery voltage variable
    *byte = data[13]; // write the 13-th received byte as the less-significant byte of the battery voltage variable
    byte += 1; // pointer to the more-significant byte of the battery voltage variable
    *byte = data[12]; // write the 12-th received byte as the more-significant byte of the battery voltage variable
    free(data); // deallocate data memory
    printf("Battery voltage: %hu millivolt\nPress return to exit.", value);
    getchar();

    return 0;
}
