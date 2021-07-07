--[[
    Copyright (c) 2021, James Puleo <james@jame.xyz>

    SPDX-License-Identifier: GPL-3.0-only
   
    This file belongs to the Base Lua API of Tappy.
    You should *NEVER* modify this, or any files around it!
    You will suffer the consequences of me laughing at you
    if you do.
 --]]

local Hooks = {}

local hookMap = {}
hookMap["chat"] = {}
hookMap["syncProjectile"] = {}
hookMap["connectRequest"] = {}
hookMap["togglePvp"] = {}
hookMap["playerHurt"] = {}
hookMap["playerDeath"] = {}
hookMap["damageNpc"] = {}
hookMap["playerSpawn"] = {}
hookMap["modifyTile"] = {}
hookMap["disconnect"] = {}
hookMap["changeTeam"] = {}

function Hooks.add(name, func)
    if type(name) ~= "string" then
        error("expected 'string' for argument #1, but got " .. type(name))
    end

    if type(func) ~= "function" then
        error("expected 'function' for argument #2, but got " .. type(func))
    end

    local hookFuncs = hookMap[name]
    if hookFuncs == nil then
        hookFuncs = {}
        hookMap[name] = hookFuncs
    end
    table.insert(hookFuncs, func)
end

function Hooks.remove(name, func)
    if type(name) ~= "string" then
        error("expected 'string' for argument #1, but got '" .. type(name) .. "'")
    end

    if type(func) ~= "function" then
        error("expected 'function' for argument #2, but got '" .. type(func) .. "'")
    end

    local hookFuncs = hookMap[name]
    if hookFuncs == nil then
        error("invalid hook " .. name)
    end

    table.remove(hookFuncs, func)
end

function Hooks.publish(name, ...)
    local canceled = false
    for k, f in pairs(hookMap[name]) do
        if f(...) then
            canceled = true
        end
    end

    return canceled
end

return Hooks
