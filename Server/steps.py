import sqlite3
visits_db = '__HOME__/final_project/steps.db'

def request_handler(request):
    conn = sqlite3.connect(visits_db)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS data (user text,year int,month int,day int,steps int);''')
    if request['method']=='POST':
        user=request['form']['user']
        year=request['form']['year']
        year=int(year)
        month=request['form']['month']
        month=int(month)
        day=request['form']['day']
        day=int(day)
        steps=request['form']['steps']
        steps=int(steps)
        things = c.execute('''SELECT * FROM data WHERE user = ? AND year = ? AND month = ? AND day = ?;''',(user,year,month,day)).fetchall()
        if len(things)==0:
            c.execute('''INSERT into data VALUES (?,?,?,?,?);''', (user,year,month,day,steps))
            conn.commit() # commit commands
            conn.close() # close connection to database
            return 'Insert'
        else:
            c.execute('''UPDATE data SET steps = ? WHERE user = ? AND year = ? AND month = ? AND day = ?;''', (steps,user,year,month,day))
            conn.commit() # commit commands
            conn.close() # close connection to database
            return 'Update'
    else:
        user=request['values']['user']
        year=request['values']['year']
        year=int(year)
        month=request['values']['month']
        month=int(month)
        day=request['values']['day']
        day=int(day)
        if request['values']['action']=='check':
            things = c.execute('''SELECT steps FROM data WHERE user = ? AND year = ? AND month = ? AND day = ?;''',(user,year,month,day)).fetchall()
            if len(things)==0:
                conn.commit() # commit commands
                conn.close() # close connection to database
                return str(year)+'/'+str(month)+'/'+str(day)+': No Record'
            else:
                conn.commit() # commit commands
                conn.close() # close connection to database
                return str(year)+'/'+str(month)+'/'+str(day)+': '+str(things[0][0])
        if request['values']['action']=='initialize':
            out=''
            things = c.execute('''SELECT steps FROM data WHERE user = ? AND year = ? AND month = ? AND day = ?;''',(user,year,month,day)).fetchall()
            if(len(things)==0):
                out=str(0)
            else:
                out=str(things[0][0])
            for i in range(10):
                (year,month,day)=yesterday(year,month,day)
                things = c.execute('''SELECT steps FROM data WHERE user = ? AND year = ? AND month = ? AND day = ?;''',(user,year,month,day)).fetchall()
                if(len(things)==0):
                    out=str(0)+','+out
                else:
                    out=str(things[0][0])+','+out
            conn.commit() # commit commands
            conn.close() # close connection to database
            return out
            
def yesterday(year,month,day):
    if day>1:
        return (year,month,day-1)
    else:
        if month==1:
            return (year-1,12,31)
        if month==2 or month==4 or month==6 or month==8 or month==9 or month==11:
            return (year,month-1,31)
        if month==5 or month==7 or month==10 or month==12:
            return (year,month-1,30)
        if month==3:
            if year%400==0:
                return (year,2,29)
            elif year%100==0:
                return (year,2,28)
            elif year%4==0:
                return (year,2,29)
            else:
                return (year,2,28)
                
            
            
    
