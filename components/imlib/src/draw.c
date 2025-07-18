/*****************************************************************************
 draw

 @date: 2023/11/10


*****************************************************************************/
#include "imlib.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "font.h"
#include "fmath.h"

/**
 * Calculate the starting address of the specified row of the image
 */
void *imlib_compute_row_ptr(const image_t *img, int y)
{
    switch (img->pixfmt)
    {
        case PIXFORMAT_BINARY:
            {
                return IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
            }
        case PIXFORMAT_GRAYSCALE:
            {
                return IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
            }
        case PIXFORMAT_RGB565:
            {
                return IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
            }
        default:
            {
                // This shouldn't happen, at least we return a valid memory block
                return img->data;
            }
    }
}

/**
 * Get the data of the specified pixel of the image
 */
inline int imlib_get_pixel_fast(image_t *img, const void *row_ptr, int x)
{
    switch (img->pixfmt)
    {
        case PIXFORMAT_BINARY:
            {
                return IMAGE_GET_BINARY_PIXEL_FAST((uint32_t *) row_ptr, x);
            }
        case PIXFORMAT_GRAYSCALE:
            {
                return IMAGE_GET_GRAYSCALE_PIXEL_FAST((uint8_t *) row_ptr, x);
            }
        case PIXFORMAT_RGB565:
            {
                return IMAGE_GET_RGB565_PIXEL_FAST((uint16_t *) row_ptr, x);
            }
        default:
            {
                return -1;
            }
    }
}

// Set pixel (handles boundary check and image type check).
/**
 * Set the value of the specified pixel of the image
 */
void imlib_set_pixel(image_t *img, int x, int y, int p)
{
    if ((0 <= x) && (x < img->w) && (0 <= y) && (y < img->h))
    {
        switch (img->pixfmt)
        {
            case PIXFORMAT_BINARY:
                {
                    IMAGE_PUT_BINARY_PIXEL(img, x, y, p);
                    break;
                }
            case PIXFORMAT_GRAYSCALE:
                {
                    IMAGE_PUT_GRAYSCALE_PIXEL(img, x, y, p);
                    break;
                }
            case PIXFORMAT_RGB565:
                {
                    IMAGE_PUT_RGB565_PIXEL(img, x, y, p);
                    break;
                }
            default:
                {
                    break;
                }
        }
    }
}

// https://stackoverflow.com/questions/1201200/fast-algorithm-for-drawing-filled-circles
/**
 * Fill a circular area
 * @param img: target image.
 * @param cx, cy: center coordinates of the circle.
 * @param r0, r1: radius range of the circle.
 * @param c: fill color.
 */
static void point_fill(image_t *img, int cx, int cy, int r0, int r1, int c)
{
    for (int y = r0; y <= r1; y++)
    {
        for (int x = r0; x <= r1; x++)
        {
            if (((x * x) + (y * y)) <= (r0 * r0))
            {
                imlib_set_pixel(img, cx + x, cy + y, c);
            }
        }
    }
}

/**
 * Set the color of a single pixel in the image, supporting anti-aliasing effect.
 * @param img: target image, type image_t, contains image width, height, pixel format and other information.
 * @param x, y: the pixel position to be set.
 * @param err: blending coefficient, ranging from 0 to 255, indicating the proportion of the new color c. A larger err value indicates that the original color occupies a larger proportion.
 * @param c: new color value, its specific meaning varies with the pixel format.
 */
static void imlib_set_pixel_aa(image_t *img, int x, int y, int err, int c)
{
    if (!((0 <= x) && (x < img->w) && (0 <= y) && (y < img->h)))
    {
        return;
    }

    switch (img->pixfmt)
    {
        case PIXFORMAT_BINARY:
            {
                uint32_t *ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                int old_c = IMAGE_GET_BINARY_PIXEL_FAST(ptr, x) * 255;
                int new_c = (((old_c * err) + ((c ? 255 : 0) * (256 - err))) >> 8) > 127;
                IMAGE_PUT_BINARY_PIXEL_FAST(ptr, x, new_c);
                break;
            }
        case PIXFORMAT_GRAYSCALE:
            {
                uint8_t *ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                int old_c = IMAGE_GET_GRAYSCALE_PIXEL_FAST(ptr, x);
                int new_c = ((old_c * err) + ((c & 0xff) * (256 - err))) >> 8;
                IMAGE_PUT_GRAYSCALE_PIXEL_FAST(ptr, x, new_c);
                break;
            }
        case PIXFORMAT_RGB565:
            {
                uint16_t *ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                int old_c = IMAGE_GET_RGB565_PIXEL_FAST(ptr, x);
                int old_c_r5 = COLOR_RGB565_TO_R5(old_c);
                int old_c_g6 = COLOR_RGB565_TO_G6(old_c);
                int old_c_b5 = COLOR_RGB565_TO_B5(old_c);
                int c_r5 = COLOR_RGB565_TO_R5(c);
                int c_g6 = COLOR_RGB565_TO_G6(c);
                int c_b5 = COLOR_RGB565_TO_B5(c);
                int new_c_r5 = ((old_c_r5 * err) + (c_r5 * (256 - err))) >> 8;
                int new_c_g6 = ((old_c_g6 * err) + (c_g6 * (256 - err))) >> 8;
                int new_c_b5 = ((old_c_b5 * err) + (c_b5 * (256 - err))) >> 8;
                int new_c = COLOR_R5_G6_B5_TO_RGB565(new_c_r5, new_c_g6, new_c_b5);
                IMAGE_PUT_RGB565_PIXEL_FAST(ptr, x, new_c);
                break;
            }
        default:
            {
                break;
            }
    }
}

