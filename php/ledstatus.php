<?
   $fh = fopen("ledstatus.txt", 'r');
   echo fread($fh, 4);
   fclose($fh);
?>
