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
local Colors = require("Colors")
local nextNpcId = 1
local Commands = require("Commands")

Commands.register("pos", function(client)
    local pos = client:player():position()
    client:sendMessage(format("{}, {}", pos.x, pos.y), 255, Colors.YELLOW)
end)

Commands.register("tp", function(client, target)
    if client == target then
        client:sendMessage("You cannot teleport to yourself", 255, Colors.RED)
        return
    end
    client:sendMessage(format("Teleporting to {}", target:player():character().name), 255, Colors.YELLOW)
    client:player():teleport(target:player():position())
end, {"client"})

Commands.register("kickme", function(client, msg)
    msg = msg or "okay, you asked for it"
    client:disconnect(msg)
end)

Commands.register("echo", function(client, arg1, arg2)
    client:sendMessage(format("arg1 is '{}', arg2 is '{}'", arg1, arg2))
end)

Hooks.add("chat", function(event)
    if string.find(event.message, "/tp") == 1 and false then
        event.canceled = true
        local args = {}
        for token in string.gmatch(event.message, "[^%s]+") do
            table.insert(args, token)
        end

        if #args >= 3 then
            local x = tonumber(args[2])
            local y = tonumber(args[3])
            if not x or not y then
                event.client:sendMessage("Invalid position", 255, Colors.RED)
                return
            end
            event.client:sendMessage(format("Teleporting to {}, {}", x, y), 255, Colors.YELLOW)
            event.client:player():teleport({ x = x, y = y })
        elseif #args >= 2 then
            local targetClient
            for _, c in pairs(Game.clients()) do
                if string.find(c:player():character().name, args[2]) then
                    targetClient = c
                    break
                end
            end

            if not targetClient then
                event.client:sendMessage("Unknown player", 255, Colors.RED)
            elseif targetClient == event.client then
                event.client:sendMessage("You cannot teleport to yourself", 255, Colors.RED)
            else
                event.client:sendMessage(format("Teleporting to {}", targetClient:player():character().name), 255, Colors.YELLOW)
                event.client:player():teleport(targetClient:player():position())
            end
        else
            event.client:sendMessage("Not enough arguments", 255, Colors.RED)
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
    elseif string.find(event.message, "/i") == 1 then
        event.canceled = true
        local args = {}
        for token in string.gmatch(event.message, "[^%s]+") do
            table.insert(args, token)
        end

        if #args < 2 then
            return
        end
        local id = tonumber(args[2])
        local stack = 1
        if #args >= 3 then
            stack = tonumber(args[3]) or 1
        end

        event.client:player():inventory():setItem(49, { id = id, stack = stack })
        event.client:sendMessage(format("Given {} of {}", stack, id), 255, Colors.YELLOW)
    end
end)

Hooks.add("connectRequest", function(event)
    print(format("Client {} is connecting with version {}", event.client:id(), event.version))
    -- We can even disconnect a client while they are connecting.
    -- This involves causing an event to fire within an event, and making sure we don't destroy the client too soon.
    -- event.client:disconnect("will this break")
end)

Hooks.add("playerDeath", function(event)
    local text
    if event.reason.player then
        text = format("{} killed {}", Game.client(event.reason.player):player():character().name, event.target:player():character().name)
    else
        text = format("{} died.", event.target:player():character().name)
    end

    Utils.broadcast(text, Colors.SPRINGGREEN)
end)

Hooks.add("damageNpc", function(event)
    print(format("Hit NPC {} for {} damage", event.npcId, event.damage))
end)

Hooks.add("finishedConnecting", function(event)
    local name = event.client:player():character().name
    print(format("{}/{} finished connecting", name, event.client:address()))
    Utils.broadcast(format("{} finished connecting.", name), Colors.MAGENTA)
end)

Hooks.add("disconnect", function(event)
    local name = event.client:player():character().name
    if name == nil then
        return
    end
    Utils.broadcast(format("{} disconnected.", name), Colors.TEAL)
end)

Hooks.add("changeTeam", function(event)
    Utils.broadcast(format("{} is now on team {}", event.client:player():character().name, event.team))
end)

local modifyTileNagLastTime = {}
Hooks.add("modifyTile", function(event)
    local now = os.time()
    if event.modification.x >= 2090 and event.modification.x <= 2110 and event.modification.y >= 290 and event.modification.y <= 305 then
        event.canceled = true
        local lastNag = modifyTileNagLastTime[event.client:id()]
        if lastNag == nil or now > lastNag + 3 then
            event.client:sendMessage("You cannot modify that close to the spawn region", 255, Colors.RED)
            modifyTileNagLastTime[event.client:id()] = now
        end
    end
end)