import sqlite3
import datetime
visits_db = '__HOME__/final_project/friends.db'

def request_handler(request):
    conn = sqlite3.connect(visits_db)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS friends (user1 text,user2 text);''')
    c.execute('''CREATE TABLE IF NOT EXISTS location (user text,lat float,lon float,timing timestamp);''')
    c.execute('''CREATE TABLE IF NOT EXISTS messages (user1 text,user2 text,content text,flag int,timing timestamp);''')
    c.execute('''CREATE TABLE IF NOT EXISTS requests (user1 text,user2 text,flag int,timing timestamp);''')
    if request['method']=='POST':
        module=request['form']['module']
        if module=='message':
            user1=request['form']['user1']
            user2=request['form']['user2']
            content=request['form']['content']
            things = c.execute('''SELECT * FROM friends WHERE user1 = ? AND user2 = ?;''',(user1,user2)).fetchall()
            if len(things)==0:
                conn.commit() # commit commands
                conn.close() # close connection to database
                return user2+' is not your friend!'
            else:
                c.execute('''INSERT into messages VALUES (?,?,?,?,?);''', (user1,user2,content,0,datetime.datetime.now()))
                conn.commit() # commit commands
                conn.close() # close connection to database
                return 'Succeed!'
        if module=='add':
            action=request['form']['action']
            if action=='accept':
                user1=request['form']['user1']
                user2=request['form']['user2']
                c.execute('''INSERT into friends VALUES (?,?);''', (user1,user2))
                c.execute('''INSERT into friends VALUES (?,?);''', (user2,user1))
                c.execute('''INSERT into messages VALUES (?,?,?,?,?);''', ('system',user2,user1+' accepted your friend request!',0,datetime.datetime.now()))
                conn.commit() # commit commands
                conn.close() # close connection to database
                return 'Accepted!'
            if action=='refuse':
                user1=request['form']['user1']
                user2=request['form']['user2']
                c.execute('''INSERT into messages VALUES (?,?,?,?,?);''', ('system',user2,user1+' refused your friend request!',0,datetime.datetime.now()))
                conn.commit() # commit commands
                conn.close() # close connection to database
                return 'Refused!'
            if action=='send':
                user1=request['form']['user1']
                user2=request['form']['user2']
                things = c.execute('''SELECT * FROM friends WHERE user1 = ? AND user2 = ?;''',(user1,user2)).fetchall()
                if len(things)!=0:
                    conn.commit() # commit commands
                    conn.close() # close connection to database
                    return user2+' is already your friend!'
                else:
                    things = c.execute('''SELECT * FROM requests WHERE user1 = ? AND user2 = ? AND flag = ?;''',(user1,user2,0)).fetchall()
                    if len(things)!=0:
                        conn.commit() # commit commands
                        conn.close() # close connection to database
                        return 'Repeat sending request!'
                    else:
                        c.execute('''INSERT into requests VALUES (?,?,?,?);''', (user1,user2,0,datetime.datetime.now()))
                        conn.commit() # commit commands
                        conn.close() # close connection to database
                        return "Succeed!"
        if module=='nearby':
            user=request['form']['user']
            lat=request['form']['lat']
            lon=request['form']['lon']
            lat=float(lat)
            lon=float(lon)
            c.execute('''INSERT into location VALUES (?,?,?,?);''', (user,lat,lon,datetime.datetime.now()))
            conn.commit() # commit commands
            conn.close() # close connection to database
            return 'Position!'
    else:
        module=request['values']['module']
        if module=='message':
            user=request['values']['user']
            space=request['values']['space']
            length=int(space)
            things = c.execute('''SELECT * FROM messages WHERE user2 = ? AND flag = ? ORDER BY timing ASC;''',(user,0)).fetchall()
            if len(things)<length:
                length=len(things)
            out=''
            if length!=0:
                for i in range(length-1):
                    (user1,user2,content,flag,timing)=things[i]
                    out+='From '
                    out+=user1
                    out+=':\n'
                    out+=content
                    out+=','
                    c.execute('''UPDATE messages SET flag = ? WHERE user1 = ? AND user2 = ? AND content = ? AND timing = ?;''', (1,user1,user2,content,timing))
                (user1,user2,content,flag,timing)=things[length-1]
                out+='From '
                out+=user1
                out+=':\n'
                out+=content
                c.execute('''UPDATE messages SET flag = ? WHERE user1 = ? AND user2 = ? AND content = ? AND timing = ?;''', (1,user1,user2,content,timing))
            conn.commit() # commit commands
            conn.close() # close connection to database
            return out
        if module=='add':
            user=request['values']['user']
            space=request['values']['space']
            length=int(space)
            things = c.execute('''SELECT * FROM requests WHERE user2 = ? AND flag = ? ORDER BY timing ASC;''',(user,0)).fetchall()
            if len(things)<length:
                length=len(things)
            out=''
            if length!=0:
                for i in range(length-1):
                    (user1,user2,flag,timing)=things[i]
                    out+=user1
                    out+=','
                    c.execute('''UPDATE requests SET flag = ? WHERE user1 = ? AND user2 = ? AND timing = ?;''', (1,user1,user2,timing))
                (user1,user2,flag,timing)=things[length-1]
                out+=user1
                c.execute('''UPDATE requests SET flag = ? WHERE user1 = ? AND user2 = ? AND timing = ?;''', (1,user1,user2,timing))
            conn.commit() # commit commands
            conn.close() # close connection to database
            return out
        if module=='nearby':
            user=request['values']['user']
            twenty_s_ago = datetime.datetime.now()- datetime.timedelta(seconds = 20)
            things = c.execute('''SELECT * FROM location WHERE user = ? AND timing > ? ORDER BY timing DESC;''',(user,twenty_s_ago)).fetchall()
            if len(things)==0:
                conn.commit() # commit commands
                conn.close() # close connection to database
                return 'Please check your GPS!'
            else:
                (me,mylat,mylon,timing)=things[0]
                things = c.execute('''SELECT * FROM friends WHERE user1 = ?;''',(user,)).fetchall()
                out='Nearby Friends (200m):\n'
                number=0
                for x in things:
                    (me,friend)=x
                    things = c.execute('''SELECT * FROM location WHERE user = ? AND timing > ? ORDER BY timing DESC;''',(friend,twenty_s_ago)).fetchall()
                    if len(things)!=0:
                        (friend,lat,lon,timing)=things[0]
                        if ((mylat-lat)**2+(mylon-lon)**2)**0.5<0.002:
                            number+=1
                            out+=friend
                            out+='\n'
                if number==0:
                    conn.commit() # commit commands
                    conn.close() # close connection to database
                    return 'No friend is nearby'
                else:
                    conn.commit() # commit commands
                    conn.close() # close connection to database
                    return out
                    


            
    