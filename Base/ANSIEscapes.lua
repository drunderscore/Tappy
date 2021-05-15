--[[
    Copyright (c) 2021, James Puleo <james@jame.xyz>

    SPDX-License-Identifier: GPL-3.0-only

    This file belongs to the Base Lua API of Tappy.
    You should *NEVER* modify this, or any files around it!
    You will suffer the consequences of me laughing at you
    if you do.
 --]]

local Escapes = {}
Escapes.BLACK = "\u{001b}[30m"
Escapes.RED = "\u{001b}[31m"
Escapes.GREEN = "\u{001b}[32m"
Escapes.YELLOW = "\u{001b}[33m"
Escapes.BLUE = "\u{001b}[34m"
Escapes.MAGENTA = "\u{001b}[35m"
Escapes.CYAN = "\u{001b}[36m"
Escapes.WHITE = "\u{001b}[37m"
Escapes.RESET = "\u{001b}[0m"

return Escapes
