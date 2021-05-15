--[[
    Copyright (c) 2021, James Puleo <james@jame.xyz>

    SPDX-License-Identifier: GPL-3.0-only

    This file belongs to the Base Lua API of Tappy.
    You should *NEVER* modify this, or any files around it!
    You will suffer the consequences of me laughing at you
    if you do.
 --]]

local Hooks = require("Hooks") -- This let's us know when certain actions happen in-game and hook them

math.randomseed(os.time())

Hooks.add("syncProjectile", function(event)
    if event.projectile.type == 953 then
        event.client:disconnect("lmao no")
    end
end)

Hooks.add("connectRequest", function(event)
    print("Client " .. event.client:id() .. " is connecting with version " .. event.version)
    -- FIXME: Yeah this doesn't work... we send the disconnect packet, destroy the client, and then try to return the
    -- client packet handler, which is inside the now destroyed client.
    -- event.client:disconnect("will this break")
end)
