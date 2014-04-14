
local ooe = os.execute

function os.execute(cli)
  print("(lua) + " .. cli)
  return ooe(cli)
end
