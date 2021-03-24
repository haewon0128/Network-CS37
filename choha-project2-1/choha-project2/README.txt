****************

1. First type: make all 
   in server side

2. type: ftserver [port number]
   in server side. the port number is the port that you want

3. on another window, for list
   type: python ftclient.py [serverhost] [serverport] -l [newportnumber]
   for file
   type: python ftclient.py [serverhost] [serverport] -g [filename] [newportnumber]
   the server host should be the server that server side run on such as flip1, flip2..
   the server port should be same with the server run on.

4. when it asks if the user wants to overwrite the file, user should type Y or N
   Y for overwriting, N for not overwritng

5. client can keep sending request until the server is ready to accept

****************