// https://gist.github.com/randvoorhies/807ce6e20840ab5314eb7c547899de68#file-bresenham-js-L381
/**
 * Draw a line
 */
static void imlib_draw_thin_line(image_t *img, int x0, int y0, int x1, int y1, int c)
{
    const int dx = abs(x1 - x0);
    const int sx = x0 < x1 ? 1 : -1;
    const int dy = abs(y1 - y0);
    const int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    int e2, x2; // error value e_xy
    int ed = dx + dy == 0 ? 1 : fast_floorf(fast_sqrtf(dx * dx + dy * dy));

    for (;;)
    {
        // pixel loop
        imlib_set_pixel_aa(img, x0, y0, 256 * abs(err - dx + dy) / ed, c);
        e2 = err;
        x2 = x0;
        if (2 * e2 >= -dx)
        {
            // x step
            if (x0 == x1)
            {
                break;
            }
            if (e2 + dy < ed)
            {
                imlib_set_pixel_aa(img, x0, y0 + sy, 256 * (e2 + dy) / ed, c);
            }
            err -= dy;
            x0 += sx;
        }
        if (2 * e2 <= dy)
        {
            // y step
            if (y0 == y1)
            {
                break;
            }
            if (dx - e2 < ed)
            {
                imlib_set_pixel_aa(img, x2 + sx, y0, 256 * (dx - e2) / ed, c);
            }
            err += dx;
            y0 += sy;
        }
    }
}

// https://gist.github.com/randvoorhies/807ce6e20840ab5314eb7c547899de68#file-bresenham-js-L813
/**
 * Draw a line
 */
void imlib_draw_line(image_t *img, int x0, int y0, int x1, int y1, int c, int th)
{
    line_t line = {x0, y0, x1, y1};
    if (!lb_clip_line(&line, 0, 0, img->w, img->h))
    {
        return;
    }

    x0 = line.x1;
    y0 = line.y1;
    x1 = line.x2;
    y1 = line.y2;

    // plot an anti-aliased line of width th pixel
    const int ex = abs(x1 - x0);
    const int sx = x0 < x1 ? 1 : -1;
    const int ey = abs(y1 - y0);
    const int sy = y0 < y1 ? 1 : -1;
    int e2 = fast_floorf(fast_sqrtf(ex * ex + ey * ey)); // length

    if (th <= 1 || e2 == 0)
    {
        return imlib_draw_thin_line(img, x0, y0, x1, y1, c); // assert
    }

    int dx = ex * 256 / e2;
    int dy = ey * 256 / e2;
    th = 256 * (th - 1); // scale values

    if (dx < dy)
    {
        // steep line
        x1 = (e2 + th / 2) / dy;    // start offset
        int err = x1 * dy - th / 2; // shift error value to offset width
        for (x0 -= x1 * sx;; y0 += sy)
        {
            x1 = x0;
            imlib_set_pixel_aa(img, x1, y0, err, c); // aliasing pre-pixel
            for (e2 = dy - err - th; e2 + dy < 256; e2 += dy)
            {
                x1 += sx;
                imlib_set_pixel(img, x1, y0, c);         // pixel on the line
            }
            imlib_set_pixel_aa(img, x1 + sx, y0, e2, c); // aliasing post-pixel
            if (y0 == y1)
            {
                break;
            }
            err += dx; // y-step
            if (err > 256)
            {
                err -= dy;
                x0 += sx;
            } // x-step
        }
    }
    else
    {
        // flat line
        y1 = (e2 + th / 2) / dx;    // start offset
        int err = y1 * dx - th / 2; // shift error value to offset width
        for (y0 -= y1 * sy;; x0 += sx)
        {
            y1 = y0;
            imlib_set_pixel_aa(img, x0, y1, err, c); // aliasing pre-pixel
            for (e2 = dx - err - th; e2 + dx < 256; e2 += dx)
            {
                y1 += sy;
                imlib_set_pixel(img, x0, y1, c);         // pixel on the line
            }
            imlib_set_pixel_aa(img, x0, y1 + sy, e2, c); // aliasing post-pixel
            if (x0 == x1)
            {
                break;
            }
            err += dy; // x-step
            if (err > 256)
            {
                err -= dx;
                y0 += sy;
            } // y-step
        }
    }
}

