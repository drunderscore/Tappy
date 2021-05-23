--[[
    Copyright (c) 2021, James Puleo <james@jame.xyz>

    SPDX-License-Identifier: GPL-3.0-only
   
    This file belongs to the Base Lua API of Tappy.
    You should *NEVER* modify this, or any files around it!
    You will suffer the consequences of me laughing at you
    if you do.
 --]]

print("Tappy Base initializing")

package.path = package.path .. ";Base/?.lua"

local Escapes = require("ANSIEscapes")
local Hooks = require("Hooks")

local Base = {}

function Base.onClientChat(client, message)
    local event = {}
    event.canceled = false
    event.client = client
    event.message = message
    Hooks.publish("chat", event)
    if event.canceled then
        return
    end
    print(Escapes.CYAN .. client:player():character().name .. "/" .. client:address() .. ": " .. event.message .. Escapes.RESET)
    for k, c in ipairs(Game.clients()) do
        c:sendMessage(event.message, client:id())
    end
end

function Base.onClientSyncProjectile(client, proj)
    local event = {}

    event.client = client
    -- The projectile being synced
    event.projectile = proj
    -- Sync this projectile to the original sender.
    event.syncToOwner = false
    -- Don't sync to other clients, and kill the projectile on the owner's side.
    event.canceled = false
    Hooks.publish("syncProjectile", event);

    if event.canceled then
        client:killProjectile(event.projectile.id, client:id())
    else
        local projId = Game.addProjectile(event.projectile, event.projectile.id)
        for i, c in pairs(Game.clients()) do
            if event.syncToOwner or c:id() ~= event.projectile.owner then
                c:syncProjectile(projId)
            end
        end
    end
end

function Base.onConnectRequest(client, version)
    local event = {}
    event.client = client
    event.version = version
    Hooks.publish("connectRequest", event)
end

function Base.onTogglePvp(client, pvp)
    local event = {}

    event.client = client
    event.pvp = pvp
    event.canceled = false
    Hooks.publish("togglePvp", event)

    if event.canceled then
        client:player():setPvp(client, not pvp, true)
    else
        client:player():setPvp(client, pvp)
    end
end

function Base.onPlayerHurt(client, target, reason, damage, direction, flags, cooldownCounter)
    local event = {}
    event.client = client
    event.target = target
    event.reason = reason
    event.damage = damage
    event.direction = direction
    event.flags = flags
    event.cooldownCounter = cooldownCounter
    Hooks.publish("playerHurt", event)
end

function Base.onPlayerDeath(client, target, reason, damage, direction, flags)
    local event = {}
    event.client = client
    event.target = target
    event.reason = reason
    event.damage = damage
    event.direction = direction
    event.flags = flags
    Hooks.publish("playerDeath", event)
end

function Base.onDamageNpc(client, npcId, damage, knockback, hitDirection, crit)
    local event = {}
    event.client = client
    event.npcId = npcId
    event.damage = damage
    event.knockback = knockback
    event.hitDirection = hitDirection
    event.crit = crit
    Hooks.publish("damageNpc", event)
end

function Base.onClientFinishConnecting(client)
    local event = {}
    event.client = client

    Hooks.publish("finishedConnecting", event)
end

function Base.onPlayerSpawn(client)
    local event = {}
    event.client = client

    Hooks.publish("playerSpawn", event)
end

return Base