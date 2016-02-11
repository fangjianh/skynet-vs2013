package.cpath = package.cpath .. ";../../luaclib/?.so"

local ltask = require "ltask"

ltask.init(4)
ltask.task("hello.lua", "Hello", "world")
local c = ltask.channel()
ltask.task("producer.lua",c)
ltask.task("consumer.lua",c)
ltask.run()