/**
 * Draw a line
 */
void imlib_draw_arrow(image_t *img, int x0, int y0, int x1, int y1, int c, int th, int size)
{
    int dx = (x1 - x0);
    int dy = (y1 - y0);
    float length = fast_sqrtf((dx * dx) + (dy * dy));

    float ux = IM_DIV(dx, length);
    float uy = IM_DIV(dy, length);
    float vx = -uy;
    float vy = ux;

    int a0x = fast_roundf(x1 - (size * ux) + (size * vx * 0.5));
    int a0y = fast_roundf(y1 - (size * uy) + (size * vy * 0.5));
    int a1x = fast_roundf(x1 - (size * ux) - (size * vx * 0.5));
    int a1y = fast_roundf(y1 - (size * uy) - (size * vy * 0.5));

    imlib_draw_line(img, x0, y0, x1, y1, c, th);
    imlib_draw_line(img, x1, y1, a0x, a0y, c, th);
    imlib_draw_line(img, x1, y1, a1x, a1y, c, th);
}

static void xLine(image_t *img, int x1, int x2, int y, int c)
{
    while (x1 <= x2)
    {
        imlib_set_pixel(img, x1++, y, c);
    }
}

static void yLine(image_t *img, int x, int y1, int y2, int c)
{
    while (y1 <= y2)
    {
        imlib_set_pixel(img, x, y1++, c);
    }
}

/**
 * Draw a rectangle
 */
void imlib_draw_rectangle(image_t *img, int rx, int ry, int rw, int rh, int c, int thickness, bool fill)
{
    if (fill)
    {
        for (int y = ry, yy = ry + rh; y < yy; y++)
        {
            for (int x = rx, xx = rx + rw; x < xx; x++)
            {
                imlib_set_pixel(img, x, y, c);
            }
        }
    }
    else if (thickness > 0)
    {
        int thickness0 = (thickness - 0) / 2;
        int thickness1 = (thickness - 1) / 2;

        for (int i = rx - thickness0, j = rx + rw + thickness1, k = ry + rh - 1; i < j; i++)
        {
            yLine(img, i, ry - thickness0, ry + thickness1, c);
            yLine(img, i, k - thickness0, k + thickness1, c);
        }

        for (int i = ry - thickness0, j = ry + rh + thickness1, k = rx + rw - 1; i < j; i++)
        {
            xLine(img, rx - thickness0, rx + thickness1, i, c);
            xLine(img, k - thickness0, k + thickness1, i, c);
        }
    }
}

// https://gist.github.com/randvoorhies/807ce6e20840ab5314eb7c547899de68#file-bresenham-js-L404
/**
 * Draw circle
 */
static void imlib_draw_circle_thin(image_t *img, int cx, int cy, int r, int c, bool fill)
{
    int x = r;
    int y = 0;             // II. quadrant from bottom left to top right
    int err = 2 - (2 * r); // error of 1.step
    r = 1 - err;
    for (;;)
    {
        int i = 256 * abs(err + (2 * (x + y)) - 2) / r; // get blend value of pixel
        imlib_set_pixel_aa(img, cx + x, cy - y, i, c);  // I. Quadrant
        imlib_set_pixel_aa(img, cx + y, cy + x, i, c);  // II. Quadrant
        imlib_set_pixel_aa(img, cx - x, cy + y, i, c);  // III. Quadrant
        imlib_set_pixel_aa(img, cx - y, cy - x, i, c);  // IV. Quadrant
        if (fill)
        {
            xLine(img, cx, cx + x - 1, cy - y, c);
            yLine(img, cx + y, cy, cy + x - 1, c);
            xLine(img, cx - x + 1, cx, cy + y, c);
            yLine(img, cx - y, cy - x + 1, cy, c);
        }
        if (x == 0)
        {
            break;
        }
        int e2 = err;
        int x2 = x; // remember values
        if (err > y)
        {
            // x step
            i = 256 * (err + (2 * x) - 1) / r; // outward pixel
            if (i < 256)
            {
                imlib_set_pixel_aa(img, cx + x, cy - y + 1, i, c);
                imlib_set_pixel_aa(img, cx + y - 1, cy + x, i, c);
                imlib_set_pixel_aa(img, cx - x, cy + y - 1, i, c);
                imlib_set_pixel_aa(img, cx - y + 1, cy - x, i, c);
            }
            err -= (--x * 2) - 1;
        }
        if (e2 <= x2--)
        {
            // y step
            if (!fill)
            {
                i = 256 * (1 - (2 * y) - e2) / r; // inward pixel
                if (i < 256)
                {
                    imlib_set_pixel_aa(img, cx + x2, cy - y, i, c);
                    imlib_set_pixel_aa(img, cx + y, cy + x2, i, c);
                    imlib_set_pixel_aa(img, cx - x2, cy + y, i, c);
                    imlib_set_pixel_aa(img, cx - y, cy - x2, i, c);
                }
            }
            err -= (--y * 2) - 1;
        }
    }
}

