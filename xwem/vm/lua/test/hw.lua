#! /usr/bin/lua

_G.null = false

print("Hello, I'm XuanWuOS!\n")
mythdsp = xwos.cthd.sp()
print("Thread strong pointer:", mythdsp)
print("pi =", math.pi)
print("2^11 =", 2 ^ 11)
print("exp(10) =", math.exp(10))

t1 = {1, 2, null, 4, 5}
t1["b"] = "b"
print(string.format("t1 = %d", #t1))
print(string.format("t1[%d] = %s", 5, t1[5]))
t2 = {6, 7}
t2["a"] = "a"
t2["c"] = "c"

function tbl_add(t1, t2)
  local t = {}
  for k, v in pairs(t1) do
    if (type(k) == "number") then
      table.insert(t, v)
    else
      t[k] = v
    end
  end
  for k, v in pairs(t2) do
    if (type(k) == "number") then
      table.insert(t, v)
    else
      t[k] = v
    end
  end
  return t
end

tbl_metatbl = {
  ["__add"] = tbl_add
}

setmetatable(t2, tbl_metatbl)

t3 = t1 + t2

for k, v in pairs(t3) do
  print(k, v);
end
