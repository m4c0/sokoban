export module sokoban:levels;

static constexpr const char *level_1 = "                        "
                                       "           XXXXX        "
                                       "         XXX...X        "
                                       "         X*PO..X        "
                                       "         XXX.O*X        "
                                       "         X*XXO.X        "
                                       "         X.X.*.XX       "
                                       "         XO.0OO*X       "
                                       "         X...*..X       "
                                       "         XXXXXXXX       "
                                       "                        "
                                       "                        ";
static constexpr const char *level_2 = "     XXXXX              "
                                       "     X...X              "
                                       "     XO..X              "
                                       "   XXX..OXXX            "
                                       "   X..O..O.X            "
                                       " XXX.X.XXX.X     XXXXXXX"
                                       " X...X.XXX.XXXXXXX...**X"
                                       " X.O..O..............**X"
                                       " XXXXX.XXXX.XPXXXX...**X"
                                       "     X......XXX  XXXXXXX"
                                       "     XXXXXXXX           "
                                       "                        ";
static constexpr const char *level_3 = "                        "
                                       "    XXXXXXXXXXXX        "
                                       "    X**..X.....XXX      "
                                       "    X**..X.O..O..X      "
                                       "    X**..XOXXXX..X      "
                                       "    X**....P.XX..X      "
                                       "    X**..X.X..O.XX      "
                                       "    XXXXXX.XXO.O.X      "
                                       "      X.O..O.O.O.X      "
                                       "      X....X.....X      "
                                       "      XXXXXXXXXXXX      "
                                       "                        ";

static constexpr const auto max_levels = 3;
static constexpr const char *levels[max_levels] = {level_1, level_2, level_3};
