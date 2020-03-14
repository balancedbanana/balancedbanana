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
bbc = subprocess.Popen(["../../src/client/bbc", "--help"], stdin=subprocess.PIPE, env=my_env)
bbc.wait()
bbc = subprocess.Popen(["../../src/client/bbc", "run", "--help"], stdin=subprocess.PIPE, env=my_env)
bbc.wait()
# start server
bbs = subprocess.Popen("../../src/scheduler/bbs", stdin=subprocess.PIPE, env=my_env)
time.sleep(1)
# start worker
bbd = subprocess.Popen("../../src/worker/bbd", stdin=subprocess.PIPE, env=my_env)
time.sleep(1)
try:
    #start client
    bbc = subprocess.Popen(["../../src/client/bbc", "run", "--image=centos", "--job", "echo Hi"], stdin=subprocess.PIPE, env=my_env)
    time.sleep(1)
    #auth client agains server
    bbc.communicate(input=b'thePassword\n')
    bbc.wait()
    bbc = subprocess.Popen(["sshpass", "../../src/client/bbc", "run", "--image=centos", "--job", "echo Hi"], stdin=subprocess.PIPE, env=my_env)
    time.sleep(1)
    #auth client agains server
    bbc.communicate(input=b'thePassword\n')
    bbc.wait()
    time.sleep(1)
    res = requests.get("http://localhost:8202/v1/jobs/2365")
    result = yaml.safe_load(res.content)
    assert result['user_name'] != '', "Username must not be empty"
    res = requests.get("http://localhost:8202/v1/jobs/hours/3")
    result = yaml.safe_load(res.content)
    for job in result['jobs']:
        print(job['job_id'])
    res = requests.get("http://localhost:8202/v1/workmachines/0/jobs")
    result = yaml.safe_load(res.content)
    for job in result['jobs']:
        print(job['job_id'])
    res = requests.get("http://localhost:8202/v1/user/330/jobs")
    result = yaml.safe_load(res.content)
    for job in result['jobs']:
        print(job['job_id'])
    res = requests.get("http://localhost:8202/v1/user//jobs")
    res = requests.get("http://localhost:8202/v1/workmachines/workload")
    result = yaml.safe_load(res.content)
    bbd.communicate(input=b'stop\n')
    bbd.wait()
    bbd = subprocess.Popen("../../src/worker/bbd", stdin=subprocess.PIPE, env=my_env)
    time.sleep(5)
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