import requests

def request_handler(request):
     try:
 
         if request['method'] == 'POST':
             return "POST requests not allowed."
         x = float(request['values']['lon'])
         y = float(request['values']['lat'])
         key = request['values']['key']
         if key == "temp" or key == "vis":
             request = "https://api.openweathermap.org/data/2.5/weather?lon={}&lat={}&APPID=KEY".format(x, y)
             r = requests.get(request)
             res = r.json()
             if key == "temp":
                 return res['main']['temp']
 
             if key == "vis":
                 return res['weather'][0]['main']
 
         elif key == "date" or key == "time":
             request = "http://api.timezonedb.com/v2.1/get-time-zone?key=KEY&format=json&by=position&lng={}&lat={}".format(x, y)
             is_good=False
             while not is_good:
                 try:
                     r = requests.get(request)
                     res = r.json()
                     timedate = res["formatted"]
                     date=timedate[:10]
                     time=timedate[11:]
                     is_good=True
                 except:
                     is_good=False
             if key == "date":
                 return date
 
 
             if key == "time":
                 return time
 
         else: raise ValueError
 
     except KeyError:
         return "You must enter valid key, lon and lat values."
 
     except ValueError:
         return "Both lon and lat must be valid numbers. Key should be one of time, date, vis, or temp"
 
 

