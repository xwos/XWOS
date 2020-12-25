#! /usr/bin/lua

_G.us = 1000
_G.ms = 1000000
_G.s = 1000000000

print("XWOS SKD TEST!\n")
mythdsp = xwos.cthd.sp()
print("Thread strong pointer:", mythdsp)
tt = xwos.skd.tt()
print(string.format("Current timetick:%f ms", tt / ms))
print("sleep 1s...\n")
xwos.cthd.sleep(1 * s)
tt = xwos.skd.tt()
print(string.format("Sleep 1s from %f ms\n", tt / ms))
xwos.cthd.sleep_from(tt, 1 * s)
tt = xwos.skd.tt()
print(string.format("Current Timetick:%f ms", tt /ms))