// https://stackoverflow.com/questions/27755514/circle-with-thickness-drawing-algorithm
/**
 * Draw a circle
 */
void imlib_draw_circle(image_t *img, int cx, int cy, int r, int c, int thickness, bool fill)
{
    if ((r == 0) && (fill || (thickness > 0)))
    {
        imlib_set_pixel(img, cx, cy, c);
    }

    if ((r <= 0) || ((!fill) && (thickness <= 0)))
    {
        return;
    }

    if (thickness == 1 || fill)
    {
        imlib_draw_circle_thin(img, cx, cy, r + (IM_MAX(thickness, 0) / 2), c, fill);
    }
    else
    {
        int thickness0 = (thickness - 0) / 2;
        int thickness1 = (thickness - 1) / 2;

        int xo = r + thickness0;
        int xi = IM_MAX(r - thickness1, 0);
        int xi_tmp = xi;
        int y = 0;
        int erro = 1 - xo;
        int erri = 1 - xi;

        while (xo >= y)
        {
            xLine(img, cx + xi, cx + xo, cy + y, c);
            yLine(img, cx + y, cy + xi, cy + xo, c);
            xLine(img, cx - xo, cx - xi, cy + y, c);
            yLine(img, cx - y, cy + xi, cy + xo, c);
            xLine(img, cx - xo, cx - xi, cy - y, c);
            yLine(img, cx - y, cy - xo, cy - xi, c);
            xLine(img, cx + xi, cx + xo, cy - y, c);
            yLine(img, cx + y, cy - xo, cy - xi, c);

            y++;

            if (erro < 0)
            {
                erro += 2 * y + 1;
            }
            else
            {
                xo--;
                erro += 2 * (y - xo + 1);
            }

            if (y > xi_tmp)
            {
                xi = y;
            }
            else
            {
                if (erri < 0)
                {
                    erri += 2 * y + 1;
                }
                else
                {
                    xi--;
                    erri += 2 * (y - xi + 1);
                }
            }
        }

        // Anti-alias the outer and inner edges.
        imlib_draw_circle_thin(img, cx, cy, r + thickness0, c, false);
        imlib_draw_circle_thin(img, cx, cy, xi_tmp, c, false);
    }
}

// https://scratch.mit.edu/projects/50039326/
// Effect https://scratch.mit.edu/projects/50039326/editor/
/**
 * Draw a pixel with a distortion effect. Its design can be used to achieve a certain tilt or perspective effect.
 * @param img: target image for drawing.
 * @param x0 and y0: base point coordinates.
 * @param dx and dy: offset relative to the base point (x0, y0) to determine the drawing position.
 * @param shear_dx and shear_dy: tilt parameters to control the intensity and direction of the deformation.
 * @param r0 and r1: define the radius range of the fill area to control the size of the drawing area.
 * @param c: color value for filling the area.
 */
static void scratch_draw_pixel(image_t *img, int x0, int y0, int dx, int dy, float shear_dx, float shear_dy, int r0, int r1, int c)
{
    point_fill(img, x0 + dx, y0 + dy + fast_floorf((dx * shear_dy) / shear_dx), r0, r1, c);
}

// https://scratch.mit.edu/projects/50039326/
static void scratch_draw_line(image_t *img, int x0, int y0, int dx, int dy0, int dy1, float shear_dx, float shear_dy, int c)
{
    int y = y0 + fast_floorf((dx * shear_dy) / shear_dx);
    yLine(img, x0 + dx, y + dy0, y + dy1, c);
}

