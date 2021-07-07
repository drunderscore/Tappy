--[[
    Copyright (c) 2021, James Puleo <james@jame.xyz>

    SPDX-License-Identifier: GPL-3.0-only

    This file belongs to the Base Lua API of Tappy.
    You should *NEVER* modify this, or any files around it!
    You will suffer the consequences of me laughing at you
    if you do.
 --]]

local Library = {}

local commands = {}

local Hooks = require("Hooks")
local Colors = require("Colors")
local Utils = require("Utilities")
local Escapes = require("ANSIEscapes")

-- TOOD: Isn't this slow as hell?
local function parseCommand(str)
    local cmd
    local args = {}
    local hasQuoteOpen = false
    local buffer = ""

    for c in string.gmatch(str, ".") do
        if c == " " then
            if not hasQuoteOpen then
                if cmd == nil then
                    cmd = buffer
                else
                    table.insert(args, buffer)
                end
                buffer = ""
            else
                buffer = buffer .. " "
            end
        elseif c == "\"" then
            hasQuoteOpen = not hasQuoteOpen
        else
            buffer = buffer .. c
        end
    end

    if cmd == nil then
        cmd = buffer
    else
        table.insert(args, buffer)
    end

    return cmd, args
end

function Library.register(name, fn, expectedTypes)
    if type(name) ~= "string" then
        error(format("expected 'string' for argument #1, but got '{}'", type(name)))
    end

    if type(fn) ~= "function" then
        error(format("expected 'function' for argument #2, but got '{}'", type(fn)))
    end

    if expectedTypes ~= nil and type(expectedTypes) ~= "table" then
        error(format("expected 'table' for argument #3, but got '{}'", type(expectedTypes)))
    end

    if commands[name] ~= nil then
        error(format("a command with the name {} already exists", name))
    end

    commands[name] = {fn = fn, expectedTypes = expectedTypes}
end

Hooks.add("chat", function(event)
    -- TODO: Customizable specifier?
    if string.sub(event.message, 1, 1) ~= "/" then
        return
    end

    event.canceled = true

    local realCommandStr = string.sub(event.message, 2)
    local name, args = parseCommand(realCommandStr)
    local cmd = commands[name]

    if cmd == nil then
        event.client:sendMessage("Unknown command.", 255, Colors.RED)
        return
    end

    if cmd.expectedTypes ~= nil then
        if #args < #cmd.expectedTypes then
            event.client:sendMessage(format("Expected at least {} argument(s), but got {}", #cmd.expectedTypes, #args), 255, Colors.RED)
            return
        end
        for i, v in pairs(cmd.expectedTypes) do
            if v == "number" then
                local value = tonumber(args[i])
                if value == nil then
                    event.client:sendMessage(format("Expected 'number' for argument #{}", i), 255, Colors.RED)
                    return
                end

                args[i] = value
            elseif v == "client" then
                local foundPlayers = Utils.findClients(args[i])
                if #foundPlayers ~= 1 then
                    -- FIXME: Would be nice if the sender knew if they had no players or too many players found
                    event.client:sendMessage(format("Expected client for argument #{}", i), 255, Colors.RED)
                    return
                end

                args[i] = foundPlayers[1]
            elseif v == "clients" then
                local foundPlayers = Utils.findClients(args[i])

                if #foundPlayers == 0 then
                    event.client:sendMessage(format("Expected client(s) for argument #{}", i), 255, Colors.RED)
                    return
                end

                args[i] = foundPlayers
            end
        end
    end

    local success, error = pcall(cmd.fn, event.client, table.unpack(args))

    if not success then
        event.client:sendMessage("Error occurred whilst executing that command. Check the console for more details.", 255, Colors.RED)
        print(format("{}Client {} caused error executing '{}': {}{}", Escapes.RED, event.client:id(), event.message, error, Escapes.RESET))
        return
    end
end)

return Library