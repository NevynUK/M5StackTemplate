#include "utils.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "driver/ppa.h"
#include "driver/jpeg_decode.h"
#include "driver/jpeg_encode.h"

/**
 * Get the byte length of a utf-8 character
 * @param input_byte: the first byte of the input utf-8 character
 * @return: the byte length of the character
 */
static int get_utf8_byte_size(const char input_byte)
{
    // Determine the length of the character based on the first byte of the utf-8 encoding
    if (input_byte < 0x80)
        return 1;
    if (input_byte < 0xC0)
        return -1;
    if (input_byte < 0xE0)
        return 2;
    if (input_byte < 0xF0)
        return 3;
    if (input_byte < 0xF8)
        return 4;
    if (input_byte < 0xFC)
        return 5;
    return 6;
}

/**
 * Convert utf-8 encoding to unicode encoding
 * @param utf8_input: input utf-8 string
 * @param unicode_output: output unicode character
 * @return: byte length of the utf-8 character
 */
int utf8_to_unicode(const char *utf8_input, uint64_t *unicode_output)
{
    assert(utf8_input != NULL && unicode_output != NULL);
    *unicode_output = 0;

    int utf_bytes = get_utf8_byte_size(*utf8_input);
    uint8_t *output = (uint8_t *) unicode_output;

    switch (utf_bytes)
    {
        case 1:
            *output = *utf8_input;
            break;
        case 2:
            if ((*(utf8_input + 1) & 0xC0) != 0x80)
                return 0;
            *output = (*utf8_input << 6) + (*(utf8_input + 1) & 0x3F);
            output[1] = (*utf8_input >> 2) & 0x07;
            break;
        case 3:
            if ((*(utf8_input + 1) & 0xC0) != 0x80 || (*(utf8_input + 2) & 0xC0) != 0x80)
                return 0;
            *output = (*(utf8_input + 1) << 6) + (*(utf8_input + 2) & 0x3F);
            output[1] = (*utf8_input << 4) + ((*(utf8_input + 1) >> 2) & 0x0F);
            break;
        case 4:
            if ((*(utf8_input + 1) & 0xC0) != 0x80 || (*(utf8_input + 2) & 0xC0) != 0x80 || (*(utf8_input + 3) & 0xC0) != 0x80)
                return 0;
            *output = (*(utf8_input + 2) << 6) + (*(utf8_input + 3) & 0x3F);
            output[1] = (*(utf8_input + 1) << 4) + ((*(utf8_input + 2) >> 2) & 0x0F);
            output[2] = ((*utf8_input << 2) & 0x1C) + ((*(utf8_input + 1) >> 4) & 0x03);
            break;
        case 5:
            if ((*(utf8_input + 1) & 0xC0) != 0x80 || (*(utf8_input + 2) & 0xC0) != 0x80 || (*(utf8_input + 3) & 0xC0) != 0x80 || (*(utf8_input + 4) & 0xC0) != 0x80)
                return 0;
            *output = (*(utf8_input + 3) << 6) + (*(utf8_input + 4) & 0x3F);
            output[1] = (*(utf8_input + 2) << 4) + ((*(utf8_input + 3) >> 2) & 0x0F);
            output[2] = (*(utf8_input + 1) << 2) + ((*(utf8_input + 2) >> 4) & 0x03);
            output[3] = (*utf8_input << 6);
            break;
        case 6:
            if ((*(utf8_input + 1) & 0xC0) != 0x80 || (*(utf8_input + 2) & 0xC0) != 0x80 || (*(utf8_input + 3) & 0xC0) != 0x80 || (*(utf8_input + 4) & 0xC0) != 0x80 || (*(utf8_input + 5) & 0xC0) != 0x80)
                return 0;
            *output = (*(utf8_input + 4) << 6) + (*(utf8_input + 5) & 0x3F);
            output[1] = (*(utf8_input + 3) << 4) + ((*(utf8_input + 4) >> 2) & 0x0F);
            output[2] = (*(utf8_input + 2) << 2) + ((*(utf8_input + 3) >> 4) & 0x03);
            output[3] = ((*utf8_input << 6) & 0x40) + (*(utf8_input + 1) & 0x3F);
            break;
        default:
            return 0;
    }

    return utf_bytes;
}

/**
 *
 */
int image_jpg_read(image_t **img_out, char *path)
{
    // uint32_t last_time = esp_timer_get_time();
    FILE *fp = fopen(path, "rb");
    if (fp == NULL)
    {
        printf("open %s to read fail\n", path);
        return -1;
    }

    image_t *img_jpg = (image_t *) malloc(sizeof(image_t));
    fseek(fp, 0, SEEK_END);
    img_jpg->size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    img_jpg->pixfmt = PIXFORMAT_JPEG;
    img_jpg->data = (uint8_t *) heap_caps_malloc(img_jpg->size, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    if (img_jpg->data == NULL)
    {
        ESP_LOGE(TAG, "alloc memory for img_jpg->data failed");
    }
    fread(img_jpg->data, 1, img_jpg->size, fp);
    fclose(fp);

    // ESP_LOGI(TAG, "jpeg read spend time %lld ms.", (esp_timer_get_time() - last_time) / 1000);
    // last_time = esp_timer_get_time();
    *img_out = app_jpeg_decode(img_jpg, img_jpg->size); // img memory allocated internally by app_jpeg_decode()
    // ESP_LOGI(TAG, "jpeg decode spend time %lld ms.", (esp_timer_get_time() - last_time) / 1000);

    heap_caps_free(img_jpg->data);

    return 0;
}
