#!/usr/bin/python

import cgi, cgitb 
import os
import time
from lockfile import LockFile

r2t2file = '../r2t2reg/r2t2.txt'
r2t2lock = '../r2t2reg/r2t2.lock'
timeout = 300

form = cgi.FieldStorage() 

# get post event
call  = form.getvalue('call')
location = form.getvalue('location')
band =  form.getvalue('band')
rig =  form.getvalue('rig')
ant =  form.getvalue('ant')
status = form.getvalue('status')
version = form.getvalue('version')
gpsloc =  form.getvalue('gpsloc')
port = 8008

ip = os.environ["REMOTE_ADDR"]

if call == None:
    print ("Content-type: text/html")
    print ()
    print ("ok")
    quit() 

lock = LockFile(r2t2lock)
while not lock.i_am_locking():
    try:
        lock.acquire(timeout=5)    # wait up to 5 seconds
    except:
        quit()
        #lock.break_lock()
        #lock.acquire()

#formatstr = 'TIME:{}\nCALL:{}\nIP:{}\nLOCATION:{}\nBAND:{}\nRIG:{}\nANT:{}\n\nstatus:{}\n'
#s = formatstr.format(time.strftime("%Y-%m-%d %H:%M:%S"),call, ip, location, band, rig, ant, status)
formatstr = '{}~{}~{}~{}~{}~{}~{}~{}~{}~N~\n'
s = formatstr.format(status, call, location, band, rig, ant, time.strftime("%Y-%m-%d %H:%M:%S"),ip,port)

# append to file
f = open(r2t2file,'a')
f.write(s)
f.close()

# read file
with open(r2t2file) as f:
    lines = f.readlines()

# sort and remove old entries
r2t2devices = {}
curtime = time.time()
for line in lines:
    f=line.split('~')
    date = time.strptime(f[6], '%Y-%m-%d %H:%M:%S')	
    if time.mktime(date) + timeout > curtime and time.mktime(date) <= curtime:
        if f[1] in r2t2devices:
            date1 = time.strptime(r2t2devices[f[1]][6], '%Y-%m-%d %H:%M:%S')	
            if time.mktime(date) > time.mktime(date1):
                r2t2devices[f[1]] = f
        else:
            r2t2devices[f[1]] = f

# write back
f = open(r2t2file,'w')
for key in r2t2devices:
    s = formatstr.format(*r2t2devices[key])
    f.write(s)
f.close()

lock.release()

print ("Content-type: text/html")
print ()
print ("ok")
