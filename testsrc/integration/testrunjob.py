import requests
import yaml
import sys
import subprocess
import time
import os
import inspect
my_env = os.environ.copy()
my_env["HOME"] = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
my_env["USER"] = "steve"
# Test bbc help
bbc = subprocess.Popen(["../../src/client/bbc", "--help"], env=my_env)
bbc.wait()
assert bbc.returncode == 0, "bbc Help must exit successful with code 0"
bbc = subprocess.Popen(["../../src/client/bbc", "run", "--help"], env=my_env)
bbc.wait()
assert bbc.returncode == 0, "bbc run Help must exit successful with code 0"
# Schedule a job without a reachable server
bbc = subprocess.Popen(["../../src/client/bbc", "run", "--image=centos", "--job", "echo Hi"], env=my_env)
bbc.wait()
assert bbc.returncode == 255, "bbc must fails to connect with code 255"
# Try Connecting with invalid hostname, localhost is the valid default
bbc = subprocess.Popen(["../../src/client/bbc", "run", "--server", "localhos", "--port", "8000", "--image=centos", "--job", "echo Hi"], env=my_env)
bbc.wait()
assert bbc.returncode == 255, "bbc must fails to connect with code 255"
# bbd Help
bbd = subprocess.Popen(["../../src/worker/bbd", "--help"], env=my_env)
bbd.wait()
assert bbd.returncode == 0, "bbd Help must exit successful with code 0"
# Test fail to find with overrides
bbd = subprocess.Popen(["../../src/worker/bbd", "-s", "localhos", "-S", "8000"], env=my_env)
bbd.wait()
assert bbd.returncode == 255, "bbd must fails to connect with code 255"
# bbs Help
bbs = subprocess.Popen(["../../src/scheduler/bbs", "--help"], env=my_env)
bbs.wait()
assert bbs.returncode == 0, "bbs Help must exit successful with code 0"
# Try starting the server with invalid hostnames, fallback to accept any requests
bbs = subprocess.Popen(["../../src/scheduler/bbs", "-s", "localhos", "-S", "8000", "-W", "9000", "-w", "ocalhost"], stdin=subprocess.PIPE, env=my_env)
bbs.communicate(input=b'stop\n')
bbs.wait()
assert bbs.returncode == 0, "Trying to start and stop the Scheduler must exit successful with code 0"
bbd = subprocess.Popen("../../src/worker/bbd", env=my_env)
bbd.wait()
assert bbd.returncode == 255, "bbd must fails to connect with code 255"
# start server
bbs = subprocess.Popen("../../src/scheduler/bbs", stdin=subprocess.PIPE, env=my_env)
time.sleep(1)
assert bbs.poll() == None, "The Scheduler must be running"
try:
    #test for failed auth
    bbc = subprocess.Popen(["../../src/client/bbc", "run", "--image=centos", "--job", "echo Hi"], stdin=subprocess.PIPE, env=my_env)
    time.sleep(1)
    assert bbc.poll() == None, "The Client must be running"
    # try auth client agains server, with a wrong password
    bbc.communicate(input=b'theP3assword2\n')
    bbc.wait()
    assert bbc.returncode == 255, "The Client should fail during Authentification"
    #start client, image centos is preloaded by the travis script
    bbc = subprocess.Popen(["../../src/client/bbc", "run", "--image=centos", "--job", "echo Hi"], stdin=subprocess.PIPE, env=my_env)
    time.sleep(1)
    assert bbc.poll() == None, "The Client must be running"
    #auth client agains server, with correct password (set inside .travis script)
    bbc.communicate(input=b'thePassword\n')
    bbc.wait()
    assert bbc.returncode == 0, "Scheduling the Job must be successful"
    bbc = subprocess.Popen(["../../src/client/bbc", "run", "--image=:(", "--job", "echo Bye"], stdin=subprocess.PIPE, env=my_env)
    bbc.wait()
    assert bbc.returncode == 0, "Scheduling the Job must be successful, but fails to run image :( never exits"
    assert bbs.poll() == None, "The Scheduler must be running"
    # restart scheduler
    bbs.communicate(input=b'stop\n')
    bbs.wait()
    assert bbs.returncode == 0, "stopping the Scheduler must be successful"
    bbs = subprocess.Popen("../../src/scheduler/bbs", stdin=subprocess.PIPE, env=my_env)
    time.sleep(1)
    assert bbs.poll() == None, "The Scheduler must be running"
    # start worker
    bbd = subprocess.Popen("../../src/worker/bbd", stdin=subprocess.PIPE, env=my_env)
    time.sleep(1)
    assert bbs.poll() == None, "The Scheduler must be running"
    assert bbd.poll() == None, "The Worker must be running"
    bbc = subprocess.Popen(["../../src/client/bbc", "run", "--image=centos", "--job", "echo Hi"], env=my_env)
    bbc.wait()
    assert bbc.returncode == 0, "Scheduling another Job must be successful without password, after restarting bbs"
    time.sleep(1)
    res = requests.get("http://localhost:8202/v1/jobs/1")
    # result = yaml.safe_load(res.content)
    # assert result['user_name'] != my_env["USER"], "Username must be steve"
    res = requests.get("http://localhost:8202/v1/jobs/hours/3")
    # result = yaml.safe_load(res.content)
    # for job in result['jobs']:
    #     print(job['job_id'])
    res = requests.get("http://localhost:8202/v1/workmachines/1/jobs")
    # result = yaml.safe_load(res.content)
    # for job in result['jobs']:
    #     print(job['job_id'])
    res = requests.get("http://localhost:8202/v1/user/330/jobs")
    # result = yaml.safe_load(res.content)
    # for job in result['jobs']:
    #     print(job['job_id'])
    res = requests.get("http://localhost:8202/v1/user//jobs")
    res = requests.get("http://localhost:8202/v1/workmachines/workload")
    result = yaml.safe_load(res.content)
    bbd.communicate(input=b'stop\n')
    bbd.wait()
    assert bbs.returncode == 0, "stopping the Worker must be successful"
    bbd = subprocess.Popen("../../src/worker/bbd", stdin=subprocess.PIPE, env=my_env)
    time.sleep(1)
    assert bbd.poll() == None, "The Worker must be running"
    assert bbs.poll() == None, "The Scheduler must be running"
    time.sleep(4)
    assert bbd.poll() == None, "The Worker must be running"
    assert bbs.poll() == None, "The Scheduler must be running"
except:
    bbs.communicate(input=b'stop\n')
    bbs.wait()
    bbd.communicate(input=b'stop\n')
    bbd.wait()
    raise
else:
    bbd.communicate(input=b'stop\n')
    bbd.wait()
    bbs.communicate(input=b'stop\n')
    bbs.wait()
    sys.exit(0)