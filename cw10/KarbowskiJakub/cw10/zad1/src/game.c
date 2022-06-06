#include "game.h"

#include <stdio.h>

static void update_game_winner(game_t *game)
{
    bool is_draw = true;
    for (int i = 0; i < 9; ++i)
    {
        if (game->board[i] == FIELD_EMPTY)
        {
            is_draw = false;
            break;
        }
    }

    if (is_draw)
    {
        game->winner = WINNER_DRAW;
        game->is_over = true;
        return;
    }

    winner_t winner = WINNER_DRAW;

    do
    {
        // XXX
        // ---
        // ---
        if ((game->board[POS_NW] != FIELD_EMPTY) &&
            (game->board[POS_NW] == game->board[POS_N]) &&
            (game->board[POS_NW] == game->board[POS_NE]))
        {
            winner = (winner_t) game->board[POS_NW];
            break;
        }

        // ---
        // XXX
        // ---
        if ((game->board[POS_W] != FIELD_EMPTY) &&
            (game->board[POS_W] == game->board[POS_C]) &&
            (game->board[POS_W] == game->board[POS_E]))
        {
            winner = (winner_t) game->board[POS_W];
            break;
        }

        // ---
        // ---
        // XXX
        if ((game->board[POS_SW] != FIELD_EMPTY) &&
            (game->board[POS_SW] == game->board[POS_S]) &&
            (game->board[POS_SW] == game->board[POS_SE]))
        {
            winner = (winner_t) game->board[POS_SW];
            break;
        }




        // X - -
        // X - -
        // X - -
        if ((game->board[POS_NW] != FIELD_EMPTY) &&
            (game->board[POS_NW] == game->board[POS_W]) &&
            (game->board[POS_NW] == game->board[POS_SW]))
        {
            winner = (winner_t) game->board[POS_NW];
            break;
        }

        // - X -
        // - X -
        // - X -
        if ((game->board[POS_N] != FIELD_EMPTY) &&
            (game->board[POS_N] == game->board[POS_C]) &&
            (game->board[POS_N] == game->board[POS_S]))
        {
            winner = (winner_t) game->board[POS_N];
            break;
        }

        // - - X
        // - - X
        // - - X
        if ((game->board[POS_NE] != FIELD_EMPTY) &&
            (game->board[POS_NE] == game->board[POS_E]) &&
            (game->board[POS_NE] == game->board[POS_SE]))
        {
            winner = (winner_t) game->board[POS_NE];
            break;
        }




        // X - -
        // - X -
        // - - X
        if ((game->board[POS_NW] != FIELD_EMPTY) &&
            (game->board[POS_NW] == game->board[POS_C]) &&
            (game->board[POS_NW] == game->board[POS_SE]))
        {
            winner = (winner_t) game->board[POS_NW];
            break;
        }

        // - - X
        // - X -
        // X - -
        if ((game->board[POS_NE] != FIELD_EMPTY) &&
            (game->board[POS_NE] == game->board[POS_C]) &&
            (game->board[POS_NE] == game->board[POS_SW]))
        {
            winner = (winner_t) game->board[POS_NE];
            break;
        }
    } while (0);

    if (winner != WINNER_DRAW)
    {
        game->winner = winner;
        game->is_over = true;
    }
}

void game_init(game_t *game)
{
    for (int i = 0; i < 9; ++i)
        game->board[i] = FIELD_EMPTY;

    game->is_over = false;

    game->next_player = PLAYER_X;
}

err_t game_move(game_t *game, pos_t pos)
{
    if (!game) return ERR_GENERIC;

    if (game->is_over) return ERR_GENERIC;

    if (game->board[pos] != FIELD_EMPTY) return ERR_GENERIC;

    game->board[pos] = (field_t) game->next_player;

    game->next_player = game->next_player == PLAYER_X ? PLAYER_O : PLAYER_X;

    update_game_winner(game);

    return ERR_OK;
}

void board_print(const board_t board)
{
    char c[9];
    for (int i = 0; i < 9; ++i)
    {
        switch (board[i])
        {
            case FIELD_EMPTY:
                c[i] = ' ';
                break;

            case FIELD_X:
                c[i] = 'X';
                break;

            case FIELD_O:
                c[i] = 'O';
                break;
        }
    }
    printf(
        "+ - + - + - +\n"
        "| %c | %c | %c |\n"
        "+ - + - + - +\n"
        "| %c | %c | %c |\n"
        "+ - + - + - +\n"
        "| %c | %c | %c |\n"
        "+ - + - + - +\n",
        c[0], c[1], c[2],
        c[3], c[4], c[5],
        c[6], c[7], c[8]
    );
}

void game_print(const game_t *game)
{
    board_print(game->board);
    if (game->is_over)
    {
        switch (game->winner)
        {
            case WINNER_DRAW:
                printf("Draw!\n");
                break;

            case WINNER_X:
                printf("X wins!\n");
                break;

            case WINNER_O:
                printf("O wins!\n");
                break;
        }
    }
    else
    {
        printf("It is %c's turn\n", game->next_player == PLAYER_X ? 'X' : 'O');
    }
}
