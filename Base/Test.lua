--[[
    Copyright (c) 2021, James Puleo <james@jame.xyz>

    SPDX-License-Identifier: GPL-3.0-only

    This file belongs to the Base Lua API of Tappy.
    You should *NEVER* modify this, or any files around it!
    You will suffer the consequences of me laughing at you
    if you do.
 --]]

local Hooks = require("Hooks") -- This let's us know when certain actions happen in-game and hook them

Hooks.add("chat", function(event)
    if (string.find(event.message, "/tp") == 1) then
        event.canceled = true
        local targetClient
        for i, c in pairs(Game.clients()) do
            local partialName = string.sub(event.message, 5)
            if string.find(c:player():character():name(), partialName) then
                targetClient = c
                break
            end
        end

        if not targetClient then
            event.client:sendMessage("Unknown player", 255, { r = 255, g = 0, b = 0 })
        elseif targetClient == event.client then
            event.client:sendMessage("You cannot teleport to yourself", 255, { r = 255, g = 0, b = 0 })
        else
            event.client:sendMessage("Teleporting to " .. targetClient:player():character():name(), 255, { r = 0x55, g = 0xad, b = 0x12 })
            event.client:teleport(targetClient:player():position())
        end
    end
end)

Hooks.add("connectRequest", function(event)
    print("Client " .. event.client:id() .. " is connecting with version " .. tostring(event.version))
    -- FIXME: Yeah this doesn't work... we send the disconnect packet, destroy the client, and then try to return the
    -- client packet handler, which is inside the now destroyed client.
    -- event.client:disconnect("will this break")
end)