// https://scratch.mit.edu/projects/50039326/
static void scratch_draw_sheared_ellipse(image_t *img, int x0, int y0, int width, int height, bool filled, float shear_dx, float shear_dy, int c, int thickness)
{
    int thickness0 = (thickness - 0) / 2;
    int thickness1 = (thickness - 1) / 2;
    if (((thickness > 0) || filled) && (shear_dx != 0))
    {
        int a_squared = width * width;
        int four_a_squared = a_squared * 4;
        int b_squared = height * height;
        int four_b_squared = b_squared * 4;

        int x = 0;
        int y = height;
        int sigma = (2 * b_squared) + (a_squared * (1 - (2 * height)));

        while ((b_squared * x) <= (a_squared * y))
        {
            if (filled)
            {
                scratch_draw_line(img, x0, y0, x, -y, y, shear_dx, shear_dy, c);
                scratch_draw_line(img, x0, y0, -x, -y, y, shear_dx, shear_dy, c);
            }
            else
            {
                scratch_draw_pixel(img, x0, y0, x, y, shear_dx, shear_dy, -thickness0, thickness1, c);
                scratch_draw_pixel(img, x0, y0, -x, y, shear_dx, shear_dy, -thickness0, thickness1, c);
                scratch_draw_pixel(img, x0, y0, x, -y, shear_dx, shear_dy, -thickness0, thickness1, c);
                scratch_draw_pixel(img, x0, y0, -x, -y, shear_dx, shear_dy, -thickness0, thickness1, c);
            }

            if (sigma >= 0)
            {
                sigma += four_a_squared * (1 - y);
                y -= 1;
            }

            sigma += b_squared * ((4 * x) + 6);
            x += 1;
        }

        x = width;
        y = 0;
        sigma = (2 * a_squared) + (b_squared * (1 - (2 * width)));

        while ((a_squared * y) <= (b_squared * x))
        {
            if (filled)
            {
                scratch_draw_line(img, x0, y0, x, -y, y, shear_dx, shear_dy, c);
                scratch_draw_line(img, x0, y0, -x, -y, y, shear_dx, shear_dy, c);
            }
            else
            {
                scratch_draw_pixel(img, x0, y0, x, y, shear_dx, shear_dy, -thickness0, thickness1, c);
                scratch_draw_pixel(img, x0, y0, -x, y, shear_dx, shear_dy, -thickness0, thickness1, c);
                scratch_draw_pixel(img, x0, y0, x, -y, shear_dx, shear_dy, -thickness0, thickness1, c);
                scratch_draw_pixel(img, x0, y0, -x, -y, shear_dx, shear_dy, -thickness0, thickness1, c);
            }

            if (sigma >= 0)
            {
                sigma += four_b_squared * (1 - x);
                x -= 1;
            }

            sigma += a_squared * ((4 * y) + 6);
            y += 1;
        }
    }
}

// https://scratch.mit.edu/projects/50039326/
static void scratch_draw_rotated_ellipse(image_t *img, int x, int y, int x_axis, int y_axis, int rotation, bool filled, int c, int thickness)
{
    if ((x_axis > 0) && (y_axis > 0))
    {
        if ((x_axis == y_axis) || (rotation == 0))
        {
            scratch_draw_sheared_ellipse(img, x, y, x_axis / 2, y_axis / 2, filled, 1, 0, c, thickness);
        }
        else if (rotation == 90)
        {
            scratch_draw_sheared_ellipse(img, x, y, y_axis / 2, x_axis / 2, filled, 1, 0, c, thickness);
        }
        else
        {
            // Avoid rotations above 90.
            if (rotation > 90)
            {
                rotation -= 90;
                int temp = x_axis;
                x_axis = y_axis;
                y_axis = temp;
            }

            // Avoid rotations above 45.
            if (rotation > 45)
            {
                rotation -= 90;
                int temp = x_axis;
                x_axis = y_axis;
                y_axis = temp;
            }

            float theta = fast_atanf(IM_DIV(y_axis, x_axis) * (-tanf(IM_DEG2RAD(rotation))));
            float shear_dx = (x_axis * cosf(theta) * cosf(IM_DEG2RAD(rotation))) - (y_axis * sinf(theta) * sinf(IM_DEG2RAD(rotation)));
            float shear_dy = (x_axis * cosf(theta) * sinf(IM_DEG2RAD(rotation))) + (y_axis * sinf(theta) * cosf(IM_DEG2RAD(rotation)));
            float shear_x_axis = fast_fabsf(shear_dx);
            float shear_y_axis = IM_DIV((y_axis * x_axis), shear_x_axis);
            scratch_draw_sheared_ellipse(img, x, y, fast_floorf(shear_x_axis / 2), fast_floorf(shear_y_axis / 2), filled, shear_dx, shear_dy, c, thickness);
        }
    }
}

/**
 * Draw an ellipse
 * @param img: The target image on which the drawing operation will be performed.
 * @param cx and cy: The coordinates of the center of the ellipse.
 * @param rx and ry: The horizontal and vertical radii of the ellipse.
 * @param rotation: The rotation angle of the ellipse in degrees.
 * @param c: The color value of the ellipse.
 * @param thickness: The thickness of the ellipse border.
 * @param fill: A boolean value indicating whether to fill the ellipse.
 */
void imlib_draw_ellipse(image_t *img, int cx, int cy, int rx, int ry, int rotation, int c, int thickness, bool fill)
{
    int r = rotation % 180;
    if (r < 0)
    {
        r += 180;
    }

    scratch_draw_rotated_ellipse(img, cx, cy, rx * 2, ry * 2, r, fill, c, thickness);
}

/**
 * Draw a string, supporting multiple font attributes, such as character rotation, mirroring, scaling, etc.
 * @param img: target image.
 * @param x_off and y_off: starting position for drawing the string.
 * @param str: the string to be drawn.
 * @param c: string color.
 * @param scale: character scaling ratio.
 * @param x_spacing and y_spacing: horizontal and vertical spacing between characters.
 * @param mono_space: whether to enable monospaced font.
 * @param char_rotation: rotation angle of single character and whole string. 0, 90, 180, 360, etc.
 * @param char_hmirror and char_vflip: character horizontal mirror and vertical flip.
 * @param string_rotation: rotation angle of single character and whole string. 0, 90, 180, 360, etc.
 * @param string_hmirror and string_vflip: string horizontal mirror and vertical flip.
 */
