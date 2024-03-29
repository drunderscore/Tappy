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
local ID = require("ID")

local Base = {}
-- FIXME: If a plugin wants to remove an item that has a pickup timer, bad things will happen.
-- Expose a helper for this?
local droppedItemPickupTimers = {}

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
        client:player():setPvp(not pvp, true)
    else
        client:player():setPvp(pvp, false)
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

function Base.onModifyTile(client, modification)
    local event = {}
    event.client = client
    event.canceled = false
    --- If you've modified the modification in any way, you'll want to sync it back to whoever sent it.
    event.syncToSender = false
    event.modification = modification;

    -- FIXME: This should generalized (ex, placeBlock, removeBlock, placeWall, removeWall, etc)
    Hooks.publish("modifyTile", event)

    if event.canceled then
        client:syncTileRect(modification.x, modification.y, 3, 3)
    else
        client:modifyTile(modification)

        if event.syncToSender then
            client:syncTileRect(modification.x, modification.y, 3, 3)
        end

        local tileDropEvent = {}
        tileDropEvent.client = client
        tileDropEvent.modification = modification

        if modification.action == 6 or modification.action == 11 or modification.action == 13 or modification.action == 17 then
            tileDropEvent.drops = {{id = ID.Item.Wire, stack = 1}}
        elseif modification.action == 9 then
            tileDropEvent.drops = {{id = ID.Item.Actuator, stack = 1}}
        else
            tileDropEvent.drops = {}
        end

        Hooks.publish("tileDrop", tileDropEvent)

        for _, item in pairs(tileDropEvent.drops) do
            Game.addDroppedItem({
                item = item,
                position = {x = modification.x * 16, y = modification.y * 16},
                velocity = {x = 0, y = 0},
                owner = event.client:id()
            })
        end
    end
end

function Base.onClientDisconnect(client, reason)
    local event = {}
    event.client = client
    event.reason = reason

    Hooks.publish("disconnect", event)
end

function Base.onClientSyncPlayerTeam(client, team)
    local event = {}
    event.client = client
    event.team = team
    event.syncToSender = false

    Hooks.publish("changeTeam", event)

    client:player():setTeam(event.team, event.syncToSender)
end

function Base.onClientSyncItem(client, item, hasPickupDelay, id)
    if id ~= 400 and item.item.id == ID.Item.None then
        local event = {}
        event.client = client
        event.item = item
        Hooks.publish("removeItem", event)
        Game.removeDroppedItem(id)
        if droppedItemPickupTimers[id] ~= nil then
            droppedItemPickupTimers[id]:destroy()
            droppedItemPickupTimers[id] = nil
        end
    else
        local event = {}
        event.canceled = false
        event.client = client
        event.item = item
        -- The client only tells us if an item has pickup delay. If it does, they mean this constant value.
        event.pickupDelay = hasPickupDelay and math.floor((100 / 60) * 1000) or 0
        if id ~= 400 then
            event.id = id
        end

        Hooks.publish("syncItem", event)
        if event.canceled then
            return
        end

        local properId = id == 400 and Game.nextAvailableDroppedItemId() or id
        if event.pickupDelay == 0 then
            item.owner = client:id()
        end
        Game.addDroppedItem(item, properId)

        -- Only do this to new items
        if event.pickupDelay > 0 then
            droppedItemPickupTimers[properId] = Timer.create(function()
                Game.setItemOwner(properId, item.owner or client:id())
                droppedItemPickupTimers[properId] = nil
                return true
            end, event.pickupDelay)
        end
    end
end

function Base.onClientSyncItemOwner(client, id, owner)
    local event = {}
    event.client = client
    event.id = id
    event.owner = owner

    Hooks.publish("syncItemOwner", event)
end

return Base
