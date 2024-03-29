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
    if hookMap[name] == nil then
        hookMap[name] = {}
        return
    end
    for k, f in pairs(hookMap[name]) do
        f(...)
    end
end

return Hooks
