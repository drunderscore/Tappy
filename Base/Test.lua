--[[
    Copyright (c) 2021, James Puleo <james@jame.xyz>

    SPDX-License-Identifier: GPL-3.0-only

    This file belongs to the Base Lua API of Tappy.
    You should *NEVER* modify this, or any files around it!
    You will suffer the consequences of me laughing at you
    if you do.
 --]]

local Hooks = require("Hooks") -- This let's us know when certain actions happen in-game and hook them
local Utils = require("Utilities")
local nextNpcId = 1

Hooks.add("chat", function(event)
    if string.find(event.message, "/tp") == 1 then
        event.canceled = true
        local targetClient
        for i, c in pairs(Game.clients()) do
            local partialName = string.sub(event.message, 5)
            if string.find(c:player():character().name, partialName) then
                targetClient = c
                break
            end
        end

        if not targetClient then
            event.client:sendMessage("Unknown player", 255, { r = 255, g = 0, b = 0 })
        elseif targetClient == event.client then
            event.client:sendMessage("You cannot teleport to yourself", 255, { r = 255, g = 0, b = 0 })
        else
            event.client:sendMessage("Teleporting to " .. targetClient:player():character().name, 255, { r = 0x55, g = 0xad, b = 0x12 })
            event.client:teleport(targetClient:player():position())
        end
    elseif string.find(event.message, "/npc") == 1 then
        event.canceled = true
        local npc = {}
        npc.id = nextNpcId
        npc.position = event.client:player():position()
        npc.velocity = { x = 0, y = 0 }
        npc.target = 0
        npc.hp = 1337
        npc.type = tonumber(string.sub(event.message, 6)) or 3
        npc.direction = false
        npc.directionY = false
        npc.ai = {}
        npc.spriteDirection = false
        npc.spawnedFromStatue = false

        for _, c in pairs(Game.clients()) do
            c:syncNpc(npc)
        end

        nextNpcId = nextNpcId + 1
    elseif string.find(event.message, "/getpos") == 1 then
        event.canceled = true
        local pos = event.client:player():position()
        event.client:sendMessage("(" .. pos.x .. ", " .. pos.y .. ")")
    end
end)

Hooks.add("connectRequest", function(event)
    print("Client " .. event.client:id() .. " is connecting with version " .. tostring(event.version))
    -- FIXME: Yeah this doesn't work... we send the disconnect packet, destroy the client, and then try to return the
    -- client packet handler, which is inside the now destroyed client.
    -- event.client:disconnect("will this break")
end)

Hooks.add("playerDeath", function(event)
    local text
    if event.reason.player then
        text = Game.client(event.reason.player):player():character().name .. " killed " .. event.target:player():character().name
    else
        text = event.target:player():character().name .. " died."
    end

    Utils.broadcast(text, { r = 255, g = 255, b = 10 })
end)

Hooks.add("damageNpc", function(event)
    print("Hit NPC " .. event.npcId .. " for " .. event.damage .. " damage")
end)

Hooks.add("finishedConnecting", function(event)
    local thePink = { r = 255, g = 0, b = 255 }
    local name = event.client:player():character().name
    print(name .. "/" .. event.client:address() .. " finished connecting")
    Utils.broadcast(name .. " finished connecting.", thePink)
end)