typedef struct
{
    uint16_t w;
    uint16_t h;
    uint8_t *data;
} font_t;

font_t gfont;

// 8x16
// 16x16 displayed in two parts
void imlib_draw_string(image_t *img, int x_off, int y_off, const char *str, int c, float scale, int x_spacing, int y_spacing, bool mono_space, int char_rotation, bool char_hmirror, bool char_vflip, int string_rotation, bool string_hmirror, bool string_vflip)
{
    char_rotation %= 360;
    if (char_rotation < 0)
    {
        char_rotation += 360;
    }
    char_rotation = (char_rotation / 90) * 90;

    string_rotation %= 360;
    if (string_rotation < 0)
    {
        string_rotation += 360;
    }
    string_rotation = (string_rotation / 90) * 90;

    bool char_swap_w_h = (char_rotation == 90) || (char_rotation == 270);
    bool char_upsidedown = (char_rotation == 180) || (char_rotation == 270);

    /* Set font */
    if (gfont.data == NULL)
    {
        gfont.w = 16;
        gfont.h = 16;
        gfont.data = font_ascii_8x16;
    }

    if (string_hmirror)
    {
        x_off -= fast_floorf(gfont.w * scale) - 1;
    }
    if (string_vflip)
    {
        y_off -= fast_floorf(gfont.h * scale) - 1;
    }

    int org_x_off = x_off;
    int org_y_off = y_off;
    const int anchor = x_off;

    // for (char ch, last = '\0'; (ch = *str); str++, last = ch) {

    uint64_t unicode = 0;
    uint8_t bytes = 0;
    char ch = 0;
    glyph_t *g = (glyph_t *) malloc(sizeof(glyph_t));
    while (*str)
    {
        bytes = utf8_to_unicode(str, &unicode);

        /* Get single character data */
        if (bytes == 1)
        { // Latin range uses 8x16 dot matrix characters
            ch = unicode;
            g->w = 8;
            g->h = 16;
            g->data = font_ascii_8x16 + (unicode - 0x20) * 16;
        }
        else if (bytes == 3)
        {
            g->w = 16;
            g->h = 16;
            g->data = unicode_font16x16_start + unicode * 32;
            // printf("unicode: %llx\n", unicode);
            // continue;
        }

        // if ((last == '\r') && (ch == '\n')) {
        //     // handle "\r\n" strings
        //     continue;
        // }

        // if ((ch == '\n') || (ch == '\r')) {
        //     // handle '\n' or '\r' strings
        //     x_off = anchor;
        //     y_off += (string_vflip ? -1 : +1) * (fast_floorf((char_swap_w_h ? gfont.w : gfont.h) * scale) +
        //     y_spacing); // newline height == space height continue;
        // }

        // if ((ch < ' ') || (ch > '~')) {
        //     // handle unknown characters
        //     continue;
        // }

        // if (!mono_space) {
        //     // Find the first pixel set and offset to that.
        //     bool exit = false;

        //     if (!char_swap_w_h) {
        //         for (int x = 0, xx = g->w; x < xx; x++) {
        //             for (int y = 0, yy = g->h; y < yy; y++) {
        //                 if (g->data[(char_upsidedown ^ char_vflip) ? (g->h - 1 - y) : y] &
        //                     (1 << ((char_upsidedown ^ char_hmirror ^ string_hmirror) ? x : (g->w - 1 - x)))) {
        //                     x_off += (string_hmirror ? +1 : -1) * fast_floorf(x * scale);
        //                     exit = true;
        //                     break;
        //                 }
        //             }

        //             if (exit) {
        //                 break;
        //             }
        //         }
        //     } else {
        //         for (int y = g->h - 1; y >= 0; y--) {
        //             for (int x = 0, xx = g->w; x < xx; x++) {
        //                 if (g->data[(char_upsidedown ^ char_vflip) ? (g->h - 1 - y) : y] &
        //                     (1 << ((char_upsidedown ^ char_hmirror ^ string_hmirror) ? x : (g->w - 1 - x)))) {
        //                     x_off += (string_hmirror ? +1 : -1) * fast_floorf((g->h - 1 - y) * scale);
        //                     exit = true;
        //                     break;
        //                 }
        //             }

        //             if (exit) {
        //                 break;
        //             }
        //         }
        //     }
        // }

        if (bytes == 1)
        {
            for (int y = 0, yy = fast_floorf(g->h * scale); y < yy; y++)
            {
                for (int x = 0, xx = fast_floorf(g->w * scale); x < xx; x++)
                {
                    if (g->data[fast_floorf(y / scale)] & (1 << (g->w - 1 - fast_floorf(x / scale))))
                    {
                        int16_t x_tmp = x_off + (char_hmirror ? (xx - x - 1) : x);
                        int16_t y_tmp = y_off + (char_vflip ? (yy - y - 1) : y);
                        point_rotate(x_tmp, y_tmp, IM_DEG2RAD(char_rotation), x_off + (xx / 2), y_off + (yy / 2), &x_tmp, &y_tmp);
                        point_rotate(x_tmp, y_tmp, IM_DEG2RAD(string_rotation), org_x_off, org_y_off, &x_tmp, &y_tmp);
                        imlib_set_pixel(img, x_tmp, y_tmp, c);
                    }
                }
            }
        }
        else if (bytes == 3)
        {
            uint8_t mask = 0;
            uint8_t font_data[32];
            memcpy(font_data, &unicode_font16x16_start[unicode * 32], 32);
            for (int y = 0, yy = fast_floorf(g->h * scale); y < yy; y++)
            {
                for (int x = 0, xx = fast_floorf(g->w * scale); x < xx; x++)
                {
                    if (x < (int) (xx / 2.0))
                    {
                        if (font_data[fast_floorf(y / scale)] & (1 << ((int) (g->w / 2.0) - 1 - fast_floorf(x / scale))))
                        {
                            int16_t x_tmp = x_off + (char_hmirror ? (xx - x - 1) : x);
                            int16_t y_tmp = y_off + (char_vflip ? (yy - y - 1) : y);
                            point_rotate(x_tmp, y_tmp, IM_DEG2RAD(char_rotation), x_off + (xx / 2), y_off + (yy / 2), &x_tmp, &y_tmp);
                            point_rotate(x_tmp, y_tmp, IM_DEG2RAD(string_rotation), org_x_off, org_y_off, &x_tmp, &y_tmp);
                            imlib_set_pixel(img, x_tmp, y_tmp, c);
                        }
                    }
                    else
                    {
                        if (font_data[fast_floorf(y / scale) + 16] & (1 << (g->w - 1 - fast_floorf(x / scale))))
                        {
                            int16_t x_tmp = x_off + (char_hmirror ? (xx - x - 1) : x);
                            int16_t y_tmp = y_off + (char_vflip ? (yy - y - 1) : y);
                            point_rotate(x_tmp, y_tmp, IM_DEG2RAD(char_rotation), x_off + (xx / 2), y_off + (yy / 2), &x_tmp, &y_tmp);
                            point_rotate(x_tmp, y_tmp, IM_DEG2RAD(string_rotation), org_x_off, org_y_off, &x_tmp, &y_tmp);
                            imlib_set_pixel(img, x_tmp, y_tmp, c);
                        }
                    }
                }
            }
        }

        if (mono_space)
        {
            x_off += (string_hmirror ? -1 : +1) * (fast_floorf((char_swap_w_h ? g->h : g->w) * scale) + x_spacing);
        }
        else
        {
            // Find the last pixel set and offset to that.
            bool exit = false;

            if (!char_swap_w_h)
            {
                for (int x = g->w - 1; x >= 0; x--)
                {
                    for (int y = g->h - 1; y >= 0; y--)
                    {
                        if (g->data[(char_upsidedown ^ char_vflip) ? (g->h - 1 - y) : y] & (1 << ((char_upsidedown ^ char_hmirror ^ string_hmirror) ? x : (g->w - 1 - x))))
                        {
                            x_off += (string_hmirror ? -1 : +1) * (fast_floorf((x + 2) * scale) + x_spacing);
                            exit = true;
                            break;
                        }
                    }

                    if (exit)
                    {
                        break;
                    }
                }
            }
            else
            {
                for (int y = 0, yy = g->h; y < yy; y++)
                {
                    for (int x = g->w - 1; x >= 0; x--)
                    {
                        if (g->data[(char_upsidedown ^ char_vflip) ? (g->h - 1 - y) : y] & (1 << ((char_upsidedown ^ char_hmirror ^ string_hmirror) ? x : (g->w - 1 - x))))
                        {
                            x_off += (string_hmirror ? -1 : +1) * (fast_floorf(((g->h - 1 - y) + 2) * scale) + x_spacing);
                            exit = true;
                            break;
                        }
                    }

                    if (exit)
                    {
                        break;
                    }
                }
            }

            if (!exit)
            {
                x_off += (string_hmirror ? -1 : +1) * fast_floorf(scale * 3); // space char
            }
        }

        str += bytes;
    }
}

