
-- this should really be built-in to lua
function dofile(filename, ...)
	return assert(loadfile(filename))(...)
end
