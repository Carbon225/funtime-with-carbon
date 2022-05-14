#include "pgm.h"

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

typedef enum parser_state_t
{
    PARSER_INIT,
    PARSER_ERR,
    PARSER_P,
    PARSER_2,
    PARSER_P2_SPACE,
    PARSER_WIDTH,
    PARSER_WIDTH_SPACE,
    PARSER_HEIGHT,
    PARSER_HEIGHT_SPACE,
    PARSER_MAX,
    PARSER_MAX_SPACE,
    PARSER_PIXEL,
} parser_state_t;

static int digittoint(int c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else
        return 0;
}

int pgm_load(image_t *img, const char *path)
{
    if (!img || !path) return -1;

    FILE *f = fopen(path, "r");
    if (!f) return -1;

    img->w = 0;
    img->h = 0;
    img->max = 0;
    img->data = 0;
    int pixel_i = 0;
    parser_state_t state = PARSER_INIT;
    bool comment_active = false;

    for (;;)
    {
        int c = fgetc(f);
        if (c == EOF) break;

        if (comment_active)
        {
            if (c == '\n')
            {
                comment_active = false;
            }
            else
            {
                continue;
            }
        }
        else if (c == '#')
        {
            comment_active = true;
            continue;
        }

        switch (state)
        {
            case PARSER_ERR:
                break;

            case PARSER_INIT:
                if (c == 'P')
                {
                    state = PARSER_P;
                }
                else
                {
                    state = PARSER_ERR;
                }
                break;

            case PARSER_P:
                if (c == '2')
                {
                    state = PARSER_2;
                }
                else
                {
                    state = PARSER_ERR;
                }
                break;

            case PARSER_2:
                if (isspace(c))
                {
                    state = PARSER_P2_SPACE;
                }
                else
                {
                    state = PARSER_ERR;
                }
                break;

            case PARSER_P2_SPACE:
                if (isdigit(c))
                {
                    state = PARSER_WIDTH;
                    img->w = img->w * 10 + digittoint(c);
                }
                else if (isspace(c));
                else
                {
                    state = PARSER_ERR;
                }
                break;

            case PARSER_WIDTH:
                if (isdigit(c))
                {
                    img->w = img->w * 10 + digittoint(c);
                }
                else if (isspace(c))
                {
                    state = PARSER_WIDTH_SPACE;
                }
                else
                {
                    state = PARSER_ERR;
                }
                break;

            case PARSER_WIDTH_SPACE:
                if (isdigit(c))
                {
                    state = PARSER_HEIGHT;
                    img->h = img->h * 10 + digittoint(c);
                }
                else if (isspace(c));
                else
                {
                    state = PARSER_ERR;
                }
                break;

            case PARSER_HEIGHT:
                if (isdigit(c))
                {
                    img->h = img->h * 10 + digittoint(c);
                }
                else if (isspace(c))
                {
                    img->data = malloc(img->h * img->w);
                    if (!img->data)
                    {
                        state = PARSER_ERR;
                    }
                    else
                    {
                        state = PARSER_HEIGHT_SPACE;
                    }
                }
                else
                {
                    state = PARSER_ERR;
                }
                break;

            case PARSER_HEIGHT_SPACE:
                if (isdigit(c))
                {
                    state = PARSER_MAX;
                    img->max = img->max * 10 + digittoint(c);
                }
                else if (isspace(c));
                else
                {
                    state = PARSER_ERR;
                }
                break;

            case PARSER_MAX:
                if (isdigit(c))
                {
                    img->max = img->max * 10 + digittoint(c);
                }
                else if (isspace(c))
                {
                    if (img->max > 255)
                    {
                        state = PARSER_ERR;
                    }
                    else
                    {
                        state = PARSER_MAX_SPACE;
                    }
                }
                else
                {
                    state = PARSER_ERR;
                }
                break;

            case PARSER_MAX_SPACE:
                if (isdigit(c) && pixel_i < img->w * img->h)
                {
                    state = PARSER_PIXEL;
                    img->data[pixel_i] = img->data[pixel_i] * 10 + digittoint(c);
                }
                else if (isspace(c));
                else
                {
                    state = PARSER_ERR;
                }
                break;

            case PARSER_PIXEL:
                if (isdigit(c) && pixel_i < img->w * img->h)
                {
                    img->data[pixel_i] = img->data[pixel_i] * 10 + digittoint(c);
                }
                else if (isspace(c))
                {
                    state = PARSER_MAX_SPACE;
                    pixel_i++;
                }
                else
                {
                    state = PARSER_ERR;
                }
                break;
        }
    }

    fclose(f);

    if (img->data && (img->w * img->h == pixel_i || img->w * img->h == pixel_i + 1) && state != PARSER_ERR)
    {
        return 0;
    }
    else
    {
        free(img->data);
        return -1;
    }
}

int pgm_save(const image_t *img, const char *path)
{
    if (!img || !path) return -1;

    FILE *f = fopen(path, "w");
    if (!f) return -1;

    fprintf(f, "P2\n%d %d\n%d\n", img->w, img->h, img->max);

    for (int i = 0; i < img->w * img->h; ++i)
    {
        fprintf(f, "%d\n", img->data[i]);
    }

    fclose(f);

    return 0;
}
