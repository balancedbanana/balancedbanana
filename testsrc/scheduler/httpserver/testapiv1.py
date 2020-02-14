import requests
import yaml
import sys
import subprocess
import time
import os
sample = subprocess.Popen("./HttpTests");
time.sleep(1)
res = requests.get("http://localhost:8234/v1/jobs/hours/2365")
print(res.content)
result = yaml.safe_load(res.content)
assert result['user_name'] != '', "Username must not be empty"
sample.terminate()
sys.exit(0)