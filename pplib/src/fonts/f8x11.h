/*
    created with FontEditor written by H. Reddmann
    HaReddmann at t-online dot de

    File Name           : f8x11.h
    Date                : 03.02.2023
    Font size in bytes  : 0x02AC, 684
    Font width          : 8
    Font height         : 11
    Font first char     : 0x20
    Font last char      : 0x7E
    Font bits per pixel : 1
    Font is compressed  : false

    The font data are defined as

    struct _FONT_ {
     // common shared fields
       uint16_t   font_Size_in_Bytes_over_all_included_Size_it_self;
       uint8_t    font_Width_in_Pixel_for_fixed_drawing;
       uint8_t    font_Height_in_Pixel_for_all_Characters;
       uint8_t    font_Bits_per_Pixels;
                    // if MSB are set then font is a compressed font
       uint8_t    font_First_Char;
       uint8_t    font_Last_Char;
       uint8_t    font_Char_Widths[font_Last_Char - font_First_Char +1];
                    // for each character the separate width in pixels,
                    // characters < 128 have an implicit virtual right empty row
                    // characters with font_Char_Widths[] == 0 are undefined

     // if compressed font then additional fields
       uint8_t    font_Byte_Padding;
                    // each Char in the table are aligned in size to this value
       uint8_t    font_RLE_Table[3];
                    // Run Length Encoding Table for Decompression
       uint8_t    font_Char_Size_in_Bytes[font_Last_Char - font_First_Char +1];
                    // for each char the size in (bytes / font_Byte_Padding) are stored,
                    // this get us the table to seek to the right beginning of each char
                    // in the font_data[]. Withhelp of font_Byte_Padding we ensure that
                    // one byte is suitable to store character sizes greater 255

     // for compressed and uncompressed fonts
       uint8_t    font_data[];
                    // bit field of all characters
    }
*/

#include <inttypes.h>
#include <avr/pgmspace.h>

#ifndef f8x11_H
#define f8x11_H

#define f8x11_WIDTH 8
#define f8x11_HEIGHT 11