// for (int y = 0, yy = fast_floorf(g_h * scale); y < yy; y++) {
//    for (int x = 0, xx = fast_floorf(80); x < xx; x++) {
//        if (x <= 39) {
//            printf("->\n");
//            if (g_data[fast_floorf(y / scale)] & (1 << (g_w - 1 - fast_floorf(x / scale)))) {
//                int16_t x_tmp = x_off + (char_hmirror ? (xx - x - 1) : x);
//                int16_t y_tmp = y_off + (char_vflip ? (yy - y - 1) : y);
//                point_rotate(x_tmp, y_tmp, IM_DEG2RAD(char_rotation), x_off + (xx / 2), y_off + (yy / 2), &x_tmp,
//                &y_tmp); point_rotate(x_tmp, y_tmp, IM_DEG2RAD(string_rotation), org_x_off, org_y_off, &x_tmp,
//                &y_tmp); imlib_set_pixel(img, x_tmp, y_tmp, c); printf("left\n");
//            }
//        } else {
//            if (g_data[fast_floorf(y / scale) + 16] & (1 << (g_w - 1 - fast_floorf(x / scale)))) {
//                int16_t x_tmp = x_off + (char_hmirror ? (xx - x - 1) : x);
//                int16_t y_tmp = y_off + (char_vflip ? (yy - y - 1) : y);
//                point_rotate(x_tmp, y_tmp, IM_DEG2RAD(char_rotation), x_off + (xx / 2), y_off + (yy / 2), &x_tmp,
//                &y_tmp); point_rotate(x_tmp, y_tmp, IM_DEG2RAD(string_rotation), org_x_off, org_y_off, &x_tmp,
//                &y_tmp); imlib_set_pixel(img, x_tmp, y_tmp, c); printf("right\n");
//            }
//        }

