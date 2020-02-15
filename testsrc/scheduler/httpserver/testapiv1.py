import requests
import yaml
import sys
import subprocess
import time
import os
sample = subprocess.Popen("./HttpTests", stdin=subprocess.PIPE);
try:
    time.sleep(1)
    res = requests.get("http://localhost:8234/v1/jobs/2365")
    result = yaml.safe_load(res.content)
    assert result['user_name'] != '', "Username must not be empty"
    res = requests.get("http://localhost:8234/v1/jobs/hours/3")
    result = yaml.safe_load(res.content)
    for job in result['jobs']:
        print(job['job_id'])
except:
    sample.communicate(input=b'stop\n')
    sample.wait()
    raise
else:
    sample.communicate(input=b'stop\n')
    sample.wait()
    sys.exit(0)