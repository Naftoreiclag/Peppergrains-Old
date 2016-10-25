print("Hello from Lua")
print("Lua version: ", _VERSION)

for available in pairs(_ENV) do
    print(tostring(available))
    --[[
    if type(available) == "table" do
        for method in pairs(available) do
            print(tostring(available) .. "." .. tostring(method))
        end
    else
        print(tostring(available))
    end
    ]]--
end

return true