//    }

// /**
//  * Draw 8x16 sized characters on the specified frame buffer.
//  */
// void imlib_draw_char_8x16(image_t *img, int32_t start_x, int32_t start_y, uint8_t ch, uint32_t color)
// {
//     uint8_t bytes_per_pixel = get_bytes_per_pixel(img->pixfmt);

//     // Load bitmap character data from font library
//     uint8_t font_data[16];
//     for (uint8_t i = 0; i < 16; i++) {
//         font_data[i] = font_ascii_8x16[(ch - 0x20) * 16 + i];
//     }

//     // Position to the starting location of the frame buffer.
//     uint8_t *dst_ptr = img->pixels + (start_y * img->w + start_x) * bytes_per_pixel;

//     uint8_t row_index, col_index, current_byte;
//     for (row_index = 0; row_index < 16; row_index++) {
//         current_byte = font_data[row_index];
//         for (col_index = 0; col_index < 8; col_index++) {
//             if (current_byte & 0x80) {
//                 set_pixel_color(dst_ptr + col_index * bytes_per_pixel, bytes_per_pixel, color);
//             }
//             current_byte <<= 1;
//         }
//         dst_ptr += img->w * bytes_per_pixel; // Move to the next line of the frame buffer.
//     }
// }

// /**
//  * Draw 16x16 sized characters on the specified frame buffer.
//  */
// void imlib_draw_char_16x16(image_t *img, int32_t start_x, int32_t start_y, uint32_t code, uint32_t color)
// {
//     uint8_t row, col;
//     uint8_t data1, data2, mask;
//     uint8_t font_data[32];

//     uint8_t bytes_per_pixel = get_bytes_per_pixel(img->pixfmt);

//     /* Get bitmap character data. Character is 16x16, single character takes 32 bytes. */
//     memcpy(font_data, &unicode_font16x16_start[code*32], 32);

//     /* Position to the starting location of the frame buffer. */
//     uint8_t *dst_ptr = img->pixels + (start_y * img->w + start_x) * bytes_per_pixel;

//     /* Font scanning method: top-down first, then left-right */
//     for (row = 0; row < 16; row++) {
//         data1 = font_data[row];      // Left half
//         data2 = font_data[16 + row]; // Right half
//         for (col = 0; col < 8; col++) {
//             mask = 0x80 >> col;
//             if (data1 & mask)
//                 set_pixel_color(dst_ptr + col * bytes_per_pixel , bytes_per_pixel, color);
//             if (data2 & mask)
//                 set_pixel_color(dst_ptr + (col + 8) * bytes_per_pixel, bytes_per_pixel, color);
//         }
//         dst_ptr += img->w * bytes_per_pixel; // Move to the next line of the frame buffer.
//     }
// }

// /**
//  * Draw string on the specified frame buffer.
//  */
// void imlib_draw_string(image_t *img, uint16_t x, uint16_t y, const char *str, uint32_t color)
// {
//     uint64_t unicode = 0;
//     uint8_t bytes;
//     uint16_t offset = 0;

//     while (*str) {
//         bytes = utf8_to_unicode(str, &unicode);
//         if (bytes == 1) { // ASCII font 8x16
//             imlib_draw_char_8x16(img, x+offset, y, unicode, color);
//             offset += 8;
//         } else if (bytes == 3) { // Chinese font 16x16
//             imlib_draw_char_16x16(img, x+offset, y, unicode, color);
//             offset += 16;
//         }
//         str += bytes;
//     }
// }
