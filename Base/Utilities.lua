--[[
    Copyright (c) 2021, James Puleo <james@jame.xyz>

    SPDX-License-Identifier: GPL-3.0-only

    This file belongs to the Base Lua API of Tappy.
    You should *NEVER* modify this, or any files around it!
    You will suffer the consequences of me laughing at you
    if you do.
 --]]

local Utilities = {}

function Utilities.broadcast(text, color)
    color = color or { r = 255, g = 255, b = 255 }
    for _, c in pairs(Game.clients()) do
        c:sendMessage(text, 255, color)
    end
end

function Utilities.findClients(partOfName)
    local found = {}
    for _, c in pairs(Game.clients()) do
        if string.find(c:player():character().name, partOfName) then
            table.insert(found, c)
        end
    end
    return found
end

return Utilities