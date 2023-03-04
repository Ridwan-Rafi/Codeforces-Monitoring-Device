import firebase_admin
from firebase_admin import credentials
from firebase_admin import db
from TotalSolve import cfSolveCount
from validUser import  isValidUser

#mainDatabase
cred = credentials.Certificate('codeforce.json')
firebase_admin.initialize_app(cred, {'databaseURL': 'https://codeforces-fbdd0-default-rtdb.firebaseio.com'})

#testDatabase
#cred = credentials.Certificate('IOT.json')
#firebase_admin.initialize_app(cred, {'databaseURL': 'https://iotlab2-32f25-default-rtdb.firebaseio.com'})


def UpdateSolveCount(id,count):
    ref = db.reference('/TodaySolve/')
    ref.update({
        id:count
    })
def UpdateValidData(id,flag):
    ref = db.reference('/Valid/')
    ref.update({
        id:flag
    })
def markValidUser():
    ref = db.reference('/').get()
    user = ref['User']
    valid = ref['Valid']
    for data in user:
        if (data != '*' and valid[data] ==-1):
            count = isValidUser(user[data])
            UpdateValidData(data, count)
        else:
            print(data+" ok")


def databaseUpdate():
    ref = db.reference('/').get()
    user = ref['User']
    valid = ref['Valid']
    for data in user:
        if (data != '*' and valid[data]==1):
            count = cfSolveCount(user[data])
            print(data+" "+str(count))
            UpdateSolveCount(data,cfSolveCount(user[data]))
