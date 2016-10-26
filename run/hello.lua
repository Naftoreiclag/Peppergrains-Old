print("Hello from Lua")
print("Lua version: ", _VERSION)

local mod = require "world"

for k, v in pairs(_ENV) do
    if type(v) == "table" then
        for method in pairs(v) do
            print(tostring(k) .. "." .. tostring(method))
        end
    else
        print(tostring(k))
    end
end

return true
