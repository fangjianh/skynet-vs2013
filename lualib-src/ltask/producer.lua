package.cpath = package.cpath .. ";../../luaclib/?.so"

local ltask = require "ltask"
local c = ...
assert(type(c) == "number")
local taskid = ltask.taskid()

for i=1,10 do
	ltask.send(c, i)
	print(string.format("task %d send %d", taskid, i))
	coroutine.yield()
end

print("producer exit")