static font_t font_8x11[] PROGMEM = {
    0x02, 0xAC, 0x08, 0x0B, 0x01, 0x20, 0x7E,
    0x03, 0x01, 0x04, 0x06, 0x05, 0x07, 0x05, 0x01, 0x02, 0x02, 0x03, 0x05, 0x02, 0x02, 0x01, 0x04, 
    0x05, 0x03, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x01, 0x02, 0x04, 0x05, 0x04, 0x05, 
    0x07, 0x07, 0x05, 0x06, 0x06, 0x05, 0x05, 0x06, 0x06, 0x01, 0x04, 0x06, 0x05, 0x07, 0x06, 0x06, 
    0x06, 0x06, 0x06, 0x05, 0x05, 0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x02, 0x04, 0x02, 0x05, 0x06, 
    0x02, 0x05, 0x05, 0x05, 0x05, 0x05, 0x02, 0x05, 0x05, 0x01, 0x01, 0x05, 0x01, 0x07, 0x05, 0x05, 
    0x05, 0x05, 0x02, 0x04, 0x02, 0x05, 0x05, 0x07, 0x04, 0x05, 0x04, 0x03, 0x01, 0x03, 0x06, 
    0x00, 0x00, 0x00, 0x00, 0xFE, 0x72, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x44, 0xF8, 0x0F, 0x11, 0x88, 
    0xF0, 0x1F, 0x22, 0x30, 0x42, 0x22, 0xFF, 0x13, 0x09, 0x31, 0x04, 0x51, 0x84, 0x12, 0x48, 0x02, 
    0x29, 0x44, 0x11, 0x84, 0x39, 0x32, 0x62, 0x12, 0x60, 0x80, 0xE4, 0x00, 0xFE, 0x09, 0x50, 0x80, 
    0xFC, 0xA3, 0x00, 0x02, 0x28, 0x00, 0x04, 0x20, 0xC0, 0x07, 0x08, 0x40, 0x00, 0x20, 0x80, 0x80, 
    0x00, 0x04, 0x00, 0x01, 0x0C, 0x18, 0x30, 0x60, 0x00, 0x7F, 0x04, 0x24, 0x20, 0x01, 0xF1, 0x87, 
    0x00, 0x04, 0xF0, 0x1F, 0xC1, 0x04, 0x25, 0x24, 0x11, 0x71, 0x88, 0x20, 0x02, 0x12, 0x91, 0x88, 
    0xB8, 0x03, 0x0C, 0x58, 0x30, 0xC2, 0x7F, 0x80, 0xF0, 0x89, 0x84, 0x24, 0x24, 0x21, 0xF1, 0xF0, 
    0x47, 0x44, 0x22, 0x12, 0x11, 0x71, 0x04, 0x20, 0x38, 0x31, 0x68, 0xC0, 0x00, 0xDC, 0x11, 0x91, 
    0x88, 0x44, 0xC4, 0x1D, 0x8E, 0x88, 0x48, 0x44, 0x22, 0xE2, 0x0F, 0x84, 0x00, 0x08, 0x21, 0x20, 
    0x80, 0x02, 0x22, 0x08, 0x02, 0x05, 0x28, 0x40, 0x01, 0x0A, 0x50, 0x20, 0x08, 0x22, 0xA0, 0x00, 
    0x02, 0x01, 0x04, 0x20, 0x2C, 0x11, 0x70, 0x00, 0x3C, 0x10, 0x42, 0x26, 0x4A, 0xD1, 0x09, 0x51, 
    0x70, 0x00, 0x0C, 0x1C, 0x98, 0x30, 0x04, 0x26, 0xC0, 0x01, 0x30, 0xFF, 0x89, 0x48, 0x44, 0x22, 
    0xE2, 0x0E, 0x7F, 0x04, 0x24, 0x20, 0x01, 0x09, 0x88, 0x20, 0xFE, 0x13, 0x90, 0x80, 0x04, 0x44, 
    0x10, 0x7C, 0xF8, 0x4F, 0x44, 0x22, 0x12, 0x91, 0x80, 0xFC, 0x27, 0x02, 0x11, 0x88, 0x40, 0x00, 
    0xFC, 0x11, 0x90, 0x80, 0x44, 0x24, 0x12, 0xF2, 0xF9, 0x0F, 0x04, 0x20, 0x00, 0x01, 0x08, 0xFC, 
    0xE7, 0x3F, 0xC0, 0x00, 0x08, 0x40, 0xFE, 0xF1, 0x1F, 0x0C, 0x90, 0x40, 0x08, 0x81, 0x00, 0xC8, 
    0x7F, 0x00, 0x02, 0x10, 0x80, 0x00, 0xE4, 0x3F, 0x0C, 0x80, 0x01, 0x30, 0x60, 0xC0, 0x80, 0xFF, 
    0xFC, 0xC7, 0x00, 0x18, 0x00, 0x01, 0x30, 0xFE, 0xE3, 0x8F, 0x80, 0x04, 0x24, 0x20, 0x01, 0xF1, 
    0xC7, 0x7F, 0x22, 0x10, 0x81, 0x08, 0x44, 0xC0, 0x01, 0xFE, 0x08, 0x48, 0x40, 0x82, 0x12, 0x18, 
    0x7F, 0xFD, 0x27, 0x02, 0x11, 0x88, 0x40, 0x04, 0xDC, 0xE3, 0x88, 0x88, 0x44, 0x24, 0x22, 0xE2, 
    0x08, 0x40, 0x00, 0xFE, 0x13, 0x80, 0x00, 0xFC, 0x03, 0x20, 0x00, 0x01, 0x08, 0x40, 0xFE, 0x31, 
    0x00, 0x0E, 0x80, 0x01, 0x30, 0x60, 0xE0, 0xC0, 0x00, 0xFE, 0x00, 0x18, 0x30, 0x70, 0x00, 0x0C, 
    0x80, 0xF9, 0xC3, 0x60, 0x88, 0x80, 0x02, 0x08, 0xA0, 0x80, 0x08, 0x83, 0x19, 0x00, 0x01, 0x10, 
    0x00, 0x1F, 0x04, 0x10, 0x60, 0x00, 0x81, 0x09, 0x4A, 0x48, 0x22, 0x92, 0x90, 0x82, 0x0C, 0xE4, 
    0x7F, 0x01, 0x32, 0x00, 0x06, 0xC0, 0x00, 0x98, 0x00, 0xFD, 0x8F, 0x00, 0x02, 0x08, 0x80, 0x00, 
    0x08, 0x00, 0x20, 0x00, 0x01, 0x08, 0x40, 0x00, 0x02, 0x50, 0x00, 0x04, 0x00, 0x0C, 0x94, 0xA0, 
    0x04, 0x25, 0xF0, 0xF9, 0x0F, 0x42, 0x10, 0x82, 0x10, 0x78, 0xC0, 0x03, 0x21, 0x08, 0x41, 0x08, 
    0x24, 0xE0, 0x81, 0x10, 0x84, 0x20, 0xE4, 0x3F, 0xF0, 0x40, 0x09, 0x4A, 0x50, 0x02, 0x0B, 0xFF, 
    0x24, 0x00, 0x9E, 0x08, 0x45, 0x28, 0x42, 0xF1, 0xF7, 0x1F, 0x08, 0x20, 0x00, 0x01, 0xF0, 0xC9, 
    0x4F, 0xFE, 0xFF, 0x03, 0x02, 0x28, 0x20, 0x02, 0x20, 0xFF, 0xC1, 0x0F, 0x02, 0x10, 0x00, 0x1F, 
    0x04, 0x20, 0x00, 0x3E, 0xF8, 0x81, 0x00, 0x02, 0x10, 0x00, 0x1F, 0x78, 0x20, 0x04, 0x21, 0x08, 
    0x81, 0x07, 0xFE, 0x11, 0x82, 0x10, 0x84, 0xC0, 0x03, 0x1E, 0x08, 0x41, 0x08, 0x42, 0xF0, 0x8F, 
    0x1F, 0x04, 0x40, 0x02, 0x25, 0x48, 0x81, 0x84, 0x3F, 0x10, 0x82, 0x0F, 0x80, 0x00, 0x04, 0x10, 
    0xF8, 0xC1, 0x00, 0x18, 0x00, 0x03, 0x06, 0x0C, 0xE0, 0x01, 0x30, 0x60, 0xC0, 0x00, 0x18, 0x00, 
    0x83, 0x07, 0xCC, 0x80, 0x01, 0x0C, 0x98, 0x01, 0x20, 0x1E, 0x01, 0x07, 0x08, 0x3C, 0x20, 0x06, 
    0x29, 0x28, 0xC1, 0x08, 0x08, 0xBC, 0x17, 0xC0, 0xFF, 0x05, 0xD0, 0x7B, 0x20, 0x20, 0x80, 0x00, 
    0x04, 0x40, 0x00, 0x02, 0x08, 0x00
};

#endif
