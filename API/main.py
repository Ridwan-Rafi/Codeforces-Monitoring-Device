import requests
import json
import datetime
from datetime import  date
def isValid(creationTime):
  dateTime = str(datetime.datetime.fromtimestamp(creationTime)).split(' ')
  creationDate = str(dateTime[0])
  todayDate = str(date.today())
  if(todayDate == creationDate):
    return True
  return False
def cfSolveCount(handle):
  print(handle)
  totalSolve = set()
  contestRanks = requests.get('https://codeforces.com/api/user.status?handle='+handle)
  sj = json.loads(contestRanks.content)
  submissions = sj['result']

  for sub in submissions:
    if 'verdict' not in sub: continue
    if(sub['verdict']=='OK' ):
      if(isValid(sub['creationTimeSeconds'])):
        pb = sub['problem']
        un = str(pb['contestId'])+pb['index']
        totalSolve.add(un)
      else:
        return len(totalSolve)
  return len(totalSolve)


