#pragma once

#include <endian.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/usb/functionfs.h>

/* Little endian handling */

/*
 * cpu_to_le16/32 are used when initializing structures, a context where a
 * function call is not allowed. To solve this, we code cpu_to_le16/32 in a way
 * that allows them to be used when initializing structures.
 */

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define cpu_to_le16(x)  (x)
#define cpu_to_le32(x)  (x)
#else
#define cpu_to_le16(x)  ((((x) >> 8) & 0xffu) | (((x) & 0xffu) << 8))
#define cpu_to_le32(x)  \
	((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >>  8) | \
	(((x) & 0x0000ff00u) <<  8) | (((x) & 0x000000ffu) << 24))
#endif

#define le32_to_cpu(x)  le32toh(x)
#define le16_to_cpu(x)  le16toh(x)


/* USB descriptors written to control endpoint ep0 */
/* */

#define MAX_PACKET_SIZE_FS 64
#define MAX_PACKET_SIZE_HS 512
#define MAX_PACKET_SIZE_SS 1024
#define BMAXBURST (15)

struct func_desc {
    struct usb_interface_descriptor intf;
    struct usb_endpoint_descriptor_no_audio sink;
    struct usb_endpoint_descriptor_no_audio source;

} __attribute__((packed));

struct ss_func_desc {
    struct usb_interface_descriptor intf;
    struct usb_endpoint_descriptor_no_audio sink;
    struct usb_ss_ep_comp_descriptor sink_comp;
    struct usb_endpoint_descriptor_no_audio source;
    struct usb_ss_ep_comp_descriptor source_comp;
} __attribute__((packed));

struct desc_v2 {
    struct usb_functionfs_descs_head_v2 header;
    // The rest of the structure depends on the flags in the header.
    __le32 fs_count;
    __le32 hs_count;
    __le32 ss_count;
    struct func_desc fs_descs, hs_descs;
    struct ss_func_desc ss_descs;
} __attribute__((packed));

const struct usb_interface_descriptor interface_desc = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = USB_DT_INTERFACE,
    .bInterfaceNumber = 0,
    .bNumEndpoints = 4,
    .bInterfaceClass = USB_CLASS_VENDOR_SPEC,
    .bInterfaceSubClass = 1,
    .bInterfaceProtocol = 1,
    .iInterface = 1,
};

/* Full speed descriptors */

const struct usb_endpoint_descriptor_no_audio fs_sink = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = 1 | USB_DIR_IN,
    .bmAttributes = USB_ENDPOINT_XFER_BULK,
    .wMaxPacketSize = MAX_PACKET_SIZE_FS,
};

const struct usb_endpoint_descriptor_no_audio fs_source = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = 2 | USB_DIR_OUT,
    .bmAttributes = USB_ENDPOINT_XFER_BULK,
    .wMaxPacketSize = MAX_PACKET_SIZE_FS,
};


/* High speed descriptors */

const struct usb_endpoint_descriptor_no_audio hs_sink = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = 1 | USB_DIR_IN,
    .bmAttributes = USB_ENDPOINT_XFER_BULK,
    .wMaxPacketSize = MAX_PACKET_SIZE_HS,
};

const struct usb_endpoint_descriptor_no_audio hs_source = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = 2 | USB_DIR_OUT,
    .bmAttributes = USB_ENDPOINT_XFER_BULK,
    .wMaxPacketSize = MAX_PACKET_SIZE_HS,
};

/* Super speed descriptors */

const struct usb_endpoint_descriptor_no_audio ss_sink = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = 1 | USB_DIR_IN,
    .bmAttributes = USB_ENDPOINT_XFER_BULK,
    .wMaxPacketSize = MAX_PACKET_SIZE_SS,
};

const struct usb_ss_ep_comp_descriptor ss_sink_comp = {
    .bLength = sizeof(ss_sink_comp),
    .bDescriptorType = USB_DT_SS_ENDPOINT_COMP,
    .bMaxBurst = BMAXBURST,
};

const struct usb_endpoint_descriptor_no_audio ss_source = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = 2 | USB_DIR_OUT,
    .bmAttributes = USB_ENDPOINT_XFER_BULK,
    .wMaxPacketSize = MAX_PACKET_SIZE_SS,
};

const struct usb_ss_ep_comp_descriptor ss_source_comp = {
    .bLength = sizeof(ss_source_comp),
    .bDescriptorType = USB_DT_SS_ENDPOINT_COMP,
    .bMaxBurst = BMAXBURST,
};

const struct func_desc fs_descriptors = {
    .intf = interface_desc,
    .sink = fs_sink,
    .source = fs_source,
};

const struct func_desc hs_descriptors = {
    .intf = interface_desc,
    .sink = hs_sink,
    .source = hs_source,
};

const struct ss_func_desc ss_descriptors = {
    .intf = interface_desc,
    .sink = ss_sink,
    .sink_comp = ss_sink_comp,
    .source = ss_source,
    .source_comp = ss_source_comp,
};


#define STR_INTERFACE "Source/Sink"

const struct {
    struct usb_functionfs_strings_head header;
    struct {
        __le16 code;
        const char str1[sizeof(STR_INTERFACE)];
    } __attribute__((packed)) lang0;
} __attribute__((packed)) strings = {
    .header = {
        .magic = cpu_to_le32(FUNCTIONFS_STRINGS_MAGIC),
        .length = cpu_to_le32(sizeof(strings)),
        .str_count = cpu_to_le32(1),
        .lang_count = cpu_to_le32(1),
    },
    .lang0 = {
        .code = cpu_to_le16(0x0409),
        .str1 = STR_INTERFACE,
    },
};