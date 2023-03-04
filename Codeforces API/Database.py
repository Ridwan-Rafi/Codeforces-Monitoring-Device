import firebase_admin
from firebase_admin import credentials
from firebase_admin import db
from main import cfSolveCount

cred = credentials.Certificate('codeforce.json')
firebase_admin.initialize_app(cred, {'databaseURL': 'https://codeforces-fbdd0-default-rtdb.firebaseio.com'})

def UpdateData(id,count):
    ref = db.reference('/TodaySolve/')
    ref.update({
        id:count
    })
def databaseUpdate():
    ref = db.reference('/').get()
    user = ref['User']
    for data in user:
        if (data != '*'):
            count = cfSolveCount(user[data])
            print(data+" "+str(count))
            UpdateData(data,cfSolveCount(user[data]))

while(True):
    databaseUpdate()