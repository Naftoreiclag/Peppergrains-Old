print("Hello from Lua")
print("Lua version: ", _VERSION)

local output = ""
for elem in pairs(_G) do 
    output = output .. tostring(elem) .. "\t"
end
print(output)

return true
