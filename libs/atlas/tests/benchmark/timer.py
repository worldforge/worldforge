from time import time

def time_on():
    global start_time
    start_time = time()

def time_off(msg, maxi):
    cpu_time = time() - start_time
    print "%s: iterations=%s  CPU Time=%s  iter/s=%s" % (
        msg, maxi, cpu_time, maxi/cpu_time)
