HTTP Request:
====
POST /file-upload-fuzz/recv_dump.php HTTP/1.0
host: blog.security.localhost
content-type: multipart/form-data; boundary=----------ThIs_Is_tHe_bouNdaRY_$
content-length: 200

------------ThIs_Is_tHe_bouNdaRY_$
Content-Disposition: form-data; name="contents"; filename="/anything.here.slash-will-pass";
Content-Type: text/plain

any
------------ThIs_Is_tHe_bouNdaRY_$--

HTTP Response:
====
HTTP/1.1 200 OK
Date: Fri, 27 May 2011 11:35:08 GMT
Server: Apache/2.2.14 (Ubuntu)
X-Powered-By: PHP/5.3.2-1ubuntu4.9
Content-Length: 30
Connection: close
Content-Type: text/html

/anything.here.slash-will-pass

PHP script:
=====
<?php
if (!empty($_FILES['contents'])) {  // process file upload
    echo $_FILES['contents']['name'];
    unlink($_FILES['contents']['tmp_name']);
}
