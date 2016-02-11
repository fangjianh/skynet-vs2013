package.cpath = package.cpath .. ";../../luaclib/?.so"

local ltask = require "ltask"
local c = ...
assert(type(c) == "number")
local taskid = ltask.taskid()

local function recv()
	while true do
		if ltask.select(c) then
			local ok, value = ltask.recv(c)
			if ok then
				print(string.format("task %d recv %d", taskid, value))
				return
			end
		end
		coroutine.yield()
	end
end

for i=1,10 do
	recv()
end

print("consumer exit")
