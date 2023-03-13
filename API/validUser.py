import requests
import json

def isValidUser(handle):
  contestRanks = requests.get('https://codeforces.com/api/user.status?handle=' + handle + '&from=1&count=1')
  #contestRanks = requests.get('https://codeforces.com/api/user.status?handle='+handle)
  sj = json.loads(contestRanks.content)
  if(sj["status"]=="FAILED"):
    return 0
  else:
    return 1
